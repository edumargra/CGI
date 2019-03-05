#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>

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
    
    
    Color Ia = material.color*material.ka;
    Color Id(0.0,0.0,0.0) ;Color Is(0.0,0.0,0.0) ; 
    Color reflectedColor(0.0,0.0,0.0);
    bool blocked = false; //by default there are no shadows
    uint i;
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
            if(reflection>0 && reflection <4) {
               reflectedColor += trace(Ray(hit,vectorReflect(ray.D,N)),false,reflection+1);
            }
            Id += material.color*lights[i]->color*material.kd*max(0.0,L.dot(N));
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
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            Point pixel(x + 0.5, h - 1 - y + 0.5, 0);
            Ray ray(eye, (pixel - eye).normalized());
            Color col = trace(ray,shadows,maxRecursionDepth);
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
