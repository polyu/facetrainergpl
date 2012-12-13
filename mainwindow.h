#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "opencvWebcam.h"
#include "detector.h"
#include "verifier.h"
#include <QDesktopWidget>
#include <QGraphicsScene>
#include "webcamImagePaint.h"
#include <QMessageBox>

#include "qtUtils.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void timerEvent(QTimerEvent *);
private:
    Ui::MainWindow *ui;
    QDesktopWidget *desktop;
    opencvWebcam webcam;
    detector newDetector;
    verifier newVerifier;
    QGraphicsScene * scene;
    int timerId;
    void setQImageWebcam(QImage *);

    bool rec;
};

#endif // MAINWINDOW_H
