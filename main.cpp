/** @file */

/*
    QT Face Trainer MAIN
    Copyright (C) 2009 Rohan Anil (rohan.anil@gmail.com) -BITS Pilani Goa Campus
    http://code.google.com/p/pam-face-authentication/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <dirent.h>
#include <QInputDialog>
#include <QtGui>
#include <QApplication>
#include <unistd.h>
#include <QMessageBox>
#include "opencv2/opencv.hpp"
#include "faceTrainer.h"
#include "aboutBox.h"
#include "webcamImagePaint.h"
#include "utils.h"
#include "qtUtils.h"
#include <QMessageBox>
#include <iostream>
#include <list>
#include <string>
#include <cctype>
using namespace std;


void faceTrainer::setIbarText(QString message)
{
    ui.lbl_ibar->setText(message);

}

void faceTrainer:: populateQList()
{
    ui.lv_thumbnails->clear();

    setFace* faceSetStruct =newVerifier.getFaceSet();
    int i=0;
    for (i=0;i<faceSetStruct->count;i++)
    {
        char setName[100];
        sprintf(setName,"Set %d",i+1);
        //ui.lv_thumbnails->setIconSize(QSize(60, 60));
        QListWidgetItem *item = new QListWidgetItem(setName,ui.lv_thumbnails);
        item->setIcon(QIcon(faceSetStruct->setFilePathThumbnails[i]));
        QString qstring(faceSetStruct->setName[i]);
        //   printf("%s \n",faceSetStruct->setName[i]);
        item->setData(Qt::UserRole,qstring);


    }


}
void faceTrainer::finalCheck()
{
    bool isOk=false;
    UserManager manager;
    manager.loadSetting();
    vector<string> vname=manager.getUserList();
    vector<verifier*> vlist;
    int j=0;
    for(j=0;j<vname.size();j++)
    {
         vlist.push_back(new verifier(vname.at(j).c_str()));
    }

    for(int i=0;i<30;i++)
    {
        IplImage * queryImage = webcam.queryFrame();
        IplImage *test=newDetector.clipFace(queryImage);
        for(int d=0;d<vlist.size();d++)
        {
            if(vlist.at(d)->verifyFace(test)==1);
            {
                if(strcmp(vname.at(d).c_str(),getenv("facetrainername2"))==0)
                {
                    isOk=true;
                }
            }
        }
    }
    if(isOk)
    {
        ui.pb_finish->setEnabled(true);
        QMessageBox::warning(this,"OK","Face trainning has been finished.");
    }
    else
    {
        QMessageBox::warning(this,"OK","Face trainning failed due to bad samples.");
        ui.stkWg->setCurrentIndex(1);
    }



}

void faceTrainer::setQImageWebcam(QImage *input)
{
    if (!input)
        return ;
    
    static QGraphicsScene * scene = new QGraphicsScene(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    ui.webcamPreview->setScene(scene);
    ui.webcamPreview->setBackgroundBrush(*input);

    ui.webcamPreview->show();
    static QGraphicsScene * scene2 = new QGraphicsScene(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    ui.webcamPreview_2->setScene(scene);
    ui.webcamPreview_2->setBackgroundBrush(*input);

    ui.webcamPreview_2->show();

}
void faceTrainerAdvSettings::setQImageWebcam(QImage *input)
{
    if (!input)
        return ;

    static QGraphicsScene * scene = new QGraphicsScene(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    ui.webcamPreview->setScene(scene);
    ui.webcamPreview->setBackgroundBrush(*input);

    ui.webcamPreview->show();

}
void faceTrainerAdvSettings::saveClicked()
{
    config newConfig;
    newConfig.percentage=((double)ui.percentage->value()/100);
    //  newConfig.filterMaceFacePSLR= ui.sb_face->value();
//   newConfig.filterMaceInsideFacePSLR= ui.sb_insideFace->value();
    setConfig(&newConfig,configDirectory);
}

void faceTrainerAdvSettings::restoreDefaults()
{
    initConfig();
    ui.percentage->setValue(80);
    // ui.sb_face->setValue(MACE_FACE_DEFAULT);
    //  ui.sb_eye->setValue(MACE_EYE_DEFAULT);
    // ui.sb_insideFace->setValue(MACE_INSIDE_FACE_DEFAULT);
    saveClicked();
    newVerifier->createBiometricModels(NULL);

}

void faceTrainerAdvSettings::sT(opencvWebcam *wc,detector *nd,verifier *nv)
{
    webcam=wc;
    newDetector=nd;
    newVerifier=nv;
    startTimer( 20 );
}
void faceTrainerAdvSettings::testRecognition_do()
{

	if(this->testtime>=TT)
    {
        this->testflag=false;
        this->timer->stop();
        QMessageBox::warning(this,"Result",QString("Total Test Time:%2, Successful Time:%1").arg(this->successful).arg(TT));

        this->ui.pb_tr->setEnabled(true);
        this->successful=0;
	printf("\n");
        return;
    }
    testtime++;
    //qApp->processEvents();
    IplImage * queryImage = webcam->queryFrame();
    IplImage *test=newDetector->clipFace(queryImage);
    UserManager manager;
    if(!manager.loadSetting())
    {
        QMessageBox::warning(this,"fail load!","fail load!");
    }
    //manager.print();
    //unsetenv("facetrainername");



    if (newVerifier->verifyFace(test)==1)
{
	printf("+");
	fflush(stdout);
        successful++;
}
else
{
	printf("-");
	fflush(stdout);
}
        cvReleaseImage(&queryImage);
        cvReleaseImage(&test);
}

void faceTrainerAdvSettings:: testRecognition()
{
    this->testtime=0;
    this->successful=0;
    timer = new QTimer(this);

    connect(timer,SIGNAL(timeout()),this,SLOT(testRecognition_do()));
    timer->start(500);
    this->ui.pb_tr->setEnabled(false);
}

faceTrainerAdvSettings::faceTrainerAdvSettings(QWidget *parent, char* configDir)
        : QDialog(parent)
{
    configDirectory=configDir;
    ui.setupUi(this);
    connect(ui.pb_save,SIGNAL(clicked()), this, SLOT(saveClicked()));
    connect(ui.pb_restore,SIGNAL(clicked()), this, SLOT(restoreDefaults()));
    connect(ui.pb_tr,SIGNAL(clicked()), this, SLOT(testRecognition()));
    this->testflag=false;
    initConfig();
    TT=20;
    connect(ui.spinBox,SIGNAL(valueChanged(int)),this,SLOT(handleTT(int)));

}
void faceTrainerAdvSettings::handleTT(int value)
{
   TT=value;
   if(TT>100&&TT<1)
   {
        ui.spinBox->setValue(20);
        TT=20;
   }
}

void faceTrainerAdvSettings::initConfig()
{
    config* newConfig;

    newConfig=getConfig(configDirectory);
    ui.percentage->setValue(int(newConfig->percentage*100));
    //  ui.sb_face->setValue(newConfig->filterMaceFacePSLR);
    ////  ui.sb_eye->setValue(newConfig->filterMaceEyePSLR);
    //  ui.sb_insideFace->setValue(newConfig->filterMaceInsideFacePSLR);
}

aboutBox::aboutBox(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);

}
faceTrainer::faceTrainer(QWidget *parent)
        : QMainWindow(parent)
{
    ui.setupUi(this);
    QDesktopWidget *desktop = QApplication::desktop();
    rec=false;
    int screenWidth, width;
    int screenHeight, height;
    int x, y;
    QSize windowSize;
    qDebug()<<this->newVerifier.configDirectory;
    screenWidth = desktop->width(); // get width of screen
    screenHeight = desktop->height(); // get height of screen

    windowSize = size(); // size of our application window
    width = windowSize.width();
    height = windowSize.height();

// little computations
    x = (screenWidth - width) / 2;
    y = (screenHeight - height) / 2;
    y -= 50;

// move window to desired coordinates
    move ( x, y );

    ui.stkWg->setCurrentIndex(0);
    connect(ui.pb_capture,SIGNAL(clicked()), this, SLOT(captureClick()));
    connect(ui.pb_next_t2,SIGNAL(clicked()), this, SLOT(showTab3()));
    connect(ui.pb_next_t1,SIGNAL(clicked()), this, SLOT(showTab2()));
    connect(ui.pb_back_t2,SIGNAL(clicked()), this, SLOT(showTab1()));
    connect(ui.pb_about,SIGNAL(clicked()), this, SLOT(about()));
    connect(ui.pb_ds,SIGNAL(clicked()), this, SLOT(removeSelected()));
    connect(ui.pb_adv,SIGNAL(clicked()), this, SLOT(showAdvDialog()));
    connect(ui.pb_tr,SIGNAL(clicked()),this,SLOT(finalCheck()));
    //connect(ui.button1,SIGNAL(clicked()), this, SLOT(verify()));
    //connect(ui.but,SIGNAL(clicked()), this, SLOT(butClick()));


}
void faceTrainer::about()
{


    aboutBox newAboutBox;
    newAboutBox.exec();
}
void faceTrainer::showAdvDialog()
{
    faceTrainerAdvSettings*    newDialog= new faceTrainerAdvSettings(this,newVerifier.configDirectory);
    newDialog->initConfig();
    newDialog->sT(&webcam,&newDetector,&newVerifier);
    newDialog->exec();
    delete newDialog;

}



void faceTrainer::removeSelected()
{

    QList<QListWidgetItem *>  list=ui.lv_thumbnails->selectedItems();
    QList<QListWidgetItem *>::iterator i;

    for (i = list.begin(); i != list.end(); ++i)
    {
        QListWidgetItem *item = *i;
        QString dat=item->data(Qt::UserRole).toString();
        char *ptr =  dat.toAscii().data();
        //  printf("%s \n",ptr);

        newVerifier.removeFaceSet(ptr);

    }
    ui.lv_thumbnails->clear();

    populateQList();



}
void faceTrainer::captureClick()
{

    static int latch=0;
    if (latch==0)
    {
        ui.pb_capture->setText(tr("Cancel"));
        latch=1;
        newDetector.startClipFace(13);
    }
    else
    {
        ui.pb_capture->setText(tr("Capture"));
        latch=0;
        newDetector.stopClipFace();
    }


}
void faceTrainer::timerEvent( QTimerEvent * )
{
    IplImage * queryImage = webcam.queryFrame();
    newDetector.runDetector(queryImage);

    setIbarText(tr(newDetector.queryMessage()));
    //if(newDetector.checkEyeDetected()==1)
    //newVerifier.verifyFace(newDetector.clipFace(queryImage));
    //this works captureClick();
    //double t = (double)cvGetTickCount();

    static webcamImagePaint newWebcamImagePaint;
    newWebcamImagePaint.paintCyclops(queryImage, newDetector.eyesInformation.LE, newDetector.eyesInformation.RE);
    newWebcamImagePaint.paintEllipse(queryImage, newDetector.eyesInformation.LE, newDetector.eyesInformation.RE);
    //  cvLine(queryImage, newDetector.eyesInformation.LE, newDetector.eyesInformation.RE, cvScalar(0,255,0), 4);

    QImage * qm=QImageIplImageCvt(queryImage);

    if (newDetector.finishedClipFace()==1)
    {
        setIbarText(tr("Processing Faces, Please Wait ..."));
        // cvWaitKey(1000);
        newVerifier.addFaceSet(newDetector.returnClipedFace(),13);
        setIbarText(tr("Processing Completed."));

        captureClick();
        populateQList();

    }

    setQImageWebcam(qm);


    cvWaitKey(1);
    // sleep(1);

    delete qm;
    cvReleaseImage(&queryImage);

}


void faceTrainerAdvSettings::timerEvent( QTimerEvent * )
{
    IplImage * queryImage = webcam->queryFrame();
    newDetector->runDetector(queryImage);
    //if(newDetector->checkEyeDetected()==1)
    //newVerifier->verifyFace(newDetector->clipFace(queryImage));
    //this works captureClick();
    //double t = (double)cvGetTickCount();
    static webcamImagePaint newWebcamImagePaint;
    newWebcamImagePaint.paintCyclops(queryImage, newDetector->eyesInformation.LE, newDetector->eyesInformation.RE);
    newWebcamImagePaint.paintEllipse(queryImage, newDetector->eyesInformation.LE, newDetector->eyesInformation.RE);

//  cvLine(queryImage, newDetector->eyesInformation.LE, newDetector->eyesInformation.RE, cvScalar(0,255,0), 4);
//newVerifier->verifyFace(newDetector->clipFace(queryImage));
    QImage * qm=QImageIplImageCvt(queryImage);
    setQImageWebcam(qm);
    cvWaitKey(1);
    cvReleaseImage(&queryImage);
    delete qm;

}
void faceTrainer::showTab2()
{
    if (webcam.startCamera()==1)
    {
        ui.stkWg->setCurrentIndex(1);
        populateQList();
        timerId= startTimer( 20 );
    }
    else
    {
        QMessageBox msgBox1;
        msgBox1.setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        msgBox1.setWindowTitle(tr("Face Trainer"));
        msgBox1.setText(tr("<strong>Error</strong>"));
        msgBox1.setInformativeText(tr(
                                       "Camera Not Found. <br /> "
                                       "Plugin Your Camera and Try Again."));
        msgBox1.setStandardButtons(QMessageBox::Ok);
        msgBox1.setIconPixmap(QPixmap(":/data/ui/images/cnc.png"));
        msgBox1.exec();
    }
}

void faceTrainer::showTab1()
{
    killTimer(timerId);
    webcam.stopCamera();

    ui.stkWg->setCurrentIndex(0);

}
void faceTrainer::showTab3()
{
    ui.stkWg->setCurrentIndex(2);
}



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QString name;
    while(name.isEmpty())
    name=QInputDialog::getText(0,"User Manager","Please input your name.");

    unsetenv("facetrainername");
    unsetenv("facetrainername2");
    char _env[200];
    char _env2[200];
    sprintf(_env,"facetrainername=%s",name.toAscii().data());
    sprintf(_env2,"facetrainername2=%s",name.toAscii().data());
    printf(_env);
    putenv(_env);
    putenv(_env2);
    faceTrainer tab1;
    //tab1.setWindowTitle("Face Trainer");
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("qt_facetrainer_" + QLocale::system().name());
    app.installTranslator(&myappTranslator);

    QRect r = tab1.geometry();
    r.moveCenter(QApplication::desktop()->availableGeometry().center());
    tab1.setGeometry(r);


    tab1.show();
    return app.exec();
}

