#include "playground.h"

Playground::Playground(unsigned int width,unsigned int height)
    : QImage(width,height,QImage::Format_ARGB32)
{
    for(int x = 0; x < this->width(); x++){
        for(int y = 0; y < this->height(); y++){
            float randVal = (2.0f*qrand())/RAND_MAX - 1.0;
            float cubicAproximated = ((randVal*randVal*randVal*randVal*randVal)/32.0+0.5);
            QColor *color = new QColor((int)(cubicAproximated*255.0),
                                       (int)(cubicAproximated*255.0),
                                       (int)(cubicAproximated*255.0),
                                       (int)(cubicAproximated*255.0));

            //QColor *color = new QColor(126,126,126,126);
            //unsigned int col = (unsigned int) (x/float(width)*4294967295.0f/RAND_MAX);
            this->setPixel(x,y,color->rgba());
        }
    }
}
