#include "mesh.h"

#include <cmath>
#include <iostream>
using namespace std;

Hit Mesh::intersect(Ray const &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    *
    * Insert calculation of ray/sphere intersection here.
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/

    // place holder for actual intersection calculation
    Hit min_hit = Hit::NO_HIT();
    for(auto &triangle: triangles){
        Hit hit = triangle.intersect(ray);
        if(!std::isnan(hit.t) && (std::isnan(min_hit.t) || hit.t < min_hit.t)){
            min_hit = hit;
        }
    }
    if(std::isnan(min_hit.t)) return Hit::NO_HIT();
    return min_hit;
}

Mesh::Mesh(std::vector<Vertex> const &vertices){
    uint i;
    Point p(0,0,0);
    Triangle* t;
    for(i = 0; i < vertices.size(); i+=3){
        cout<< vertices[i].x<<","<<vertices[i].y<<","<<vertices[i].z<<"\n";
        t = new Triangle(vertices[i],vertices[i+1],vertices[i+2]);
        triangles.push_back(*t);
    }
    
}
