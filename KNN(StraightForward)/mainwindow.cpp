#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mSize = 32;
    mLayers = 6;

    knn = new GLANN(mSize,mLayers);

    ui->verticalLayout->addWidget(knn);

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
    accError = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::doCalculations(){

    mFrameCounter++;
/*
    float r = 3.0 + (mFrameCounter%100)/100.0;

    float x0 = 0.5;
    float x = x0;

    //initalise input (logistic function) output should be r
    QVector<float> input;
    for(int i = 0; i < mSize-1; i++){
        input.append(x);
        x = r * x * (1-x);
    }

    QVector<float> target;
    for(int i = 0; i < mSize-1; i++)
        target.append(0.0);

    //Target output is the bifucation of the input ("Histogramm" of the input Function)
    for(int i = 0; i < mSize-1; i++){
        target[(int)(input[i]*(mSize-1))] += 1;
    }
    //Scale it  to 0..1(
    float maxTarget = 0.0;
    for(int i = 0; i < mSize-1; i++){
        if(maxTarget < target[i]) maxTarget = target[i];
    }
    for(int i = 0; i < mSize-1; i++){
        target[i] = target[i]/maxTarget;
    }


    //for(int i = 0; i < size-1; i++) input.append(inVal);

    QVector<float> out = knn->propagateForward(input);

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
*/
    /*------------------------------------------*/
    QVector<float> input;
    for(int i = 0; i < mSize-1; i++){
        input.append((1.0+sin((mFrameCounter % 25)/10.0*(i/(mSize-1.0f))*2*3.14159265359))/2.0);
    }

    QVector<float> out = knn->propagateForward(input);

    QVector<float> target;
    for(int i = 0; i < mSize-1; i++)
        target.append(0.5);

    target[(mFrameCounter % 25)+(mSize-1)/10] = 0.9999;

    float erroQuad = 0.0;
    QVector<float> error;
    for(int i = 0; i < mSize-1; i++) {
        error.append((target[i] - out[i]) * out[i] * (1.0f - out[i]));
        erroQuad += (target[i]-out[i])*(target[i]-out[i]);
    }

    knn->errorBackProagation(error);

    /*------------------------------------------*/

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
    if(mFrameCounter % 25 == 0 ){
        errorGraph->addLine(mFrameCounter/25-1,-lastError,mFrameCounter/25,-accError);
        lastError = accError;
        accError = 0;
    }

}

void MainWindow::timerEvent(QTimerEvent *)
{
    doCalculations();

    netView->clear();
    QPixmap tmpRendered = QPixmap::fromImage(knn->render());
    netView->addPixmap(tmpRendered);

}

void MainWindow::on_pushButton_importInputData_clicked()
{
    QStringList ls = QFileDialog::getOpenFileNames(this,
        tr("Open Wav-File"), "c:/", tr("Audio Files (*.wav)"));
    InputFiles.setStringList(ls);
    ui->listView_inputData->setModel(&InputFiles);
}

void MainWindow::on_pushButton_importTarget_clicked()
{
    QStringList ls = QFileDialog::getOpenFileNames(this,
        tr("Open Wav-File"), "c:/", tr("Audio Files (*.wav)"));
    TargetFiles.setStringList(ls);
    ui->listView_inputData->setModel(&TargetFiles);
}

void MainWindow::on_pushButton_StartStop_clicked()
{
    // ------------ !!!
    knn->setLearningrate(0.35);
    knn->setSteepness(1.1);
    knn->setMomentum(0.2);

    if(timer.isActive()){
        // Use QBasicTimer because its faster than QTimer
        timer.stop();
    }else{
        timer.start(0,this);
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    float scale = 1.5;
    ui->graphicsView_net->scale(scale,scale);
}

void MainWindow::on_pushButton_3_clicked()
{
    float scale = 0.7;
    ui->graphicsView_net->scale(scale,scale);
}

void MainWindow::on_actionExport_Network_triggered()
{
    if(knn != 0){

        QString fileName = QFileDialog::getSaveFileName(this,
                tr("Save Neuron Weights"), "",
                tr("Neuron Weights ImageFile (*.png);;All Files (*)"));

        QImage NeuronWeights = knn->getNetAsImage();

        QImageWriter writer(fileName);
        writer.setText("size", QString::number(mSize));
        writer.setText("layers", QString::number(mLayers));
        writer.write(NeuronWeights);
    }
}

void MainWindow::on_actionImport_Network_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Neuron Weights"), "",
        tr("Neuron Weights ImageFile (*.png)"));

    QImageReader reader(fileName);
    mSize = reader.text("size").toInt();
    mLayers = reader.text("layers").toInt();

    if(!fileName.isEmpty() && mSize > 0 && mLayers > 0){

        netWeights = reader.read();
        knn->replaceWeights(netWeights,mSize,mLayers);

        netView->clear();
        QPixmap tmpRendered = QPixmap::fromImage(knn->render());
        netView->addPixmap(tmpRendered);
    }
}

void MainWindow::on_pushButton_singleShot_clicked()
{
    QVector<float> input;
    for(int i = 0; i < mSize-1; i++){
        input.append(0.0);
    }
    input[50] = 1.0;
    input[120] = 1.0;

    QVector<float> out = knn->propagateForward(input);


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


}
