#ifndef SCENE_H
#define SCENE_H

#pragma once

#include <QVector>
#include <QImage>

#include "lineobject.h"
#include "pointlight.h"


class Scene
{
public:
    Scene();
    void addObject(LineObject newObject);
    void addObject(PointLight newObject);
    QImage *getSceneImage();

private:
    QVector<PointLight> mSceneP;
    QVector<LineObject> mSceneL;
    QImage *SceneImage;
};

#endif // SCENE_H
