#include "mainview.h"
#include "model.h"
#include "vertex.h"

#include <math.h>
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
    makeCurrent();
    debugLogger->stopLogging();
    delete debugLogger;

    qDebug() << "MainView destructor";
    destroyModelBuffers();
    doneCurrent();
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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0, 1.0, 0.0, 1.0);

    createShaderProgram();
    loadMesh();
    loadTexture(":/textures/cat_diff.png",texture);

    material = {0.5f,0.5f,1.f,5.f};
    lightPosition = {1.0f,100.0f,1.0f};
    materialColor = {0.66f,0.66f,0.66f};
    lightColor = {1.f,1.f,1.f};

    // Initialize transformations
    updateProjectionTransform();
    updateModelTransforms();
}

void MainView::createShaderProgram()
{
    // Create shader program
    shaderProgramNormal.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_normal.glsl");
    shaderProgramNormal.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_normal.glsl");
    shaderProgramNormal.link();

    shaderProgramPhong.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_phong.glsl");
    shaderProgramPhong.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_phong.glsl");
    shaderProgramPhong.link();

    shaderProgramGouraud.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_gouraud.glsl");
    shaderProgramGouraud.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_gouraud.glsl");
    shaderProgramGouraud.link();

    // Get the uniforms
    uniformModelViewTransformNormal = shaderProgramNormal.uniformLocation("modelViewTransform");
    uniformProjectionTransformNormal = shaderProgramNormal.uniformLocation("projectionTransform");
    uniformNormalTransformNormal = shaderProgramNormal.uniformLocation("normalTransform");

    uniformModelViewTransformPhong = shaderProgramPhong.uniformLocation("modelViewTransform");
    uniformProjectionTransformPhong = shaderProgramPhong.uniformLocation("projectionTransform");
    uniformNormalTransformPhong = shaderProgramPhong.uniformLocation("normalTransform");
    uniformMaterialPhong = shaderProgramPhong.uniformLocation("material");
    uniformLightPositionPhong = shaderProgramPhong.uniformLocation("lightCoordinates");
    uniformLightColorPhong = shaderProgramPhong.uniformLocation("lightColor");
    uniformMaterialColorPhong = shaderProgramPhong.uniformLocation("materialColor");
    uniformSampler2DPhong = shaderProgramPhong.uniformLocation("samplerUniform");

    uniformModelViewTransformGouraud = shaderProgramGouraud.uniformLocation("modelViewTransform");
    uniformProjectionTransformGouraud = shaderProgramGouraud.uniformLocation("projectionTransform");
    uniformNormalTransformGouraud = shaderProgramGouraud.uniformLocation("normalTransform");
    uniformMaterialGouraud = shaderProgramGouraud.uniformLocation("material");
    uniformLightPositionGouraud = shaderProgramGouraud.uniformLocation("lightCoordinates");
    uniformLightColorGouraud = shaderProgramGouraud.uniformLocation("lightColor");
    uniformMaterialColorGouraud = shaderProgramGouraud.uniformLocation("materialColor");
    uniformSampler2DGouraud = shaderProgramGouraud.uniformLocation("samplerUniform");

    activeShader = &shaderProgramPhong;
}
void MainView::loadTexture(QString file, GLuint texturePtr){
    QImage image(file);
    QVector<quint8> imageVec = imageToBytes(image);
    glGenTextures(1,&texturePtr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,image.width(),image.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,imageVec.data());
}

