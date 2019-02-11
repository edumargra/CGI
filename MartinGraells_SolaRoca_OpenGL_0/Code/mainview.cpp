#include "mainview.h"
#include "math.h"
#include "Vertex.h"

#include <QDateTime>

MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

MainView::~MainView() {
    debugLogger->stopLogging();

    qDebug() << "MainView destructor";
}

void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    debugLogger = new QOpenGLDebugLogger();
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ), this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if ( debugLogger->initialize() ) {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger->enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    glShader.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/vertshader.glsl");
    glShader.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/fragshader.glsl");
    glShader.link();
    glShader.bind();


    struct Vertex v1,v2,v3;
    v1.x = -0.5;
    v1.y = -0.5;
    v1.r = 1;
    v1.g = 0;
    v1.b = 0;

    v2.x = 0;
    v2.y = 0;
    v2.r = 0;
    v2.g = 1;
    v2.b = 0;

    v3.x = 0.5;
    v3.y = -0.5;
    v3.r = 0;
    v3.g = 0;
    v3.b = 1;
    struct Vertex triangle[3] = {v1,v2,v3};


    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER,sizeof(Vertex)*3,triangle, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)(2*sizeof(float)));
}

void MainView::resizeGL(int newWidth, int newHeight) {

    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
}

void MainView::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShader.bind();
    glDrawArrays(GL_TRIANGLES,0,3);
}

void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}
