#include "cylinder.h"

#include <cmath>

using namespace std;

Hit Cylinder::intersect(Ray const &ray)
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
    //Project line in circle plane
    Vector v1 (0.0,0.0,ray.O.z - initial.z);
    Point projected_origin = ray.O - v1;
    Vector v2 (0.0,0.0,ray.D.z);
    Vector projected_direction = ray.D - v2;
    //find circle intersection
    Vector OC = initial - projected_origin;
    float tca = OC.dot(projected_direction);
    if ( tca < 0) {
        return Hit::NO_HIT();
    }
    
    float d2 = OC.dot(OC) - pow(tca,2);
    if (d2 > pow(r,2)) return Hit::NO_HIT();
    float thc = sqrt(pow(r,2) - d2);
    float t0 = tca - thc;
    float t1 = tca + thc; 
    
    if (t0 > t1) std::swap(t0, t1);

    if (t0 < 0) {
        t0 = t1; // if t0 is negative, let's use t1 instead
        if (t0 < 0) return Hit::NO_HIT(); // both t0 and t1 are negative
    }

    float t = t0; 
    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    *
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/
    Vector projected_intersection (ray.at(t).x,ray.at(t).y,initial.z);
    Vector v = projected_intersection - initial;
    Vector height (0.0,0.0,ray.at(t).z);
    Vector N = v + height;

    return Hit(t,N);
}

Cylinder::Cylinder(Point const &pos1,Point const &pos2,double radius)
:
    initial(pos1),
    end(pos2),
    r(radius)
    
{}
