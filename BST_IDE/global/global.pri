INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/..

HEADERS += $$PWD/define.h

contains(DEFINES, WINDOWS) {
    LIBS  += $$PWD/lib/odbc32.lib\
             $$PWD/lib/odbccp32.lib\
             $$PWD/lib/winmm.lib\
             $$PWD/lib/usbd.lib\
             $$PWD/lib/setupapi.lib\
             $$PWD/lib/lmusbdll.lib\
             $$PWD/lib/wsock32.lib
}

!contains(DEFINES, START) {
    SOURCES += $$PWD/global.cpp\
               $$PWD/globalfun.cpp \
               $$PWD/highlighter.cpp

    HEADERS += $$PWD/global.h \
               $$PWD/globalfun.h   \
               $$PWD/highlighter.h \
               $$PWD/define.h
}
