#ifndef MESH_H_
#define MESH_H_

#include "../object.h"
#include "triangle.h"
#include "../vertex.h"

class Mesh: public Object
{
    public:
        Mesh(std::vector<Vertex> const &vertices,double const &scale, Point const &translate);

        virtual Hit intersect(Ray const &ray);

        std::vector<Triangle> triangles;
};

#endif
