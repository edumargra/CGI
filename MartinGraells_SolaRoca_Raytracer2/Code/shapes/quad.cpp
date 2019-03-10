#include "quad.h"

#include <cmath>
#include <iostream>
using namespace std;

Hit Quad::intersect(Ray const &ray)
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

    Hit hit1 = triangle1.intersect(ray);
    Hit hit2 = triangle2.intersect(ray);
    if (!std::isnan(hit1.t)) {
        if(hit1.t < hit2.t || std::isnan(hit2.t)){
            return hit1;
        }
    }
    if (!std::isnan(hit2.t)) return hit2;
    return Hit::NO_HIT();
}

vector<float> Quad::UVcoord(Vector v){
  vector<float> newCoord = {0,0};
  return newCoord;
}

Quad::Quad(Point const &pos1, Point const &pos2, Point const &pos3, Point const &pos4)
:
    triangle1(pos1,pos2,pos4),
    triangle2(pos2,pos3,pos4)
{}
