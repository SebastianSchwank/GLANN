#ifndef BITONICINDEXMASK_H
#define BITONICINDEXMASK_H

#include <QImage>
#include <QColor>

#include <qmath.h>

class BitonicIndexMask : public QImage
{
public:
    BitonicIndexMask(unsigned int width, unsigned int height);

private:
    void stage(unsigned int stageNbr, bool dir);
    void pair(unsigned int YCoord, unsigned int XCoord, int relCoord, bool dir);
    void mirror(unsigned int axis);

};

#endif // BITONICINDEXMASK_H
