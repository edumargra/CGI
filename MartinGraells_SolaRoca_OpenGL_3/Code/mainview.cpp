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
    debugLogger->stopLogging();

    qDebug() << "MainView destructor";

    destroyTextureBuffers();
    destroyModelBuffers();
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
    loadMeshes();

    // Initialize transformations
    updateProjectionTransform();
    updateModelTransforms();
}

void MainView::createShaderProgram()
{
    // Create Normal Shader program
    normalShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_normal.glsl");
    normalShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_normal.glsl");
    normalShaderProgram.link();

    // Create Gouraud Shader program
    gouraudShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_gouraud.glsl");
    gouraudShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_gouraud.glsl");
    gouraudShaderProgram.link();

    // Create Phong Shader program
    phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_phong.glsl");
    phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_phong.glsl");
    phongShaderProgram.link();

    // Get the uniforms for the normal shader.
    uniformModelViewTransformNormal  = normalShaderProgram.uniformLocation("modelViewTransform");
    uniformProjectionTransformNormal = normalShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformNormal     = normalShaderProgram.uniformLocation("normalTransform");

    // Get the uniforms for the gouraud shader.
    uniformModelViewTransformGouraud  = gouraudShaderProgram.uniformLocation("modelViewTransform");
    uniformProjectionTransformGouraud = gouraudShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformGouraud     = gouraudShaderProgram.uniformLocation("normalTransform");
    uniformMaterialGouraud            = gouraudShaderProgram.uniformLocation("material");
    uniformLightPositionGouraud       = gouraudShaderProgram.uniformLocation("lightPosition");
    uniformLightColourGouraud         = gouraudShaderProgram.uniformLocation("lightColour");
    uniformTextureSamplerGouraud      = gouraudShaderProgram.uniformLocation("textureSampler");

    // Get the uniforms for the phong shader.
    uniformModelViewTransformPhong  = phongShaderProgram.uniformLocation("modelViewTransform");
    uniformProjectionTransformPhong = phongShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformPhong     = phongShaderProgram.uniformLocation("normalTransform");
    uniformMaterialPhong            = phongShaderProgram.uniformLocation("material");
    uniformLightPositionPhong       = phongShaderProgram.uniformLocation("lightPosition");
    uniformLightColourPhong         = phongShaderProgram.uniformLocation("lightColour");
    uniformTextureSamplerPhong      = phongShaderProgram.uniformLocation("textureSampler");
}

void MainView::loadMeshes(){
    loadMesh(":/models/cat.obj",{1,1,-4},{0,0,45},{1,4,3},{0.01,0.02,0}); //texture, init pos, orientation,rotation, speed
    loadTexture(":/textures/rug_logo.png",meshes.at(0).texturePtr);
    loadMesh(":/models/sphere.obj",{3,1,-4},{0,0,0},{1,1,1},{0.01,0.02,0}); //texture, init pos, orientation, speed
    loadTexture(":/textures/rug_logo.png",meshes.at(1).texturePtr);

    loadMesh(":/models/cat.obj",{-1,0,-4},{0,180,-45},{0,-0.1,0},{-0.01,-0.03,0}); //texture, init pos, orientation, speed
    loadTexture(":/textures/rug_logo.png",meshes.at(2).texturePtr);
    loadMesh(":/models/sphere.obj",{-3,0,-4},{0,0,0},{0,0,0},{-0.01,-0.03,0}); //texture, init pos, orientation, speed
    loadTexture(":/textures/rug_logo.png",meshes.at(3).texturePtr);
}

void MainView::loadMesh(QString url, QVector3D pos, QVector3D orientation,QVector3D rotation, QVector3D speed)
{
    Model model(url);
    model.unitize();
    QVector<float> meshData = model.getVNTInterleaved();

    //this->meshSize = model.getVertices().size();
    Mesh mesh(pos,orientation,speed,rotation,model.getVertices().size());

    // Generate VAO
    //glGenVertexArrays(1, &meshVAO);
    //glBindVertexArray(meshVAO);

    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);


    // Generate VBO
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

    // Write the data to the buffer
    glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(float), meshData.data(), GL_STATIC_DRAW);

    // Set vertex coordinates to location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // Set vertex normals to location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Set vertex texture coordinates to location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    meshes.push_back(mesh);
}


