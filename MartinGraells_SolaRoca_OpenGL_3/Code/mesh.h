#ifndef MESH
#define MESH

#include <QVector3D>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>


struct Mesh
{
    QVector3D location;
    QVector3D orientation;
    QVector3D speed;
    QVector3D rotation;
    GLuint vao;
    GLuint vbo;
    int numVertices;
    GLuint texturePtr;
    QMatrix4x4 model;
    QMatrix3x3 normalMatrix;

    Mesh(){}

    Mesh(QVector3D location,QVector3D orientation,QVector3D speed,QVector3D rotation,int numVertices)
        :
          location(location),
          orientation(orientation),
          speed(speed),
          numVertices(numVertices),
          rotation(rotation)
    {   }
};


#endif // MESH

