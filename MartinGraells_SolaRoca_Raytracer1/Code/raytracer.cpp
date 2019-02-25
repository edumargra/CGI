#include "raytracer.h"
#include "objloader.h"
#include "image.h"
#include "light.h"
#include "material.h"
#include "triple.h"

// =============================================================================
// -- Include all your shapes here ---------------------------------------------
// =============================================================================

#include "shapes/sphere.h"
#include "shapes/triangle.h"
#include "shapes/plane.h"
#include "shapes/cylinder.h"
#include "shapes/quad.h"
#include "shapes/mesh.h"
// =============================================================================
// -- End of shape includes ----------------------------------------------------
// =============================================================================

#include "json/json.h"

#include <exception>
#include <fstream>
#include <iostream>

using namespace std;        // no std:: required
using json = nlohmann::json;

bool Raytracer::parseObjectNode(json const &node)
{
    ObjectPtr obj = nullptr;

// =============================================================================
// -- Determine type and parse object parametrers ------------------------------
// =============================================================================

    if (node["type"] == "sphere")
    {
        Point pos(node["position"]);
        double radius = node["radius"];
        obj = ObjectPtr(new Sphere(pos, radius));
    }
    else if(node["type"] == "triangle")
    {
        Point pos1(node["vertex1"]);
        Point pos2(node["vertex2"]);
        Point pos3(node["vertex3"]);
        obj = ObjectPtr(new Triangle(pos1,pos2,pos3));
    }else if(node["type"] == "plane")
    {
        Point pos1(node["point1"]);
        Point pos2(node["point2"]);
        Point pos3(node["point3"]);
        obj = ObjectPtr(new Plane(pos1,pos2,pos3));
    }else if(node["type"] == "cylinder")
    {
        Point pos1(node["initial_position"]);
        Point pos2(node["end_position"]);
        double radius = node["radius"];
        obj = ObjectPtr(new Cylinder(pos1,pos2,radius));
    }else if(node["type"] == "quad")
    {
        Point pos1(node["vertex1"]);
        Point pos2(node["vertex2"]);
        Point pos3(node["vertex3"]);
        Point pos4(node["vertex4"]);
        obj = ObjectPtr(new Quad(pos1,pos2,pos3,pos4));
    }else if(node["type"] == "mesh")
    {
        std::string url = node["model"];
        OBJLoader objLoader(url);
        obj = ObjectPtr(new Mesh(objLoader.vertex_data()));
    }else{
        cerr << "Unknown object type: " << node["type"] << ".\n";
    }

// =============================================================================
// -- End of object reading ----------------------------------------------------
// =============================================================================

    if (!obj)
        return false;

    // Parse material and add object to the scene
    obj->material = parseMaterialNode(node["material"]);
    scene.addObject(obj);
    return true;
}

Light Raytracer::parseLightNode(json const &node) const
{
    Point pos(node["position"]);
    Color col(node["color"]);
    return Light(pos, col);
}

Material Raytracer::parseMaterialNode(json const &node) const
{
    Color color(node["color"]);
    double ka = node["ka"];
    double kd = node["kd"];
    double ks = node["ks"];
    double n  = node["n"];
    return Material(color, ka, kd, ks, n);
}

bool Raytracer::readScene(string const &ifname)
try
{
    // Read and parse input json file
    ifstream infile(ifname);
    if (!infile) throw runtime_error("Could not open input file for reading.");
    json jsonscene;
    infile >> jsonscene;

// =============================================================================
// -- Read your scene data in this section -------------------------------------
// =============================================================================

    Point eye(jsonscene["Eye"]);
    scene.setEye(eye);

    // TODO: add your other configuration settings here

    for (auto const &lightNode : jsonscene["Lights"])
        scene.addLight(parseLightNode(lightNode));

    unsigned objCount = 0;
    for (auto const &objectNode : jsonscene["Objects"])
        if (parseObjectNode(objectNode))
            ++objCount;

    cout << "Parsed " << objCount << " objects.\n";

// =============================================================================
// -- End of scene data reading ------------------------------------------------
// =============================================================================

    return true;
}
catch (exception const &ex)
{
    cerr << ex.what() << '\n';
    return false;
}

void Raytracer::renderToFile(string const &ofname)
{
    // TODO: the size may be a settings in your file
    Image img(400, 400);
    cout << "Tracing...\n";
    scene.render(img);
    cout << "Writing image to " << ofname << "...\n";
    img.write_png(ofname);
    cout << "Done.\n";
}
