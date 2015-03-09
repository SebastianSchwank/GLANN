#ifndef SCENELOADER_H
#define SCENELOADER_H

#pragma once

#include "scene.h"
#include "Floatbits.h"

#include <QString>
#include <QTime>

class SceneLoader
{
public:
    SceneLoader(QString filename);

    Scene *getScene();

private:
    Scene* mScene = NULL;

};

#endif // SCENELOADER_H
