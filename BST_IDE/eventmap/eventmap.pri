INCLUDEPATH += ../BST_IDE/eventmap/

HEADERS += ../BST_IDE/eventmap/eventmap.h
SOURCES += ../BST_IDE/eventmap/eventmap.cpp

contains(DEFINES, IDE) {
    HEADERS += ../BST_IDE/eventmap/eventmapform.h
    SOURCES += ../BST_IDE/eventmap/eventmapform.cpp
    FORMS += ../BST_IDE/eventmap/eventmapform.ui
}








