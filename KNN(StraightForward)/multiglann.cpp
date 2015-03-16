#include "multiglann.h"

MultiGLANN::MultiGLANN(int numSets, unsigned int layerSize, unsigned int numLayers, QImage *inputWeights)
{
    this->numThreads = numSets;

    for(int i = 0; i < numThreads; i++){
        GLANN *knn = new GLANN(layerSize,numLayers,inputWeights);
        workerThreads.append(knn);
    }
}


QVector< QVector<float> > MultiGLANN::propagateForward(QVector< QVector<float> > inputSet){

    for(int i = 0; i < numThreads; i++){
        QFuture< QVector<float> > futureThreadResult = QtConcurrent::run(workerThreads[i],&GLANN::propagateForward,inputSet[i]);
        futureThreadsResults.append(futureThreadResult);
    }

    bool allThreadsAreFinished = false;
    while(!allThreadsAreFinished){
        allThreadsAreFinished = true;
        for(int i = 0; i < numThreads; i++){
            if(futureThreadsResults[i].isRunning()) allThreadsAreFinished = false;
        }
    }

    QVector< QVector<float> > results;
    for(int i = 0; i < numThreads; i++){
        results.append(futureThreadsResults[i].result());
    }

    return results;
}

QVector<QImage> MultiGLANN::errorBackProagation(QVector< QVector<float> > errorSet){

}

void replaceWeights(QImage newWeights,unsigned int layerSize = -1, unsigned int numLayers = -1){

}
