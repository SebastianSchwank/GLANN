#include "lineobject.h"

LineObject::LineObject(){
}

LineObject::LineObject(float x0, float y0, float x1, float y1,
                       unsigned int color, float emission,
                       float phase, float frequency,
                       float reflectivity){
    FloatBits UintRgba;

    UintRgba.i_ = (unsigned int) (x0*4294967295.0);
    mData.append(UintRgba.i_);
    UintRgba.i_ = (unsigned int) (y0*4294967295.0);
    mData.append(UintRgba.i_);
    UintRgba.i_ = (unsigned int) (x1*4294967295.0);
    mData.append(UintRgba.i_);
    UintRgba.i_ = (unsigned int) (y1*4294967295.0);
    mData.append(UintRgba.i_);

    mData.append(color);

    UintRgba.i_ = (unsigned int) (emission*4294967295.0);
    mData.append(UintRgba.i_);

    UintRgba.i_ = (unsigned int) (phase*4294967295.0);
    mData.append(UintRgba.i_);
    UintRgba.i_ = (unsigned int) (frequency*4294967295.0);
    mData.append(UintRgba.i_);

    UintRgba.i_ = (unsigned int) (reflectivity*4294967295.0);
    mData.append(UintRgba.i_);
}

unsigned int LineObject::getAttributeAsPixel(int i){
    return mData[i];
}

int LineObject::getSize(){
    //Size of one line-object in "uints"
    return 9;
}
