#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../object.h"
#include "../vertex.h"

class Triangle: public Object
{
    public:
        Triangle(Point const &pos1, Point const &pos2, Point const &pos3);
        virtual Hit intersect(Ray const &ray);

        Point const vertex1;
        Point const vertex2;
        Point const vertex3;
};

#endif
