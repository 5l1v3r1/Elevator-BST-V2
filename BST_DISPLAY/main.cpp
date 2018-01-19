#include <QtGui/QApplication>
#include <QGraphicsView>
#include "devicemanager.h"
#ifdef LINUX
#include <QWSServer>
#endif

int main(int argc, char *argv[])
{
#ifdef LINUX
    QApplication a(argc, argv, QApplication::GuiServer);
    QWSServer::setCursorVisible(false);
#else
    QApplication a(argc, argv);
#endif

    GetApplicationDirectory();
    if(InitLoggerInfo() == false)
    {
        IDE_DEBUG("Can't find Log Configure file!");
    }

    Logger("> -- BST_DISPLAY started!");

    //SystemAudio("D:/MyProject/IDE/BST_DISPLAY/rc/SystemAudio/open.wav");

    GlobalPara.m_DeviceManager = new DeviceManager(QRect(0,0,a.desktop()->width(),a.desktop()->height()));
    IDE_TRACE_INT(a.desktop()->width());
    IDE_TRACE_INT(a.desktop()->height());
    GlobalPara.m_DeviceManager->setWindowTitle(QObject::tr("BST_DISPLAY"));
    GlobalPara.m_DeviceManager->show();

    int ret = a.exec();
    Logger("> -- BST_DISPLAY exited!");
    return ret;
}
