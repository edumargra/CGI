#include "sphere.h"
#include "solvers.h"

#include <cmath>

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

vector<float> Sphere::UVcoord(Vector v){
  vector<float> newCoord;
  float zeta = acos((v.z - position.z)/r);
  float sigma = atan2(v.y - position.y, v.x - position.x);
  if(sigma < 0) sigma += 2*M_PI; //our space for u is between 0 and 1
  newCoord.push_back(sigma/2*M_PI); //u
  newCoord.push_back((M_PI - zeta)/M_PI); //v
}

Sphere::Sphere(Point const &pos, double radius)
:
    position(pos),
    r(radius)
{}
