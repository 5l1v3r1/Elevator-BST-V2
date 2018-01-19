INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/..

SOURCES +=  $$PWD/stcpage.cpp \
            $$PWD/jstextedit.cpp \
            $$PWD/stoform.cpp \
            $$PWD/pieview.cpp

HEADERS  += $$PWD/stcpage.h \
            $$PWD/jstextedit.h \
            $$PWD/stoform.h \
            $$PWD/pieview.h

FORMS   += $$PWD/stcpage.ui \
           $$PWD/stoform.ui

OTHER_FILES += $$PWD/qtdata.cht
