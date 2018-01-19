INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/..

contains(DEFINES, WINDOWS) {
    INCLUDEPATH += $$PWD/qdevicewatcher/Windows/include/
    LIBS += $$PWD/qdevicewatcher/Windows/lib/libQDeviceWatcherd2.a
}
contains(DEFINES, UBUNTU) {
    INCLUDEPATH += $$PWD/qdevicewatcher/Ubuntu/include/
    LIBS += $$PWD/qdevicewatcher/Ubuntu/lib/libQDeviceWatcher.a
}
contains(DEFINES, LINUX) {
    INCLUDEPATH += $$PWD/qdevicewatcher/Linux/include/
    LIBS += $$PWD/qdevicewatcher/Linux/lib/libQDeviceWatcher.a
}

HEADERS += $$PWD/devicewathcer.h
SOURCES += $$PWD/devicewathcer.cpp
