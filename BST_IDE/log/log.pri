INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/..

contains(DEFINES, WINDOWS) {
    INCLUDEPATH += $$PWD/log4qt/Windows/include/
    LIBS += $$PWD/log4qt/Windows/lib/liblog4qt.a
}
contains(DEFINES, UBUNTU) {
    INCLUDEPATH += $$PWD/log4qt/Ubuntu/include/
    LIBS += $$PWD/log4qt/Ubuntu/lib/liblog4qt.so
}
contains(DEFINES, LINUX) {
    INCLUDEPATH += $$PWD/log4qt/Linux/include/
    LIBS += $$PWD/log4qt/Linux/lib/liblog4qt.a
}

contains(DEFINES, IDE){
    HEADERS += $$PWD/logquerypage.h
    SOURCES += $$PWD/logquerypage.cpp
    FORMS +=   $$PWD/logquerypage.ui
}



