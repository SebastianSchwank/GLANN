#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mSize = 256;
    mLayers = 2;

    knn = new GLANN(mSize,mLayers,this);

    ui->verticalLayout->addWidget(knn);
    renderEnablet = true;
    knn->setRenderEnablet(renderEnablet);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *)
{

    QVector<float> input;
    float inVal = 1.0f*qrand()/RAND_MAX;
    //for(int i = 0; i < size-1; i++)
    //    input.append((1.0+sin( frameStep/7.0 * 2 * 3.145 + 2.0 * 3.145 * 1.0f * 4.0*i/(size-1)))/4.0+0.25);
    for(int i = 0; i < mSize-1; i++)
        input.append((1.0+sin( mIterationCounter/10.0 * 2 * 3.145 + 2.0 * 3.145 * 1.0f * 4.0*i/(mSize-1)))/2.0);
    //for(int i = 0; i < size-1; i++) input.append(inVal);
    QVector<float> out = knn->propagateForward(input);

    //targe = inverted sine
    QVector<float> target;
    for(int i = 0; i < mSize-1; i++)
        target.append((1.0-sin( mIterationCounter/10.0 * 2 * 3.145 + 2.0 * 3.145 * 1.0f * 4.0*i/(mSize-1)))/2.0);

    //qDebug() << "Out: " << out;
    //qDebug() << "Target: " << target;

    //render();
    //Calc target(=input) - output ERROR SIGNAL
    float erroQuad = 0.0;
    for(int i = 0; i < mSize-1; i++) {
        input[i] = (target[i] - out[i]) * out[i] * (1.0f - out[i]);
        erroQuad += (target[i]-out[i])*(target[i]-out[i]);
    }

    //program.setUniformValue("learningRate",(float)0.123);

    //qDebug() << "Glob.Error: " << erroQuad;

    knn->errorBackProagation(input);
    //qDebug() << "Out: " << out;

    mIterationCounter += 1;
    mGlobError += erroQuad;

    if(mIterationCounter % 10 == 0 ){
        qDebug() << "Glob.QuadPeriodicMidError: " << mGlobError/10.0;
        mGlobError = 0.0;
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    if(timer.isActive()){
        // Use QBasicTimer because its faster than QTimer
        timer.stop();
    }else{
        timer.start(0,this);
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Wav-File"), "c:/", tr("Audio Files (*.wav)"));

}
