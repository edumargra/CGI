#include "mainview.h"
#include "math.h"

#include <QDateTime>

/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
    debugLogger->stopLogging();

    qDebug() << "MainView destructor";
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::describeVertex()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),nullptr);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<GLvoid*>((3*sizeof(float))));
}

void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    debugLogger = new QOpenGLDebugLogger();
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ),
             this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if ( debugLogger->initialize() ) {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger->enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    // Set the color of the screen to be black on clear (new frame)
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);

    createShaderProgram();

    //Definition of the vertices that form the cube and the pyramid
    //n/f=near/far, t/b=top/bottom, r/l=right/left
    Vertex ntr,nbr,ftr,fbr,ntl,nbl,ftl,fbl,topOfPyramid;

    ntr.x=1;
    ntr.y=1;
    ntr.z=1;
    ntr.r=1;
    ntr.g=1;
    ntr.b=1;

    nbr.x=1;
    nbr.y=1;
    nbr.z=-1;
    nbr.r=1;
    nbr.g=1;
    nbr.b=0;

    ftr.x=1;
    ftr.y=-1;
    ftr.z=1;
    ftr.r=1;
    ftr.g=0;
    ftr.b=1;

    fbr.x=1;
    fbr.y=-1;
    fbr.z=-1;
    fbr.r=1;
    fbr.g=0;
    fbr.b=0;

    ntl.x=-1;
    ntl.y=1;
    ntl.z=1;
    ntl.r=0;
    ntl.g=1;
    ntl.b=1;

    nbl.x=-1;
    nbl.y=1;
    nbl.z=-1;
    nbl.r=0;
    nbl.g=1;
    nbl.b=0;

    ftl.x=-1;
    ftl.y=-1;
    ftl.z=1;
    ftl.r=0;
    ftl.g=0;
    ftl.b=1;

    fbl.x=-1;
    fbl.y=-1;
    fbl.z=-1;
    fbl.r=0;
    fbl.g=0;
    fbl.b=0;

    topOfPyramid.x=0;
    topOfPyramid.y=0;
    topOfPyramid.z=1;
    topOfPyramid.r=0;
    topOfPyramid.g=0;
    topOfPyramid.b=1;

    //cube defined by two trianles per face
    std::vector<Vertex> cube = {fbl,fbr,ftl,fbr,ftr,ftl,fbr,nbr,ftr,nbr,ntr,ftr,nbr,nbl,ntr,nbl,ntl,ntr,fbl,ftl,ntl,nbl,fbl,ntl,ftl,ftr,ntl,ftr,ntr,ntl,fbl,nbl,fbr,fbr,nbl,nbr};
    std::vector<Vertex> pyramid = {fbl,nbl,fbr,fbr,nbl,nbr,fbl,fbr,topOfPyramid,fbr,nbr,topOfPyramid,nbr,nbl,topOfPyramid,nbl,fbl,topOfPyramid};


    //we send the cube to the gpu
    glGenBuffers(1, &vbo_cube);
    glGenVertexArrays(1, &vao_cube);
    glBindVertexArray(vao_cube);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
    glBufferData(GL_ARRAY_BUFFER,sizeof(Vertex)*36,cube.data(), GL_DYNAMIC_DRAW);
    //Describe Vertices
    describeVertex();

    //we send the pyramid to the gpu
    glGenBuffers(1, &vbo_pyramid);
    glGenVertexArrays(1, &vao_pyramid);
    glBindVertexArray(vao_pyramid);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid);
    glBufferData(GL_ARRAY_BUFFER,sizeof(Vertex)*18,pyramid.data(), GL_DYNAMIC_DRAW);
    describeVertex();

    //Load sphere
    sphere = new Model(":/models/sphere.obj");
    QVector<QVector3D> vertices = sphere->getVertices();
    std::vector<Vertex> sphereVertices;
    Vertex temporaryVertex;
    //Vertices in order with color
    for(auto const &vertex: vertices){
        temporaryVertex.x = vertex.x();
        temporaryVertex.y = vertex.y();
        temporaryVertex.z = vertex.z();
        temporaryVertex.r = static_cast<float>(rand()) / RAND_MAX;
        temporaryVertex.g = static_cast<float>(rand()) / RAND_MAX;
        temporaryVertex.b = static_cast<float>(rand()) / RAND_MAX;
        sphereVertices.push_back(temporaryVertex);
    }
    //Pass it to the GPU
    glGenBuffers(1, &vbo_sphere);
    glGenVertexArrays(1, &vao_sphere);
    glBindVertexArray(vao_sphere);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere);
    glBufferData(GL_ARRAY_BUFFER,sizeof(Vertex)*vertices.size(),sphereVertices.data(), GL_DYNAMIC_DRAW);
    describeVertex();

    //Create and translate all the models and scale the sphere
    setAndTranslateModels();
    //Projection of the scene
    projection.setToIdentity();
    projection.perspective(60,static_cast<float>(width())/height(),0.4f,20);

    previousScale = 100;
    previousRotation = {0,0,0};

}

