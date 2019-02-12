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

    Vertex btr,bur,ftr,fur,btl,bul,ftl,ful,topOfPyramid;

    btr.x=1;
    btr.y=1;
    btr.z=1;
    btr.r=1;
    btr.g=0;
    btr.b=0;

    bur.x=1;
    bur.y=1;
    bur.z=-1;
    bur.r=0;
    bur.g=1;
    bur.b=0;

    ftr.x=1;
    ftr.y=-1;
    ftr.z=1;
    ftr.r=0;
    ftr.g=0;
    ftr.b=1;

    fur.x=1;
    fur.y=-1;
    fur.z=-1;
    fur.r=1;
    fur.g=0;
    fur.b=0;

    btl.x=-1;
    btl.y=1;
    btl.z=1;
    btl.r=0;
    btl.g=1;
    btl.b=0;

    bul.x=-1;
    bul.y=1;
    bul.z=-1;
    bul.r=0;
    bul.g=0;
    bul.b=1;

    ftl.x=-1;
    ftl.y=-1;
    ftl.z=1;
    ftl.r=1;
    ftl.g=0;
    ftl.b=0;

    ful.x=-1;
    ful.y=-1;
    ful.z=-1;
    ful.r=0;
    ful.g=1;
    ful.b=0;

    topOfPyramid.x=0;
    topOfPyramid.y=0;
    topOfPyramid.z=1;
    topOfPyramid.r=1;
    topOfPyramid.g=0;
    topOfPyramid.b=0;

    //cube defined by two trianles per face
    Vertex cube[] = {ful,fur,ftl,fur,ftr,ftl,fur,bur,ftl,bur,btr,ftr,bur,bul,btr,bul,btl,btr,ful,ftl,btl,bul,ful,btl,ftl,ftr,btl,ftr,btr,btl,ful,bul,fur,fur,bul,bur};
    Vertex pyramid[] = {ful,bul,fur,fur,bul,bur,ful,fur,topOfPyramid,fur,bur,topOfPyramid,bur,bul,topOfPyramid,bul,ful,topOfPyramid};


    //initialize cube vbo and vao
    glGenBuffers(1, &vbo_cube);
    glGenVertexArrays(1, &vao_cube);
    glBindVertexArray(vao_cube);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
    //we send the cube to the gpu
    glBufferData(GL_ARRAY_BUFFER,sizeof(Vertex)*36,cube, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)(3*sizeof(float)));

    //initialize pyramid vbo and vao
    glGenBuffers(1, &vbo_pyramid);
    glGenVertexArrays(1, &vao_pyramid);
    glBindVertexArray(vao_pyramid);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid);
    //we send the pyramid to the gpu
    glBufferData(GL_ARRAY_BUFFER,sizeof(Vertex)*18,pyramid, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)(3*sizeof(float)));

    model_cube = new QMatrix4x4();
    model_pyramid = new QMatrix4x4();
    QVector3D trans_cube = new QVector3D(2,0,-6);
    QVector3D trans_pyramid = new QVector3D(-2,0,-6);
    model_cube.translate(trans_cube);
    model_pyramid.translate(trans_pyramid);



}

void MainView::createShaderProgram()
{
    // Create shader program
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader.glsl");
    shaderProgram.link();
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

    // Draw here
    glBindVertexArray(vao_cube);
    glDrawArrays(GL_TRIANGLES,0,36);
    glBindVertexArray(vao_pyramid);
    glDrawArrays(GL_TRIANGLES,0,18);


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
    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";
    Q_UNIMPLEMENTED();
}

void MainView::setScale(int scale)
{
    qDebug() << "Scale changed to " << scale;
    Q_UNIMPLEMENTED();
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
