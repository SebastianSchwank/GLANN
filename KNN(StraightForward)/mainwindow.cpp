#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mSize = 128;
    mLayers = 3;

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

void MainWindow::timerEvent(QTimerEvent *)
{


        mFrameCounter++;

        QVector<float> input;
        float inVal1 = 1.0f*qrand()/RAND_MAX;
        float inVal2 = 1.0f*qrand()/RAND_MAX;
        //for(int i = 0; i < size-1; i++)
        //    input.append((1.0+sin( frameStep/7.0 * 2 * 3.145 + 2.0 * 3.145 * 1.0f * 4.0*i/(size-1)))/4.0+0.25);
        for(int i = 0; i < mSize-1; i++)
            input.append(0.0);

        input[(mFrameCounter%100)+mSize/8] = 0.9999;

        QVector<float> target;
        for(int i = 0; i < mSize-1; i++)
            target.append((1.0+sin( 2.0 * 3.145 * 1.0f * (mFrameCounter%100)/10 *i/(mSize-1)))/4.0+0.25);

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
        if(mFrameCounter % 100 == 0 ){
            errorGraph->addLine(mFrameCounter/10-1,-lastError,mFrameCounter/10,-accError);
            lastError = accError;
            accError = 0;
        }

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
    knn->setLearningrate(0.1);
    knn->setSteepness(0.01);

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
