#ifndef CYLINDER_H_
#define CYLINDER_H_

#include "../object.h"

class Cylinder: public Object
{
    public:
        Cylinder(Point const &pos1,Point const &pos2, double radius);

        virtual Hit intersect(Ray const &ray);
        virtual std::vector<float> UVcoord(Vector v);
        
        Point const initial;
        Point const end;
        double const r;
};

#endif
