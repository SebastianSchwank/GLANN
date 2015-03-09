#include "glann.h"


GLANN::GLANN(unsigned int size, unsigned int numLayers,
             QWidget *parent, QGLWidget *shareWidget)
      : QGLWidget(parent, shareWidget)
{
    QScreen *screen = QApplication::screens().at(0);

    //qDebug() << width << height << "------------------ WIDTH , HEIGHT";

    setFixedWidth(size*2+1);
    setFixedHeight(size*numLayers+numLayers+1);
    this->size = size;
    this->numLayers = numLayers;

    qsrand((uint)QTime::currentTime().msec());
}

void GLANN::initializeGL(){
    //setAutoBufferSwap(true);

    makeCurrent();
    initializeGLFunctions();

    //glEnable(GL_CULL_FACE);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    initShader();
    initTextures();

    // Generate 2 VBOs
    glGenBuffers(1, &vboId0);
    glGenBuffers(1, &vboId1);

    VertexData vertices[] = {
        // Vertex data for face 0
                {QVector3D(-1.0, -1.0,  0.0), QVector2D(0.0, 0.0)},  // v0
                {QVector3D( 1.0, -1.0,  0.0), QVector2D(1.0, 0.0)},  // v1
                {QVector3D(-1.0,  1.0,  0.0), QVector2D(0.0, 1.0)},  // v2
                {QVector3D( 1.0,  1.0,  0.0), QVector2D(1.0, 1.0)},  // v3
    };
    // Transfer vertex data to VBO 0
    glBindBuffer(GL_ARRAY_BUFFER, vboId0);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VertexData), vertices, GL_STATIC_DRAW);

     GLushort indices[] = {
                  0,  1,  2,  3,  3,
    };
    // Transfer index data to VBO 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboId1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 5 * sizeof(GLushort), indices, GL_STATIC_DRAW);

    //Init the Framebuffer
    initFbo();

    // Use QBasicTimer because its faster than QTimer
    timer.start(0, this);
}

void GLANN::timerEvent(QTimerEvent *)
{
    // Update scene
    update();
}

bool GLANN::initFbo(){
    QOpenGLFramebufferObjectFormat fmt;
    fmt.setSamples(1); // or 4 or disable this line
    fmt.setInternalTextureFormat(GL_RGBA8);
    fbo = new QOpenGLFramebufferObject(size, size+1, fmt);
    return true;
}

void GLANN::paintGL(){

    QVector<float> input;
    float inVal = 1.0f*qrand()/RAND_MAX;
    for(int i = 0; i < size-1; i++) input.append((1.0+sin( frameStepF + 2.0 * 3.145 * 1.0f * i/(size-1)))/2.0);
    //for(int i = 0; i < size-1; i++) input.append(inVal);
    QVector<float> out = propagateForward(input);

    QVector<float> target = input;

    qDebug() << "Out: " << out;
    qDebug() << "Target: " << target;

    //render();
    //Calc target(=input) - output ERROR SIGNAL
    for(int i = 0; i < size-1; i++) input[i] = (target[i] - out[i]) * out[i] * (1.0f - out[i]);

    qDebug() << "Error: " << input;

    errorBackProagation(input);

    render();

    frameStepF += 0.0;

}

void GLANN::errorBackProagation(QVector<float> error){
    //Check InpV.-Size
    if(error.size() != size-1) exit(inputVector_sizeError);

    //Packing Values into Textures

    for(int i = 0; i < size-1; i++){
        if(error[i] > 1.0 || error[i] < -1.0) exit(inputVector_valueUnscaled);
        NetworkWeights->setPixel(i,size*numLayers+numLayers,GLANN::pack(((error[i]*0.5f)+0.5f)).rgba());
        //currNetwActivation.setPixel(i,size,(unsigned int) ((0.5*(input[i]+1.0)) * 4294967295.0 )); //((0.5*(input[i]+1.0))* )
    }
    //SetBias Error in Output to zero
    NetworkWeights->setPixel(size-1,size*numLayers+numLayers, GLANN::pack(0.5).rgba());

    glViewport(0,0,size,size+1);

    // Offset for position
    int offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program.attributeLocation("a_position");
    program.enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program.attributeLocation("a_texcoord");
    program.enableAttributeArray(texcoordLocation);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    for(int layer = numLayers-1; layer >= 0; layer--){

        //set curr Layer
        program.setUniformValue("currLayer", layer);

        //Bind Textures
        QImage layerNetwActivation = NetworkActivation->copy(0,layer*(size+1),size,size+1);
        pixelsNetworkActivation = QGLWidget::bindTexture(layerNetwActivation);

        QImage layerNetwWeights = NetworkWeights->copy(0,layer*(size+1)+1,size,size+1);
        pixelsNetworkWeights = QGLWidget::bindTexture(layerNetwWeights);


        //Set Programm to correcting the wheights (have to add the momentum Term later)
        program.setUniformValue("shaderMode",3);

        fbo->bind();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            //Bind current Weights
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pixelsNetworkWeights);

            //Bind current Activation
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, pixelsNetworkActivation);

            // Draw cube geometry using indices from VBO 1
            glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_SHORT, 0);

        fbo->release();

        //To copy the sum activation into the Network Activation "Buffer"
        QImage layerCorrNetwWeights = fbo->toImage();

