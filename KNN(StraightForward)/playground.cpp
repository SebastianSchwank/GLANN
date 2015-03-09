#include "playground.h"

Playground::Playground(unsigned int width,unsigned int height)
    : QImage(width,height,QImage::Format_ARGB32)
{
    for(int x = 0; x < this->width(); x++){
        for(int y = 0; y < this->height(); y++){
            //QColor *color = new QColor(qrand()%255,qrand()%255,qrand()%255,qrand()%255);
            QColor *color = new QColor(126,126,126,126);
            //unsigned int col = (unsigned int) (x/float(width)*4294967295.0f/RAND_MAX);
            this->setPixel(x,y,color->rgba());
        }
    }
}
