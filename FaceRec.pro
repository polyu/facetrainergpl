#-------------------------------------------------
#
# Project created by QtCreator 2012-10-31T15:24:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FaceRec
TEMPLATE = app


SOURCES +=\
    webcamImagePaint.cpp \
    verifier.cpp \
    utils.cpp \
    tracker.cpp \
    qtUtils.cpp \
    opencvWebcam.cpp \
    faceDetector.cpp \
    eyesDetector.cpp \
    detector.cpp \
    mainwindow.cpp \
    main.cpp \
    usermanager.cpp

HEADERS  += \
    verifier.h \
    utils.h \
    tracker.h \
    qtUtils.h \
    opencvWebcam.h \
    faceDetector.h \
    eyesDetector.h \
    detector.h \
    webcamImagePaint.h \
    mainwindow.h \
    faceTrainer.h \
    aboutBox.h \
    faceTrainerAdvSettings.h \
    usermanager.h \
    pam_face_defines.h \
    pam_face_authentication.h

FORMS    += mainwindow.ui \
    faceTrainerAdvSettings.ui \
    faceTrainer.ui \
    faceAuth.ui \
    aboutDialog.ui

OTHER_FILES +=

LIBS+=  \
    /usr/lib/libopencv_core.so    \
    /usr/lib/libopencv_highgui.so \
    /usr/lib/libopencv_imgproc.so \
    /usr/lib/libopencv_objdetect.so \

RESOURCES += \
    graphics.qrc



