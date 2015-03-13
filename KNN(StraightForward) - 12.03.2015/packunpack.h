#ifndef PACKUNPACK_H
#define PACKUNPACK_H

#include <QColor>


class PackUnpack
{
public:
    PackUnpack();

    static QColor pack(float val);
    static float unpack(QColor val);

};

#endif // PACKUNPACK_H