void MainView::loadMesh()
{
    Model model(":/models/cat.obj");
    QVector<QVector3D> vertexCoords = model.getVertices();
    QVector<QVector3D> vertexNormals = model.getNormals();
    QVector<QVector2D> vertexTexCoords = model.getTextureCoords();


    QVector<float> meshData;
    meshData.reserve(2 * 3 * vertexCoords.size() + 2 * vertexCoords.size());

    for (int i=0;i < vertexCoords.size();i++)
    {
        meshData.append(vertexCoords.at(i).x());
        meshData.append(vertexCoords.at(i).y());
        meshData.append(vertexCoords.at(i).z());
        meshData.append(vertexNormals.at(i).x());
        meshData.append(vertexNormals.at(i).y());
        meshData.append(vertexNormals.at(i).z());
        meshData.append(vertexTexCoords.at(i).x());
        meshData.append(vertexTexCoords.at(i).y());
    }

    meshSize = vertexCoords.size();

    // Generate VAO
    glGenVertexArrays(1, &meshVAO);
    glBindVertexArray(meshVAO);

    // Generate VBO
    glGenBuffers(1, &meshVBO);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

    // Write the data to the buffer
    glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(float), meshData.data(), GL_STATIC_DRAW);

    // Set vertex coordinates to location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // Set colour coordinates to location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Set colour coordinates to location 1
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    activeShader->bind();

    // Set the projection matrix
    glUniformMatrix4fv(uniformProjectionTransformNormal, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransformNormal, 1, GL_FALSE, meshTransform.data());
    glUniformMatrix3fv(uniformNormalTransformNormal,1,GL_FALSE,normalTransform.data());

    glUniformMatrix4fv(uniformProjectionTransformPhong, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransformPhong, 1, GL_FALSE, meshTransform.data());
    glUniformMatrix3fv(uniformNormalTransformPhong,1,GL_FALSE,normalTransform.data());
    glUniform3fv(uniformLightPositionPhong,1,lightPosition.data());
    glUniform3fv(uniformLightColorPhong,1,lightColor.data());
    glUniform3fv(uniformMaterialColorPhong,1,materialColor.data());
    glUniform4fv(uniformMaterialPhong,1,material.data());

    glUniformMatrix4fv(uniformProjectionTransformGouraud, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransformGouraud, 1, GL_FALSE, meshTransform.data());
    glUniformMatrix3fv(uniformNormalTransformGouraud,1,GL_FALSE,normalTransform.data());
    glUniform3fv(uniformLightPositionGouraud,1,lightPosition.data());
    glUniform3fv(uniformLightColorGouraud,1,lightColor.data());
    glUniform3fv(uniformMaterialColorGouraud,1,materialColor.data());
    glUniform4fv(uniformMaterialGouraud,1,material.data());

    glUniform1i(uniformSampler2DGouraud,0);
    glUniform1i(uniformSampler2DPhong,0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture);

    glBindVertexArray(meshVAO);
    glDrawArrays(GL_TRIANGLES, 0, meshSize);

    activeShader->release();
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
    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
    updateProjectionTransform();
}

void MainView::updateProjectionTransform()
{
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());
    projectionTransform.setToIdentity();
    projectionTransform.perspective(60, aspect_ratio, 0.2, 20);
}

void MainView::updateModelTransforms()
{
    meshTransform.setToIdentity();
    meshTransform.translate(0, -0.5, -1);
    meshTransform.scale(scale);
    meshTransform.rotate(QQuaternion::fromEulerAngles(rotation));


    updateNormalTransform();
    update();
}

void MainView::updateNormalTransform(){
    normalTransform = meshTransform.normalMatrix();
}

// --- OpenGL cleanup helpers

void MainView::destroyModelBuffers()
{
    glDeleteBuffers(1, &meshVBO);
    glDeleteVertexArrays(1, &meshVAO);
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    rotation = { static_cast<float>(rotateX), static_cast<float>(rotateY), static_cast<float>(rotateZ) };
    updateModelTransforms();
}

void MainView::setScale(int newScale)
{
    scale = static_cast<float>(newScale) / 100.f;
    updateModelTransforms();
}

void MainView::setShadingMode(ShadingMode shading)
{
    qDebug() << "Changed shading to" << shading;
    switch(shading){
        case PHONG:
            activeShader = &shaderProgramPhong;
            break;
        case NORMAL:
            activeShader = &shaderProgramNormal;
            break;
        case GOURAUD:
            activeShader = &shaderProgramGouraud;
            break;
        default: break;
    }
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

