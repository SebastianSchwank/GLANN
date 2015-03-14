#ifndef GLANN_H
#define GLANN_H

#pragma once

#include <QApplication>
#include <QScreen>

#include <QGLShaderProgram>
#include <QGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QGLFramebufferObjectFormat>
#include <QGLPixelBuffer>

#include <QApplication>

#include <QBasicTimer>
#include <QTime>
#include <QImage>
#include <QVector>

#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QMouseEvent>

#include "sceneloader.h"
#include "playground.h"
#include "Floatbits.h"
#include "bitonicindexmask.h"
#include "errorCodes.h"

class GLANN : public QGLWidget,protected QGLFunctions
{
    Q_OBJECT

public:
    GLANN(unsigned int layerSize, unsigned int numLayers,
          QWidget *parent,
          QImage *NetworkWeights = 0, QGLWidget *shareWidget = 0);

    QVector<float> propagateForward(QVector<float> input);
    void errorBackProagation(QVector<float> error);

    QImage getNetAsImage();

    void setLearningrate(float value);
    void setSteepness(float value);
    void setRenderEnablet(bool enable);

protected:

    void timerEvent(QTimerEvent *);
    void initializeGL();
    void paintGL();

private:

    void render();

    static float fract (float f);

    static float unpack(QColor pixelColor);
    static QColor pack(float value);

    void copyImageSection(QPoint to, QImage* Dest, QImage Source);
    void copyImageOutput(QPoint to, QImage* Dest, QImage Source);

    void initShader();
    void initTextures();
    bool initFbo();

    QBasicTimer timer;

    QImage loadedNetworkWeights;
    bool loadedImageAvailable;

    QImage *NetworkWeights;
    QImage *NetworkActivation;

    GLuint pixelsNetworkWeights;
    GLuint pixelsNetworkActivation;

    unsigned int numLayers;
    unsigned int size;

    QGLShaderProgram program;
    QOpenGLFramebufferObject *fbo;

    GLuint vboId0;
    GLuint vboId1;
    struct VertexData
    {
        QVector3D position;
        QVector2D texCoord;
    };

};

#endif // GLANN_H
