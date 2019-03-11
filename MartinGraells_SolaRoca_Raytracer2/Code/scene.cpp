#include "scene.h"

#include "hit.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>
#include <iostream>


using namespace std;

Color Scene::trace(Ray const &ray, bool shadows,int reflection)
{
    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    for (unsigned idx = 0; idx != objects.size(); ++idx)
    {
        Hit hit(objects[idx]->intersect(ray));
        if (hit.t < min_hit.t)
        {
            min_hit = hit;
            obj = objects[idx];
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    Material material = obj->material;          //the hit objects material
    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector
    Color materialColor = material.color;
    if(material.texture != string("")){
      if(!textures.count(material.texture)){
        Image texture(string("../Scenes/") + material.texture); //texture
        textures[material.texture] = texture;
      }
      Image texture = textures[material.texture];
      vector<float> UVcoord = obj->UVcoord(hit); //coord in UV space of the hit point
      materialColor = texture.colorAt(UVcoord.at(0),UVcoord.at(1)); //color of texture at UV
    }
    
    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
    *
    * Given: material, hit, N, V, lights[]
    * Sought: color
    *
    * Hints: (see triple.h)
    *        Triple.dot(Vector) dot product
    *        Vector + Vector    vector sum
    *        Vector - Vector    vector difference
    *        Point - Point      yields vector
    *        Vector.normalize() normalizes vector, returns length
    *        double * Color     scales each color component (r,g,b)
    *        Color * Color      dito
    *        pow(a,b)           a to the power of b
    ****************************************************/


    Color Ia = materialColor*material.ka;
    Color Id(0.0,0.0,0.0) ;Color Is(0.0,0.0,0.0) ;
    bool blocked = false; //by default there are no shadows
    uint i;
    Color reflectedColor(0.0,0.0,0.0);
    for(i=0; i < lights.size(); i++){
        Vector L = (lights[i]->position - hit).normalized();
        if(shadows){
            blocked = false; //we clean the previous value to false
            Ray lightRay(lights[i]->position,-L);
            Hit min_hit(obj->intersect(lightRay));
            for (unsigned idx = 0; idx != objects.size() && !blocked; ++idx)
            {
                Hit hit2(objects[idx]->intersect(lightRay));
                if (hit2.t < min_hit.t) //if there is an object between the light and the original light
                {
                    blocked = true;
                }
            }
        }
        if(!blocked){
            Vector R = vectorReflect(L,N);
            Ray r(hit + 0.1*vectorReflect(V,N) ,vectorReflect(V,N));
            if(reflection>0 && material.ks > 0) {
               reflectedColor = trace(r,true,reflection-1);
               //printf("reflectedColor: (%f,%f,%f), reflection step: %i \n",reflectedColor.r,reflectedColor.g,reflectedColor.b,reflection);
            }
            Id += materialColor*lights[i]->color*material.kd*max(0.0,L.dot(N));
            Is += pow(max(0.0,R.dot(V)),material.n)*material.ks*lights[i]->color;
        }
    }
    Color color = Id + Is + Ia + material.ks*reflectedColor;

    return color;
}


void Scene::render(Image &img)
{
    unsigned w = img.width();
    unsigned h = img.height();
    float factor = 1.f /(superSamplingFactor+1); //space between rays
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
          Color col(0.0,0.0,0.0);
          for(unsigned k = 1; k <= superSamplingFactor; k++){
            for(unsigned g = 1; g <= superSamplingFactor; g++){
              Point pixel(x + g*factor, h - 1 - y + k*factor, 0);
              Ray ray(eye, (pixel - eye).normalized());
              col += trace(ray,shadows,maxRecursionDepth);
            }
          }
          col = col/(superSamplingFactor*2); //average of the rays for one pixel
          col.clamp();
          img(x, y) = col;
        }
    }
}

void Scene::setShadows(){
    shadows = true;
}

void Scene::setMaxRecursionDepth(int depth){
    maxRecursionDepth = depth;
}

void Scene::setSuperSamplingFactor(int factor){
    superSamplingFactor = factor/2; //same number of rays in each direction
}

//Returns the reflection of v with respect to N, normalized
Vector Scene::vectorReflect(Vector v,Vector N){
    return (2*(N.dot(v))*N-v).normalized();
}

// --- Misc functions ----------------------------------------------------------

void Scene::addObject(ObjectPtr obj)
{
    objects.push_back(obj);
}

void Scene::addLight(Light const &light)
{
    lights.push_back(LightPtr(new Light(light)));
}

void Scene::setEye(Triple const &position)
{
    eye = position;
}

unsigned Scene::getNumObject()
{
    return objects.size();
}

unsigned Scene::getNumLights()
{
    return lights.size();
}