/*
        //Set program to fbo render mode to
        //Propagate error backwards through the Layer
        program.setUniformValue("shaderMode",2);

        fbo->bind();

            // Tell OpenGL which VBOs to use
            glBindBuffer(GL_ARRAY_BUFFER, vboId0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboId1);

            //Bind current Weights
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pixelsNetworkWeights);

            //Bind current Activation
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, pixelsNetworkActivation);

            // Draw cube geometry using indices from VBO 1
            glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_SHORT, 0);

        fbo->release();

        //To copy the activation into the Network Activation "Buffer"
        QImage layerNetwErrorProp = fbo->toImage();

        //Delete Network's Activation Buffer
        glDeleteTextures(1,&pixelsNetworkActivation);

        //Delete Network's Activation Buffer
        glDeleteTextures(1,&pixelsNetworkWeights);

*/
        //Now copy corrected Weights
        QPoint destPos1 = QPoint(0, layer*(size+1)+1);
        copyImageSection(destPos1,NetworkWeights,layerCorrNetwWeights);
/*

        //After that merge Image to Activation Buffer
        QPoint destPos2 = QPoint(0, layer*(size+1));
        copyImageOutput(destPos2,NetworkWeights,layerNetwErrorProp);
*/

    }


}

QVector<float> GLANN::propagateForward(QVector<float> input){

    //Check InpV.-Size
    if(input.size() != size-1) exit(inputVector_sizeError);

    QImage layerNetwActivation(size,size+1,QImage::Format_ARGB32);
    layerNetwActivation.fill(qRgba(0,0,0,0));

    //Packing Values into Textures

    for(int i = 0; i < size-1; i++){
        if(input[i] > 1.0 || input[i] < 0.0) exit(inputVector_valueUnscaled);
        layerNetwActivation.setPixel(i,0, GLANN::pack(input[i]).rgba()); //((0.5*(input[i]+1.0))* )
        //currNetwActivation.setPixel(i,size,(unsigned int) ((0.5*(input[i]+1.0)) * 4294967295.0 )); //((0.5*(input[i]+1.0))* )
    }

    glViewport(0,0,size,size+1);

    for(int layer = 0; layer < numLayers; layer++){

        //Set program to fbo render mode to
        //Propagate Forward through the Layers
        program.setUniformValue("shaderMode",0);

        //set curr Layer
        program.setUniformValue("currLayer", layer);

        fbo->bind();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            QImage layerNetwWeights = NetworkWeights->copy(0,layer*(size+1)+1,size,size+1);
            pixelsNetworkWeights = QGLWidget::bindTexture(layerNetwWeights);

            //Bind Textures and set Bias to 1.0
            layerNetwActivation.setPixel(size-1,0,(unsigned int)4294967295);
            copyImageOutput(QPoint(0,layer*(size+1)),NetworkActivation,layerNetwActivation);
            pixelsNetworkActivation = QGLWidget::bindTexture(layerNetwActivation);

            // Tell OpenGL which VBOs to use
            glBindBuffer(GL_ARRAY_BUFFER, vboId0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboId1);

            // Offset for position
            int offset = 0;

            // Tell OpenGL programmable pipeline how to locate vertex position data
            int vertexLocation = program.attributeLocation("a_position");
            program.enableAttributeArray(vertexLocation);
            glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

            // Offset for texture coordinate
            offset += sizeof(QVector3D);

            // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
            int texcoordLocation = program.attributeLocation("a_texcoord");
            program.enableAttributeArray(texcoordLocation);
            glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

            //Bind current Weights
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pixelsNetworkWeights);

            //Bind current Activation
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, pixelsNetworkActivation);

            // Draw cube geometry using indices from VBO 1
            glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_SHORT, 0);

        fbo->release();

        //To copy the activation into the Network Activation "Buffer"
        layerNetwActivation = fbo->toImage();

        //Copy Image to Activation Buffer (currently forced to CPU)
        QPoint destPos = QPoint(0, layer*(size+1)+1);
        copyImageSection(destPos,NetworkActivation,layerNetwActivation);

        //Delete Network's Activation Buffer
        glDeleteTextures(1,&pixelsNetworkActivation);

        //Set Programm to Forward Summing the products
        program.setUniformValue("shaderMode",1);

        //Bind Textures (Cant access original Teture so I copy it from the "collection" (Seems to be a bug)
        layerNetwActivation = NetworkActivation->copy(0,layer*(size+1),size,(size+1));
        pixelsNetworkActivation = QGLWidget::bindTexture(layerNetwActivation);

        //glViewport(0,0,size,size+1);

        fbo->bind();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            //Bind current Weights
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pixelsNetworkWeights);

            //Bind current Activation
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, pixelsNetworkActivation);

            // Draw cube geometry using indices from VBO 1
            glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_SHORT, 0);

        fbo->release();

        //To copy the sum activation into the Network Activation "Buffer"
        layerNetwActivation = fbo->toImage();

        destPos = QPoint(0, (layer+1)*(size+1));
        copyImageOutput(destPos,NetworkActivation,layerNetwActivation);

        //Delete Network's Activation Buffer
        glDeleteTextures(1,&pixelsNetworkActivation);

        //Delete Network's Activation Buffer
        glDeleteTextures(1,&pixelsNetworkWeights);

    }

    QVector<float> output;
    for(int i = 0; i < size; i++){
        QColor pixelColor = QColor(NetworkActivation->pixel(i,size*numLayers+numLayers));//size*numLayers+numLayers));
        float scaled = GLANN::unpack(pixelColor);
        output.append(scaled);
    }

    return output;
}

