#-------------------------------------------------
#
# Project created by QtCreator 2012-08-20T08:55:46
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4) { QT += widgets }

TARGET = BST_DISPLAY
TEMPLATE = app
CONFIG += thread

DEFINES +=  N329     # N329 AM335X DM3730 IMX6X
DEFINES +=  LINUX  # WINDOWS LINUX UBUNTU FEDERO
DEFINES +=  GCC     # GCC MSVC
DEFINES +=  DISPLAY \
            SURRPORT_RGB888     # LVDS输出格式：RGB565 RGB666 RGB888

#DEFINES += SURRPORT_PINYIN  # 中文拼音输入支持
DEFINES += SURRPORT_MULTIMEDIA
DEFINES += MPLAYER          # 视频驱动：QtAV MPLAYER GSTREAMER FFPLAY
DEFINES += SURRPORT_NETWORK
DEFINES += SURRPORT_SQL
DEFINES += SURRPORT_XML
DEFINES += SURRPORT_SVG
DEFINES += SURRPORT_SCRIPT
#DEFINES += SURRPORT_UDT

!contains(DEFINES, N329) {
    DEFINES += SURRPORT_3D
    DEFINES += SURRPORT_WEBKIT
    DEFINES += SURRPORT_QML
}

contains(DEFINES, SURRPORT_NETWORK)     { QT += network }
contains(DEFINES, SURRPORT_WEBKIT)      { QT += webkit}
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
    contains(DEFINES, IDE) { QT += xmlpatterns }
}

include(../BST_IDE/global/global.pri)
include(../BST_IDE/devicewatcher/devicewatcher.pri)
include(../BST_IDE/eventmap/eventmap.pri)
include(../BST_IDE/devicemanager/devicemanager.pri)
include(../BST_IDE/multimedia/multimedia.pri)
include(../BST_IDE/network/network.pri)
include(../BST_IDE/graphics/graphics.pri)
include(../BST_IDE/zip/zip.pri)
include(../BST_IDE/physical/physical.pri)
include(../BST_IDE/logic/logic.pri)
include(../BST_IDE/log/log.pri)
include(../BST_IDE/tool/tool.pri)

SOURCES += main.cpp

RESOURCES += DISPLAY.qrc
