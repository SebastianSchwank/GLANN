#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mSize = 256;
    mLayers = 2;
    renderEnablet = true;

    knn = new GLANN(mSize,mLayers,this);

    ui->verticalLayout->addWidget(knn);
    renderEnablet = true;
    knn->setRenderEnablet(renderEnablet);

    errorGraph = new QGraphicsScene(this);
    ui->graphicsView_ErrorGraph->setScene(errorGraph);

    inPlot = new QGraphicsScene(this);
    ui->graphicsView_input->setScene(inPlot);

    outPlot = new QGraphicsScene(this);
    ui->graphicsView_output->setScene(outPlot);

    targetPlot = new QGraphicsScene(this);
    ui->graphicsView_target->setScene(targetPlot);

    netView = new QGraphicsScene(this);
    ui->graphicsView_net->setScene(netView);

    mFrameCounter = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *)
{
    mFrameCounter++;

    QVector<float> input;
    float inVal = 1.0f*qrand()/RAND_MAX;
    //for(int i = 0; i < size-1; i++)
    //    input.append((1.0+sin( frameStep/7.0 * 2 * 3.145 + 2.0 * 3.145 * 1.0f * 4.0*i/(size-1)))/4.0+0.25);
    for(int i = 0; i < mSize-1; i++)
        input.append(0.0);
    input[(mFrameCounter%160)+mSize/5] = 0.9999;
    //for(int i = 0; i < size-1; i++) input.append(inVal);
    QVector<float> out = knn->propagateForward(input);

    //targe = inverted sine
    QVector<float> target;
    for(int i = 0; i < mSize-1; i++)
        target.append((1.0+sin( 2.0 * 3.145 * (1.0f * (mFrameCounter % 160))/10 * i/(mSize-1)))/2.0);

    //qDebug() << "Out: " << out;
    //qDebug() << "Target: " << target;

    //render();
    //Calc target(=input) - output ERROR SIGNAL
    float erroQuad = 0.0;
    QVector<float> error;
    for(int i = 0; i < mSize-1; i++) {
        error.append((target[i] - out[i]) * out[i] * (1.0f - out[i]));
        erroQuad += (target[i]-out[i])*(target[i]-out[i]);
    }

    //program.setUniformValue("learningRate",(float)0.123);

    //qDebug() << "Glob.Error: " << erroQuad;

    knn->errorBackProagation(error);
    //qDebug() << "Out: " << out;

    float scaler = 50;
    //Plot input output & target
    inPlot->clear();
    for(int i = 1; i < mSize-1; i++){
        inPlot->addLine(i-1,-input[i-1]*scaler,i,-input[i]*scaler);
    }
    outPlot->clear();
    for(int i = 1; i < mSize-1; i++){
        outPlot->addLine(i-1,-out[i-1]*scaler,i,-out[i]*scaler);
    }
    targetPlot->clear();
    for(int i = 1; i < mSize-1; i++){
        targetPlot->addLine(i-1,-target[i-1]*scaler,i,-target[i]*scaler);
    }

    accError += erroQuad;

    //Plot the current Glob Error
    if(mFrameCounter % 160 == 0 ){
        errorGraph->addLine(mFrameCounter/160-1,-lastError,mFrameCounter/160,-accError);
        lastError = accError;
        accError = 0;
    }

    netView->addPixmap(QPixmap::fromImage(knn->render()));

}

void MainWindow::on_pushButton_importInputData_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Wav-File"), "c:/", tr("Audio Files (*.wav)"));
}

void MainWindow::on_pushButton_StartStop_clicked()
{
    // ------------ !!!
    knn->setLearningrate(0.1);

    if(timer.isActive()){
        // Use QBasicTimer because its faster than QTimer
        timer.stop();
    }else{
        timer.start(0,this);
    }
}
