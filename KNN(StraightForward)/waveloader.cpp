#include "waveloader.h"

WaveLoader::WaveLoader(QString path){
    decoder.setSourceFilename(path);
    qDebug() << decoder.audioFormat();
}

unsigned int WaveLoader::getSize(){

}

float WaveLoader::getSample(unsigned int i){

}
