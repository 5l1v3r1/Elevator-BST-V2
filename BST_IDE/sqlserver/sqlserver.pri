INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/..

SOURCES += $$PWD/sqlquery.cpp
HEADERS += $$PWD/sqlquery.h

contains(DEFINES, IDE) {
    SOURCES += $$PWD/sqlserverform.cpp \
               $$PWD/loginform.cpp \
               $$PWD/devicequeryform.cpp

    HEADERS += $$PWD/sqlserverform.h \
               $$PWD/loginform.h\
               $$PWD/devicequeryform.h

    FORMS += $$PWD/sqlserverform.ui \
             $$PWD/loginform.ui \
             $$PWD/devicequeryform.ui
}
