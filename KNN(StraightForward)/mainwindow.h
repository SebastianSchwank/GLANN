#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QGraphicsScene>

#include <QString>
#include <QString>

#include "glann.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_pushButton_importInputData_clicked();

    void on_pushButton_StartStop_clicked();

private:
    void timerEvent(QTimerEvent *);

    Ui::MainWindow *ui;

    QBasicTimer timer;

    QVector<QString> InputFiles;
    QVector<QString> TargetFiles;

    GLANN *knn;
    bool renderEnablet;
    unsigned int mSize,mLayers;

    QGraphicsScene *errorGraph;
    unsigned long int mFrameCounter;
    float lastError,accError;

    QGraphicsScene *outPlot;
    QGraphicsScene *inPlot;
    QGraphicsScene *targetPlot;

    QGraphicsScene *netView;

};

#endif // MAINWINDOW_H
