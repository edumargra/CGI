#include "plane.h"

#include <cmath>

using namespace std;

Hit Plane::intersect(Ray const &ray)
{

    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: triangle vertices
    * Sought: N
    *
    ****************************************************/

    Vector u = point2 - point1; //vector ab
    Vector v = point3 - point1; //vector ac
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
    float t = ((point1 - ray.O).dot(N))/N.dot(ray.D);
    return Hit(t,N);
}

vector<float> Plane::UVcoord(Vector v){
  vector<float> newCoord = {0,0};
  return newCoord;
}


Plane::Plane(Point const &pos1, Point const &pos2, Point const &pos3)
:
    point1(pos1),
    point2(pos2),
    point3(pos3)
{}
