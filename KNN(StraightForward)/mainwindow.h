#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include <QGraphicsScene>
#include <QImageReader>
#include <QImageWriter>

#include <QString>
#include <QStringListModel>

#include "glann.h"
#include "waveloader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void doCalculations();

private slots:

    void on_pushButton_importInputData_clicked();

    void on_pushButton_StartStop_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_importTarget_clicked();

    void on_actionExport_Network_triggered();

    void on_actionImport_Network_triggered();

private:
    void timerEvent(QTimerEvent *);

    Ui::MainWindow *ui;

    QBasicTimer timer;

    QStringListModel InputFiles;
    QStringListModel TargetFiles;

    GLANN *knn;
    unsigned int mSize,mLayers;

    QGraphicsScene *errorGraph;
    unsigned long int mFrameCounter;
    float lastError,accError;

    QImage *currRenderedImage;
    QImage netWeights;

    QGraphicsScene *outPlot;
    QGraphicsScene *inPlot;
    QGraphicsScene *targetPlot;

    QGraphicsScene *netView;

};

#endif // MAINWINDOW_H
