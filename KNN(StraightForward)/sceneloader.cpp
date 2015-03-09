#include "sceneloader.h"

SceneLoader::SceneLoader(QString filename)
{

    qsrand((uint)QTime::currentTime().msec());

    if(filename == "demoScene"){
        mScene = new Scene();

        for(int i = 0; i < 1; i++){
            LineObject l(1.0f*qrand()/RAND_MAX,1.0f*qrand()/RAND_MAX,
                         1.0f*qrand()/RAND_MAX,1.0f*qrand()/RAND_MAX,
                          qRgba(255.0f*qrand()/RAND_MAX,
                                255.0f*qrand()/RAND_MAX,
                                255.0f*qrand()/RAND_MAX,
                                255),
                         1.0,0.0,0.5,0.0);
            mScene->addObject(l);
         }

    }
}


Scene* SceneLoader::getScene(){
    if(mScene != NULL) return mScene;
    else qDebug("Error no Scene loaded !");
}
