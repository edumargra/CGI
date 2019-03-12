#ifndef MESH
#define MESH

#include <QVector3D>
#include <QOpenGLShaderProgram>


struct Mesh
{
    QVector3D location;
    QVector3D orientation;
    QVector3D speed;
    GLuint vao;
    int numVertices;
    GLuint texturePtr;

    Mesh(){}

    Mesh(QVector3D location,QVector3D orientation,QVector3D speed,int numVertices)
        :
          location(location),
          orientation(orientation),
          speed(speed),
          numVertices(numVertices)
    {   }
};


#endif // MESH

