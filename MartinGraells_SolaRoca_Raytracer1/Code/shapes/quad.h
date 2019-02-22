#ifndef QUAD_H_
#define QUAD_H_

#include "../object.h"
#include "triangle.h"
class Quad: public Object
{
    public:
        Quad(Point const &pos1, Point const &pos2, Point const &pos3, Point const &pos4);

        virtual Hit intersect(Ray const &ray);

        Triangle triangle1;
        Triangle triangle2;
};

#endif
