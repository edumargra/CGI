#include "mesh.h"

#include <cmath>
using namespace std;

Hit Mesh::intersect(Ray const &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, triangles
    * Sought: intersects? if true: *t
    *
    * Insert calculation of ray/mesh intersection here.
    *
    * For each triangle we find which ones it intersects
    * and we find the minimum hit (closest) of all of these.
    ****************************************************/

    // place holder for actual intersection calculation
    Hit min_hit = Hit::NO_HIT();
    Hit hit = Hit::NO_HIT();
    for(auto &triangle: triangles){
        hit = triangle.intersect(ray);
        if(!isnan(hit.t) && (isnan(min_hit.t) || hit.t < min_hit.t)){
            min_hit = hit;
        }
    }
    if(isnan(min_hit.t)) return Hit::NO_HIT();
    return min_hit;
}

Mesh::Mesh(vector<Vertex> const &vertices,double const &scale, Point const &translate){
    uint i;
    Point p1,p2,p3;
    Triangle* t;
    for(i = 0; i < vertices.size(); i+=3){
        p1 = *(new Point (vertices[i].x,vertices[i].y,vertices[i].z)) * scale + translate;
        p2 = *(new Point (vertices[i+1].x,vertices[i+1].y,vertices[i+1].z)) * scale + translate;
        p3 = *(new Point (vertices[i+2].x,vertices[i+2].y,vertices[i+2].z)) * scale + translate;
        t = new Triangle(p1,p2,p3);
        triangles.push_back(*t);
    }
    
}
