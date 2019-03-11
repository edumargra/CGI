#ifndef SPHERE_H_
#define SPHERE_H_

#include "../object.h"

class Sphere: public Object
{
    public:
        Sphere(Point const &pos, double radius);
        Sphere(Point const &pos, double radius,double angle,Vector const &axis);

        virtual Hit intersect(Ray const &ray);
        virtual std::vector<float> UVcoord(Vector v);
        Vector applyRotation(Vector v);
        Point const position;
        double const r;
        double const angle = 0;
        Vector const axis = Vector(0,0,0);
};

#endif
