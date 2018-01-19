INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/..

SOURCES +=  $$PWD/textedit.cpp \
            $$PWD/switch.cpp \
            $$PWD/button.cpp \
            $$PWD/checkbox.cpp \
            $$PWD/combobox.cpp \
            $$PWD/toolcontainer.cpp \
            $$PWD/toolbase.cpp \
            $$PWD/spinedit.cpp \
            $$PWD/groupbox.cpp \
            $$PWD/label.cpp \
            $$PWD/gifplayer.cpp \
            $$PWD/serialport.cpp \
            $$PWD/tooltcp.cpp \
            $$PWD/tooludp.cpp

HEADERS +=  $$PWD/textedit.h \
            $$PWD/switch.h \
            $$PWD/button.h \
            $$PWD/checkbox.h \
            $$PWD/combobox.h \
            $$PWD/tool.h \
            $$PWD/toolcontainer.h \
            $$PWD/toolbase.h \
            $$PWD/spinedit.h \
            $$PWD/groupbox.h \
            $$PWD/label.h \
            $$PWD/gifplayer.h \
            $$PWD/serialport.h \
            $$PWD/tooltcp.h \
            $$PWD/tooludp.h

contains(DEFINES, SURRPORT_UDT)     {
    contains(DEFINES, WINDOWS) {
        contains(DEFINES, MSVC) {
            INCLUDEPATH += $$PWD/udt/Windows/include/
            HEADERS +=  $$PWD/tooludt.h
            SOURCES +=  $$PWD/tooludt.cpp
            LIBS += $$PWD/udt/Windows/lib/udt.lib  ws2_32.lib
        }
    }
    contains(DEFINES, UBUNTU) {
        HEADERS +=  $$PWD/tooludt.h
        SOURCES +=  $$PWD/tooludt.cpp
        LIBS += $$PWD/udt/Ubuntu/lib/libudt.a \
                $$PWD/udt/Ubuntu/lib/libudt.so
    }
    contains(DEFINES, LINUX) {
        HEADERS +=  $$PWD/tooludt.h
        SOURCES +=  $$PWD/tooludt.cpp
        LIBS += $$PWD/udt/Linux/lib/libudt.a \
                $$PWD/udt/Linux/lib/libudt.so
    }
}

contains(DEFINES, IDE) {
    SOURCES +=  $$PWD/dieselinstrument.cpp\
                $$PWD/speedinstrument.cpp\
                $$PWD/timeinstrument.cpp \
                $$PWD/pieinstrument.cpp \
                $$PWD/catalogtreewidget.cpp \
                $$PWD/upitem.cpp \
                $$PWD/streammedialabel.cpp \
                $$PWD/previewlabel.cpp \
                $$PWD/webview.cpp \
                $$PWD/mutiitem.cpp \
                $$PWD/playlabel.cpp

    HEADERS +=  $$PWD/dieselinstrument.h\
                $$PWD/speedinstrument.h\
                $$PWD/timeinstrument.h \
                $$PWD/pieinstrument.h \
                $$PWD/catalogtreewidget.h \
                $$PWD/upitem.h \
                $$PWD/streammedialabel.h \
                $$PWD/previewlabel.h \
                $$PWD/webview.h \
                $$PWD/mutiitem.h \
                $$PWD/playlabel.h
}

contains(DEFINES, SURRPORT_PINYIN) {
    HEADERS +=  $$PWD/syszuxim.h    $$PWD/syszuxpinyin.h
    SOURCES +=  $$PWD/syszuxim.cpp  $$PWD/syszuxpinyin.cpp
}

contains(DEFINES, SURRPORT_ACTIVEX) {
    HEADERS +=  $$PWD/axwidget.h
    SOURCES +=  $$PWD/axwidget.cpp
}