void MainView::loadTexture(QString file, GLuint  &texturePtr)
{
    glGenTextures(1, &texturePtr);
    // Set texture parameters.
    glBindTexture(GL_TEXTURE_2D, texturePtr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Push image data to texture.
    QImage image(file);
    QVector<quint8> imageData = imageToBytes(image);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());

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

    // Choose the selected shader.
    QOpenGLShaderProgram *shaderProgram;
    switch (currentShader) {
    case NORMAL:
        shaderProgram = &normalShaderProgram;
        shaderProgram->bind();
        break;
    case GOURAUD:
        shaderProgram = &gouraudShaderProgram;
        shaderProgram->bind();
        break;
    case PHONG:
        shaderProgram = &phongShaderProgram;
        shaderProgram->bind();
        break;
    }

    // Set the texture and draw the mesh.
    updateEngine();
    for(uint i=0; i < meshes.size(); i++){
        switch (currentShader) {
        case NORMAL:
            updateNormalUniforms(i);
            break;
        case GOURAUD:
            updateGouraudUniforms(i);
            break;
        case PHONG:
            updatePhongUniforms(i);
            break;
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, meshes.at(i).texturePtr);

        glBindVertexArray(meshes.at(i).vao);
        glDrawArrays(GL_TRIANGLES, 0, meshes.at(i).numVertices);
    }


    shaderProgram->release();
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

void MainView::updateNormalUniforms(uint i)
{
    glUniformMatrix4fv(uniformProjectionTransformNormal, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransformNormal, 1, GL_FALSE, meshes.at(i).model.data());
    glUniformMatrix3fv(uniformNormalTransformNormal, 1, GL_FALSE, meshes.at(i).normalMatrix.data());
}

void MainView::updateGouraudUniforms(uint i)
{
    glUniformMatrix4fv(uniformProjectionTransformGouraud, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransformGouraud, 1, GL_FALSE, meshes.at(i).model.data());
    glUniformMatrix3fv(uniformNormalTransformGouraud, 1, GL_FALSE, meshes.at(i).normalMatrix.data());

    glUniform4fv(uniformMaterialGouraud, 1, &material[0]);
    glUniform3fv(uniformLightPositionGouraud, 1, &lightPosition[0]);
    glUniform3fv(uniformLightColourGouraud, 1, &lightColour[0]);

    glUniform1i(uniformTextureSamplerGouraud, 0); // Redundant now, but useful when you have multiple textures.
}

void MainView::updatePhongUniforms(uint i)
{
    glUniformMatrix4fv(uniformProjectionTransformPhong, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransformPhong, 1, GL_FALSE, meshes.at(i).model.data());
    glUniformMatrix3fv(uniformNormalTransformPhong, 1, GL_FALSE, meshes.at(i).normalMatrix.data());

    glUniform4fv(uniformMaterialPhong, 1, &material[0]);
    glUniform3fv(uniformLightPositionPhong, 1, &lightPosition[0]);
    glUniform3fv(uniformLightColourPhong, 1, &lightColour[0]);

    glUniform1i(uniformTextureSamplerPhong, 0);

}

void MainView::updateProjectionTransform()
{
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());
    projectionTransform.setToIdentity();
    projectionTransform.perspective(60, aspect_ratio, 0.2, 20);
}

void MainView::updateModelTransforms()
{
    for(uint i = 0; i < meshes.size(); i++){
        meshes.at(i).model.setToIdentity();
        meshes.at(i).model.translate(meshes.at(i).location.x(), meshes.at(i).location.y(), meshes.at(i).location.z());
        meshes.at(i).model.scale(scale);
        meshes.at(i).model.rotate(QQuaternion::fromEulerAngles(meshes.at(i).orientation + generalRotation));
        meshes.at(i).normalMatrix = meshes.at(i).model.normalMatrix();
    }

    update();
}

// --- OpenGL cleanup helpers

void MainView::destroyModelBuffers()
{
    for(uint i=0; i < meshes.size(); i++){
        glDeleteBuffers(1, &meshes.at(i).vbo);
        glDeleteVertexArrays(1, &meshes.at(i).vao);
    }

}

void MainView::destroyTextureBuffers()
{
    for(uint i = 0; i < meshes.size(); i++){
        glDeleteTextures(1, &meshes.at(i).texturePtr);
    }
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    for(uint i= 0; i < meshes.size(); i++){
        generalRotation = { static_cast<float>(rotateX), static_cast<float>(rotateY), static_cast<float>(rotateZ) };
    }
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
    currentShader = shading;
}

void MainView::updateEngine(){
    for(uint i=0; i< meshes.size(); i++){
        if(meshes.at(i).location.x() > 3 || meshes.at(i).location.x() < -3){
           meshes.at(i).speed = {-meshes.at(i).speed.x(),meshes.at(i).speed.y(),meshes.at(i).speed.z()};
           meshes.at(i).orientation = {meshes.at(i).orientation.x(), meshes.at(i).orientation.y()-180,meshes.at(i).orientation.z()};
        }
        if(meshes.at(i).location.y() > 2 || meshes.at(i).location.y()< -1.5){
            meshes.at(i).speed = {meshes.at(i).speed.x(),-meshes.at(i).speed.y(),meshes.at(i).speed.z()};
            meshes.at(i).orientation = {meshes.at(i).orientation.x(), meshes.at(i).orientation.y(),-meshes.at(i).orientation.z()};

        }
        meshes.at(i).location += meshes.at(i).speed;
        meshes.at(i).orientation += meshes.at(i).rotation;
    }
    updateModelTransforms();
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

