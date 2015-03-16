#ifndef MULTIGLANN_H
#define MULTIGLANN_H

#include <QImage>
#include <QVector>

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

#include "glann.h"

class MultiGLANN
{
public:
    MultiGLANN(int numSets, unsigned int layerSize, unsigned int numLayers, QImage *inputWeights = 0);

    QVector< QVector<float> > propagateForward(QVector< QVector<float> > inputSet);
    QVector<QImage> errorBackProagation(QVector< QVector<float> > errorSet);

    void replaceWeights(QImage newWeights,unsigned int layerSize = -1, unsigned int numLayers = -1);

private:
    int numThreads;
    QVector<GLANN*> workerThreads;
    QVector< QFuture< QVector<float> > > futureThreadsResults;
};

#endif // MULTIGLANN_H
