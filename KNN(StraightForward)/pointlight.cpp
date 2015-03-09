#include "pointlight.h"

PointLight::PointLight(){

}

PointLight::PointLight(float x, float y,
                       float emission, unsigned int color){
    FloatBits UintRgba;

    UintRgba.i_ = (unsigned int) (x*4294967295.0);
    mData.append(UintRgba.i_);
    UintRgba.i_ = (unsigned int) (y*4294967295.0);
    mData.append(UintRgba.i_);

    UintRgba.i_ = (unsigned int) (emission*4294967295.0);
    mData.append(UintRgba.i_);

    mData.append(color);
}

unsigned int PointLight::getAttributeAsPixel(int i){
    return mData[i];
}

int PointLight::getSize(){
    //Size of one line-object in "uints"
    return 4;
}
