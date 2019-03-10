#ifndef PLANE_H_
#define PLANE_H_

#include "../object.h"

class Plane: public Object
{
    public:
        Plane(Point const &pos1, Point const &pos2, Point const &pos3);

        virtual Hit intersect(Ray const &ray);
        virtual std::vector<float> UVcoord(Vector v);
        
        Point const point1;
        Point const point2;
        Point const point3;
};

#endif
