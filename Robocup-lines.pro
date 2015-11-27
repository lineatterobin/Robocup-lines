TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += ./src/main.cpp

INCLUDEPATH += /opt/opencv/include/opencv2
QMAKE_LFLAGS += -Wl,-R/opt/opencv/lib -L/opt/opencv/lib

LIBS += \
-lopencv_core \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_features2d \
-lopencv_nonfree\
-lopencv_calib3d

