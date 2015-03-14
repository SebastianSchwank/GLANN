#ifndef WAVELOADER_H
#define WAVELOADER_H

#include <QString>
#include <QAudioDecoder>

class WaveLoader
{
public:
    WaveLoader(QString path);

    unsigned int getSize();
    float getSample(unsigned int i);

private:
    QString Path;
    QAudioDecoder decoder;
};

#endif // WAVELOADER_H
