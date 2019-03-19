#include "sphere.h"
#include "solvers.h"

#include <cmath>
#include <iostream>

using namespace std;

Hit Sphere::intersect(Ray const &ray)
{
    // Sphere formula: ||x - position||^2 = r^2
    // Line formula:   x = ray.O + t * ray.D

    Vector L = ray.O - position;
    double a = ray.D.dot(ray.D);
    double b = 2 * ray.D.dot(L);
    double c = L.dot(L) - r * r;

    double t0;
    double t1;
    if (not Solvers::quadratic(a, b, c, t0, t1))
        return Hit::NO_HIT();

    // t0 is closest hit
    if (t0 < 0)  // check if it is not behind the camera
    {
        t0 = t1;    // try t1
        if (t0 < 0) // both behind the camera
            return Hit::NO_HIT();
    }

    // calculate normal
    Point hit = ray.at(t0);
    Vector N = (hit - position).normalized();

    // determine orientation of the normal
    if (N.dot(ray.D) > 0)
        N = -N;

    return Hit(t0, N);
}

vector<float> Sphere::UVcoord(Vector v){ //v is initially a point in space coord
  v = v - position; //v is now the vector from the center of the sphere to the point passsed as a parameter
  v = applyRotation(v); //we rotate the sphere (only the texture point)
  vector<float> newCoord;
  float zeta = acos(v.z/r); //from spherical coord we find zeta and phi
  float phi = atan2(v.y, v.x);
  if(phi < 0) phi += 2*M_PI; //our space for u is between 0 and 1
  newCoord.push_back(0.5+(phi/(2*M_PI))); //u
  newCoord.push_back((zeta)/M_PI); //v
  return newCoord;
}

Sphere::Sphere(Point const &pos, double radius)
:
    position(pos),
    r(radius)
{}

Sphere::Sphere(Point const &pos, double radius,double angle, Vector const &axis)
:
    position(pos),
    r(radius),
    angle(angle*M_PI/180),
    axis(axis.normalized())
{}

Vector Sphere::applyRotation(Vector v){
    return v*cos(angle) + axis.cross(v)*sin(angle) + axis*v.dot(axis)*(1-cos(angle)); //Rodrigues' rotation formula https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
}
