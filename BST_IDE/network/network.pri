INCLUDEPATH +=  ../BST_IDE/network/

contains(DEFINES, IDE) {
    HEADERS += ../BST_IDE/network/bearercloud.h \
              ../BST_IDE/network/cloud.h \
              ../BST_IDE/network/networkmanagerform.h
    SOURCES += ../BST_IDE/network/bearercloud.cpp \
              ../BST_IDE/network/cloud.cpp \
              ../BST_IDE/network/networkmanagerform.cpp
    FORMS += ../BST_IDE/network/networkmanagerform.ui
}