void MainView::createShaderProgram()
{
    // Create shader program
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                          ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader.glsl");
    shaderProgram.link();

    modelLocation = shaderProgram.uniformLocation("modelTransform");
    projectionLocation = shaderProgram.uniformLocation("projectionTransform");
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // Clear the screen before rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.bind();
    glUniformMatrix4fv(projectionLocation,1,GL_FALSE,projection.data());

    //Draw cube
    glUniformMatrix4fv(modelLocation,1,GL_FALSE,model_cube.data());
    glBindVertexArray(vao_cube);
    glDrawArrays(GL_TRIANGLES,0,36);
    //Draw pyramid
    glUniformMatrix4fv(modelLocation,1,GL_FALSE,model_pyramid.data());
    glBindVertexArray(vao_pyramid);
    glDrawArrays(GL_TRIANGLES,0,18);
    //Draw sphere
    glUniformMatrix4fv(modelLocation,1,GL_FALSE,model_sphere.data());
    glBindVertexArray(vao_sphere);
    glDrawArrays(GL_TRIANGLES,0,sphere->getVertices().size());

    shaderProgram.release();
}

/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight) 
{
    // TODO: Update projection to fit the new aspect ratio
    projection.setToIdentity();
    projection.perspective(60,static_cast<float>(newWidth)/newHeight,0.4f,20);

}
// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";
    setAndTranslateModels();
    setScaleIntern(previousScale/100);
    setRotationIntern(rotateX,rotateY,rotateZ);
    previousRotation = {static_cast<float>(rotateX),static_cast<float>(rotateY),static_cast<float>(rotateZ)};
    this->update();
}
void MainView::setRotationIntern(int rotateX, int rotateY, int rotateZ)
{
    model_cube.rotate(rotateX,1,0,0);
    model_cube.rotate(rotateY,0,1,0);
    model_cube.rotate(rotateZ,0,0,1);
    model_pyramid.rotate(rotateX,1,0,0);
    model_pyramid.rotate(rotateZ,0,1,0);
    model_pyramid.rotate(rotateY,0,0,1);
    model_sphere.rotate(rotateX,1,0,0);
    model_sphere.rotate(rotateY,0,1,0);
    model_sphere.rotate(rotateZ,0,0,1);
}

void MainView::setScale(int scale)
{
    setAndTranslateModels();
    setRotationIntern(previousRotation.x(),previousRotation.y(),previousRotation.z());
    setScaleIntern(static_cast<float>(scale)/100);
    previousScale = scale;
    this->update();

}

void MainView::setScaleIntern(float scale)
{
    model_cube.scale(scale);
    model_pyramid.scale(scale);
    model_sphere.scale(scale);
}

void MainView::setAndTranslateModels()
{
    model_cube.setToIdentity();
    model_pyramid.setToIdentity();
    model_sphere.setToIdentity();
    model_cube.translate({2,0,-6});
    model_pyramid.translate({-2,0,-6});
    model_sphere.translate({0,0,-10});
    //Downscale the sphere so that we can observe it
    model_sphere.scale(0.04f);
}

void MainView::setShadingMode(ShadingMode shading)
{
    qDebug() << "Changed shading to" << shading;
    Q_UNIMPLEMENTED();
}

// --- Private helpers

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}
