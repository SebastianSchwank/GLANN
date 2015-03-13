#include "bitonicindexmask.h"

BitonicIndexMask::BitonicIndexMask(unsigned int width,unsigned int height)
    : QImage(width,height,QImage::Format_ARGB32)
{
    this->fill(Qt::black);
    stage(1,true);//Sort from smalles to the largest
}

void BitonicIndexMask::stage(unsigned int stageNbr, bool dir){

    if(stageNbr != 0){

        for(int i = 0; i < this->width(); i+= pow(2,stageNbr)){

            pair
            stage

            dir = !dir;
        }

    }

}

void BitonicIndexMask::pair(unsigned int YCoord, unsigned int XCoord, int relCoord, bool dir){

    unsigned int RelIndexPixelTHIStmp = 8388608 + 8388608 * float(relCoord)/float(this->width());
    unsigned int RelIndexPixelTHIS;

    if(dir) RelIndexPixelTHIS = (RelIndexPixelTHIStmp << 8) || 0;
    else RelIndexPixelTHIS = (RelIndexPixelTHIStmp << 8) || 255;

    this->setPixel(XCoord,YCoord,RelIndexPixelTHIS);

    unsigned int RelIndexPixelTHATtmp = 8388608 + 8388608 * float(-relCoord)/float(this->width());
    unsigned int RelIndexPixelTHAT;

    if(dir) RelIndexPixelTHAT = (RelIndexPixelTHATtmp << 8) || 255;
    else RelIndexPixelTHAT = (RelIndexPixelTHATtmp << 8) || 0;

    this->setPixel(XCoord+relCoord,YCoord,RelIndexPixelTHAT);

}

void BitonicIndexMask::mirror(unsigned int axis){

}
