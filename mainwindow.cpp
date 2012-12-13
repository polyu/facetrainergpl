#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    rec=false;
    desktop = QApplication::desktop();
    QGraphicsScene * scene = new QGraphicsScene(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    this->ui->graphicsView->setScene(scene);
        int screenWidth, width;
        int screenHeight, height;
        int x, y;
        QSize windowSize;

        screenWidth = desktop->width();
        screenHeight = desktop->height();

        windowSize = size();
        width = windowSize.width();
        height = windowSize.height();


        x = (screenWidth - width) / 2;
        y = (screenHeight - height) / 2;
        y -= 50;


        move ( x, y );
        if (webcam.startCamera()==1)
        {
            timerId= startTimer( 20 );
            newDetector.startClipFace(13);
        }
        else
        {
            QMessageBox::warning(this,"Error","Error Opening Camera");

        }

}

void MainWindow::timerEvent(QTimerEvent *)
{
    IplImage * queryImage = webcam.queryFrame();
    newDetector.runDetector(queryImage);
    qDebug()<<newDetector.queryMessage();
    static webcamImagePaint newWebcamImagePaint;
    newWebcamImagePaint.paintCyclops(queryImage, newDetector.eyesInformation.LE, newDetector.eyesInformation.RE);
    newWebcamImagePaint.paintEllipse(queryImage, newDetector.eyesInformation.LE, newDetector.eyesInformation.RE);
    if(!rec)
    {
        IplImage *test=newDetector.clipFace(queryImage);
          if (newVerifier.verifyFace(test)==1)
          {
              rec=true;
              QMessageBox::warning(this,"OK","OK!");
          }
          //cvReleaseImage(&queryImage);
          cvReleaseImage(&test);
          qDebug()<<"testing";
    }
    QImage * qm=QImageIplImageCvt(queryImage);
    setQImageWebcam(qm);
    delete qm;
    cvReleaseImage(&queryImage);
}

MainWindow::~MainWindow()
{
    delete desktop;
    delete scene;
    delete ui;

}
void MainWindow::setQImageWebcam(QImage *input)
{
    if (!input)
        return ;
    //
    this->ui->graphicsView->setBackgroundBrush(*input);
    this->ui->graphicsView->show();

}
