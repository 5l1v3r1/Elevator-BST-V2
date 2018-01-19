INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/..

contains(DEFINES, SURRPORT_OPENCV) {
    contains(DEFINES, WINDOWS) {
        INCLUDEPATH += $$PWD/OpenCV/Windows/include/
        LIBS += -LD:/OpenCV2.0/lib/ -lcv200 -lcvaux200 -lhighgui200 -lcxcore200
    }
    contains(DEFINES, UBUNTU) {
    }
    contains(DEFINES, LINUX) {
    }
}

HEADERS += $$PWD/facedetectform.h
SOURCES += $$PWD/facedetectform.cpp

FORMS +=   $$PWD/facedetectform.ui
