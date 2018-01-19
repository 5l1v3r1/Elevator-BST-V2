INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/..

HEADERS +=  $$PWD/projectmanager.h \
            $$PWD/treeitemclass.h \
            $$PWD/deviceclass.h \
            $$PWD/modalwindow.h \
            $$PWD/deviceinfo.h

SOURCES +=  $$PWD/projectmanager.cpp \
            $$PWD/treeitemclass.cpp \
            $$PWD/deviceclass.cpp \
            $$PWD/modalwindow.cpp \
            $$PWD/deviceinfo.cpp

FORMS +=    $$PWD/prowizard.ui \
            $$PWD/devwizard.ui \
            $$PWD/deviceinfo.ui
