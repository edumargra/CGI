#include "triangle.h"

#include <cmath>

using namespace std;

Hit Triangle::intersect(Ray const &ray)
{
        
    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: triangle vertices
    * Sought: N
    *
    ****************************************************/

    Vector u = vertex2 - vertex1; //vector ab
    Vector v = vertex3 - vertex1; //vector ac
    Vector N = (u.cross(v)).normalized();
    
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    *
    * Insert calculation of ray/sphere intersection here.
    *
    * You have the sphere's center (C) and radius (r) as well as
    * 
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/
    
    float dotProduct = N.dot(ray.D);
    if(dotProduct >= 0) return Hit::NO_HIT(); //paralel or same direction as normal
    float t = ((vertex1 - ray.O).dot(N))/N.dot(ray.D);
    Point point = ray.at(t);
    Vector w = point-vertex1; //vector w
    Vector n = u.cross(v);
    float gamma = (u.cross(w)).dot(n)/n.dot(n); //baricentric coordinate respect vertex1  
    float beta = (w.cross(v)).dot(n)/n.dot(n); //baricentric coordinate respect vertex2
    float alfa = 1-gamma-beta; //baricentric coordinate respect vertex3
    if(!(alfa >= 0 && alfa <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1)){
            return Hit::NO_HIT();
    }
    return Hit(t,N);
}

Triangle::Triangle(Point const &pos1, Point const &pos2, Point const &pos3)
:
    vertex1(pos1),
    vertex2(pos2),
    vertex3(pos3)
{}