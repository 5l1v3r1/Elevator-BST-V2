#include <QtGui/QApplication>
#include <QTranslator>
#include "global.h"
#include "mainwindow.h"
#include "splashscreen.h"

//>@+++++++++++++++++++++++++++++++++++++++++++++++++++
//>@+                                                 +
//>@+        Main                                     +
//>@+                                                 +
//>@+                                                 +
//>@+++++++++++++++++++++++++++++++++++++++++++++++++++

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));

    //桌面右下角图标
    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("Can't' detect any system tray "
                                          "on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    //Splash
    QPixmap tmpPixmap = QPixmap(":/Splash/rc/Splash/Splash.jpg");
    SplashScreen Screen(tmpPixmap);
    Screen.show();
    a.processEvents();

    QString tmpString;  //>@

    //>@+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    tmpString = GetHostPCInfo();
    Screen.ShowMsg(tmpString, Qt::darkRed);
    Screen.updateProgressBar(10);

    //>@+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if(GetScreenInfo())
    {
        tmpString = QObject::tr("Desktop Size:(%1,%2)\nScreen Size:(%3,%4)\nScreen Num:%5")
                                    .arg(GlobalPara.m_DeskRect.width())
                                    .arg(GlobalPara.m_DeskRect.height())
                                    .arg(GlobalPara.m_ScreenRect.width())
                                    .arg(GlobalPara.m_ScreenRect.height())
                                    .arg(GlobalPara.m_ScreenNum);
        Screen.ShowMsg(tmpString, Qt::darkRed);
        Screen.updateProgressBar(20);
    }

    //>@+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Screen.ShowMsg(QObject::tr("Getting Application directory ..."), Qt::darkRed);
    Screen.updateProgressBar(30);
    GetApplicationDirectory();

    //>@+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Screen.ShowMsg(QObject::tr("Initializing Logger ..."), Qt::darkRed);
    Screen.updateProgressBar(40);
    if(InitLoggerInfo() == false)
    {
        Screen.ShowMsg(QObject::tr("Can not find Log configure file!"), Qt::red);
    }

    //>@+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Screen.ShowMsg(QObject::tr("Getting boot configuration to Initializing software ..."), Qt::darkRed);
    Screen.updateProgressBar(50);
    if(GetBootConfigure() == false)
    {
        Screen.ShowMsg(QObject::tr("Can not find boot configurate file!"), Qt::red);
    }

    //>@+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Screen.ShowMsg(QObject::tr("Loading Language-Internationalization-File ..."), Qt::darkRed);
    Screen.updateProgressBar(60);
    QTranslator translator;
    QString qmFile = GlobalPara.m_SoftwareDir + QString("/RuntimeRc/") + QString("IDE.qm");
    IDE_TRACE_STR(qmFile);
    translator.load(qmFile);   //将刚才发布的.qm文件加载到程序中
    a.installTranslator(&translator);

    //>@+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //>@初始化软件
    Screen.ShowMsg(QObject::tr("Initializing Software ..."), Qt::darkRed);
    Screen.updateProgressBar(70);

    GlobalPara.m_MainWindow = new MainWindow(0);
    //GlobalPara.m_MainWindow->setAttribute(Qt::WA_DeleteOnClose);
    GlobalPara.m_MainWindow->setWindowIcon(QIcon(":/Page/rc/Page/bstpro.png"));
    GlobalPara.m_MainWindow->setWindowTitle(QObject::tr("BST IDE"));
#ifdef WINDOWS
    GlobalPara.m_MainWindow->setMinimumSize ( 800, 600 );
#endif
    Screen.updateProgressBar(99);
    Screen.finish(GlobalPara.m_MainWindow);

    GlobalPara.m_MainWindow->setMinimumSize(1024, 768);
    GlobalPara.m_MainWindow->showMaximized();

    GlobalPara.m_ScriptEngine = new QScriptEngine(0);

    Logger("NetWorkInterrupt", QStringList() << "192.168.1.1" << "6666");
    Logger("NetWorkInterrupt", QStringList() << "192.168.1.1" << "6666");
    Logger("NetWorkInterrupt", QStringList() << "192.168.1.1" << "6666");

    return a.exec();
}