float GLANN::unpack(QColor pixelColor){
    float r = pixelColor.redF();
    float g = pixelColor.greenF();
    float b = pixelColor.blueF();
    float a = pixelColor.alphaF();
    float scaled = (float)(    (255.0/256.0)*r );
                             //+ (255.0/256.0)*g/(256.0) );
                             //+ (255.0/256.0)*b/(256.0*256.0)
                             //+ (255.0/256.0)*a/(256.0*256.0*256.0));
    return scaled;
}

QColor GLANN::pack(float value){

    QColor pixelColor = QColor(0,0,0,0);
    //pixelColor.setAlphaF(value*(255.0/256.0));
    //pixelColor.setBlueF(value*(255.0/256.0));
    //pixelColor.setGreenF(value*(255.0/256.0)*(1/256.0));
    pixelColor.setRedF(value*(255.0/256.0));
    return pixelColor;
}


void GLANN::render(){

     //Set Viewport back to default
     glViewport(0,0,size*2,size*numLayers+numLayers+1);

     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

     //Bind Textures
     QImage tmpNetworkActivation = NetworkActivation->copy();
     pixelsNetworkActivation = QGLWidget::bindTexture(tmpNetworkActivation);

     //Bind Textures
     QImage tmpNetworkWeights = NetworkWeights->copy();
     pixelsNetworkWeights = QGLWidget::bindTexture(tmpNetworkWeights);


     // Tell OpenGL which VBOs to use
     glBindBuffer(GL_ARRAY_BUFFER, vboId0);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboId1);

     // Offset for position
     int offset = 0;

     // Tell OpenGL programmable pipeline how to locate vertex position data
     int vertexLocation = program.attributeLocation("a_position");
     program.enableAttributeArray(vertexLocation);
     glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

     // Offset for texture coordinate
     offset += sizeof(QVector3D);

     // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
     int texcoordLocation = program.attributeLocation("a_texcoord");
     program.enableAttributeArray(texcoordLocation);
     glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

     //Set Program to screen rendering
     program.setUniformValue("shaderMode",-1);

     //Render To Screen
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, pixelsNetworkWeights);

     glActiveTexture(GL_TEXTURE1);
     glBindTexture(GL_TEXTURE_2D, pixelsNetworkActivation);

     // Draw quad geometry using indices from VBO 1
     glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_SHORT, 0);

     //Delete Network's Weights Buffer
     glDeleteTextures(1,&pixelsNetworkWeights);

     //Delete Network's Activation Buffer
     glDeleteTextures(1,&pixelsNetworkActivation);

}

void GLANN::copyImageSection(QPoint to, QImage* Dest, QImage Source){

    for(int x = 0; x < Source.width(); x++){
        for(int y = 0; y < Source.height()-1; y++){
            Dest->setPixel(x+to.x(),y+to.y(),Source.pixel(x,y));
        }
    }

}

void GLANN::copyImageOutput(QPoint to, QImage* Dest, QImage Source){

    for(int x = 0; x < Source.width(); x++){
        Dest->setPixel(to.x()+x,to.y(),Source.pixel(x,0));
    }

}

void GLANN::initTextures(){

    //Initalize Random Weights
    NetworkWeights = new Playground(size,size*numLayers+numLayers+1);
    //Initalize NetworkActivation + NeuronActivationSum + InputLayer (+1)
    NetworkActivation = new QImage(size,size*numLayers+numLayers+1,QImage::Format_ARGB32);
    //Set Activation to Zero
    NetworkActivation->fill(qRgba(126,126,126,126));

    //Set NouronsActivation to Zero
    for(int i = 0; i <= numLayers; i++){
        for(int x = 0; x < size; x++){
            NetworkWeights->setPixel(x,i*(size+1),qRgba(0,0,0,0));
        }
    }
    //Set NouronsActivation to Zero
    for(int i = 0; i <= numLayers; i++){
        for(int x = 0; x < size; x++){
            NetworkActivation->setPixel(x,i*(size+1),qRgba(0,0,0,0));
        }
    }

    // Poor filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //Texture Clamping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

}

void GLANN::initShader(){

    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();

    // Use texture unit 0
    program.setUniformValue("WeightsLayer",0);

    // Use texture unit 1
    program.setUniformValue("ActivationLayer",1);

    //width
    program.setUniformValue("size", size);

    //height
    program.setUniformValue("numLayers", numLayers);

}
