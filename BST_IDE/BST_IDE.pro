#-------------------------------------------------
#
# Project created by QtCreator 2012-06-07T08:20:45
#
#-------------------------------------------------

QT       += core gui

TARGET = BST_IDE
TEMPLATE = app
CONFIG += thread

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES +=  WINDOWS  # WINDOWS LINUX UBUNTU FEDERO
DEFINES +=  GCC     # GCC MSVC
DEFINES +=  IDE

#DEFINES += SURRPORT_PINYIN  # 中文拼音输入支持
DEFINES += SURRPORT_MULTIMEDIA
DEFINES += MPLAYER          # 视频驱动：QtAV PHONON MPLAYER GSTREAMER FFMPEG
DEFINES += SURRPORT_OPENCV
DEFINES += SURRPORT_NETWORK
DEFINES += SURRPORT_SQL
DEFINES += SURRPORT_SVG
DEFINES += SURRPORT_SCRIPT
#DEFINES += SURRPORT_ACTIVEX
DEFINES += SURRPORT_XML
#DEFINES += SURRPORT_UDT

DEFINES += SURRPORT_3D
DEFINES += SURRPORT_WEBKIT
DEFINES += SURRPORT_QML


contains(DEFINES, SURRPORT_NETWORK)     { QT += network }
contains(DEFINES, SURRPORT_WEBKIT)      { QT += webkit}
contains(DEFINES, SURRPORT_XML)         { QT += xml xmlpatterns }
contains(DEFINES, SURRPORT_MULTIMEDIA)  { QT += multimedia }
contains(DEFINES, SURRPORT_3D)          { QT += opengl }
contains(DEFINES, SURRPORT_SQL)         { QT += sql }
contains(DEFINES, SURRPORT_SVG)         { QT += svg }
contains(DEFINES, SURRPORT_QML)         { QT += declarative }
contains(DEFINES, SURRPORT_SCRIPT)      {
    QT += script
    contains(QT_CONFIG, scripttools){ QT += scripttools }
}
contains(DEFINES, SURRPORT_XML)         {
    QT += xml
    !contains(DEFINES, IDE) { QT += xmlpatterns }
}
contains(DEFINES, SURRPORT_ACTIVEX)     { CONFIG += qaxcontainer }

include(../BST_IDE/global/global.pri)
include(../BST_IDE/project/project.pri)
include(../BST_IDE/devicewatcher/devicewatcher.pri)
include(../BST_IDE/tabmanager/tabmanager.pri)
include(../BST_IDE/window/window.pri)
include(../BST_IDE/graphics/graphics.pri)
include(../BST_IDE/physical/physical.pri)
include(../BST_IDE/logic/logic.pri)
include(../BST_IDE/ascii/ascii.pri)
include(../BST_IDE/facedetect/facedetect.pri)
include(../BST_IDE/multimedia/multimedia.pri)
include(../BST_IDE/completer/completer.pri)
include(../BST_IDE/image/image.pri)
include(../BST_IDE/findform/findform.pri)
include(../BST_IDE/init/init.pri)
include(../BST_IDE/xml/xml.pri)
include(../BST_IDE/ftp/ftp.pri)
include(../BST_IDE/queryform/queryform.pri)
include(../BST_IDE/ui/ui.pri)
include(../BST_IDE/upgrade/upgrade.pri)
include(../BST_IDE/mdp/mdp.pri)
include(../BST_IDE/eventmap/eventmap.pri)
include(../BST_IDE/manufacture/manufacture.pri)
include(../BST_IDE/stc/stc.pri)
include(../BST_IDE/rc/rc.pri)
include(../BST_IDE/sqlserver/sqlserver.pri)
include(../BST_IDE/simulator/simulator.pri)
include(../BST_IDE/sms/sms.pri)
include(../BST_IDE/splash/splash.pri)
include(../BST_IDE/zip/zip.pri)
include(../BST_IDE/log/log.pri)
include(../BST_IDE/fonteditor/fonteditor.pri)
include(../BST_IDE/chat/chat.pri)
include(../BST_IDE/maps/maps.pri)
include(../BST_IDE/tool/tool.pri)
include(../BST_IDE/network/network.pri)
include(../BST_IDE/interface/interface.pri)

SOURCES += ../BST_IDE/main.cpp

INCLUDEPATH += ../BST_IDE/

RESOURCES += ../BST_IDE/IDE.qrc

TRANSLATIONS += IDE.ts

VERSION = 1.0.0.7
