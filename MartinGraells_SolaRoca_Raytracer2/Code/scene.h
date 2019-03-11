#ifndef SCENE_H_
#define SCENE_H_

#include "light.h"
#include "object.h"
#include "triple.h"
#include "image.h"


#include <vector>

// Forward declerations
class Ray;
class Image;

class Scene
{
    std::vector<ObjectPtr> objects;
    std::vector<LightPtr> lights;   // no ptr needed, but kept for consistency
    Point eye;
    std::map<std::string,Image> textures; //array of textures to onl load once per object

    public:

        // trace a ray into the scene and return the color
        Color trace(Ray const &ray, bool shadows = false, int reflection = 0);

        // render the scene to the given image
        void render(Image &img);

        void setShadows();
        void setMaxRecursionDepth(int depth);
        void setSuperSamplingFactor(int factor);

        void addObject(ObjectPtr obj);
        void addLight(Light const &light);
        void setEye(Triple const &position);

        unsigned getNumObject();
        unsigned getNumLights();

    private:
        Vector vectorReflect(Vector v,Vector N);
        bool shadows = false;
        int maxRecursionDepth = 0;
        unsigned superSamplingFactor = 1;
};

#endif
