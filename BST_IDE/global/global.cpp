#include "global.h"

const QString bstproManager::BstProSuffix = ".bstpro";
const QString bstproManager::BstProFileType = "BST Solution File";

const QString uiManager::UiSuffix = ".ui";
const QString uiManager::UiFileType = "BST UI File";

const QString mdpManager::MdpSuffix = ".mdp";
const QString mdpManager::MdpFileType = "BST MDP File";

const QString stcManager::StcSuffix = ".stc";
const QString stcManager::StcFileType = "BST STC File";

const QString rcManager::RcSuffix = ".rc";
const QString rcManager::RcFileType = "BST RC File";

const QString cfgManager::BootFileName = "boot.xml";
const QString cfgManager::CfgFileType = "BST IDE File";

const QString logTipManager::LogTipFileName = "TIPS.xml";
const QString logTipManager::LogTipFileType = "BST TIPS File";

const QString cmdManager::CmdSuffix = ".cmd";
const QString cmdManager::CmdFileType = "BST CMD File";

const QString devManager::DevSuffix = ".dev";
const QString devManager::DevFileType = "BST DEV File";

const QString mapManager::MapSuffix = ".map";
const QString mapManager::MapFileType = "BST MAP File";

const QString devInfoManager::DevInfoSuffix = ".devinfo";
const QString devInfoManager::DevInfoFileType = "BST DEVINFO File";

const QString fontManager::FontSuffix = ".font";
const QString fontManager::FontFileType = "BST FONT File";


void Logger(QString Key, QStringList tmplist)
{
    if(GlobalPara.m_Logger == 0)
        return;
    QString tmpcontent;
    LOGTIP_INFO *tmpLogTip = GlobalPara.m_LogTipGroup.value(Key);
    if(tmpLogTip == 0)
        tmpcontent = Key;
    else
        tmpcontent = tmpLogTip->content;
    int count = tmplist.count();
    QString tmpString;
    if(count == 1)
        tmpString = QString(tmpcontent).arg(tmplist.at(0));
    else if(count == 2)
        tmpString = QString(tmpcontent).arg(tmplist.at(0)).arg(tmplist.at(1));
    else if(count == 3)
        tmpString = QString(tmpcontent).arg(tmplist.at(0)).arg(tmplist.at(1)).arg(tmplist.at(2));
    else if(count == 4)
        tmpString = QString(tmpcontent).arg(tmplist.at(0)).arg(tmplist.at(1)).arg(tmplist.at(2)).arg(tmplist.at(3));
    else if(count == 5)
        tmpString = QString(tmpcontent).arg(tmplist.at(0)).arg(tmplist.at(1)).arg(tmplist.at(2)).arg(tmplist.at(3)).arg(tmplist.at(4));
    else
        tmpString = tmpcontent;
    if(tmpLogTip == 0)
        GlobalPara.m_Logger->info(tmpString);
    else if(tmpLogTip->level == "Info")
        GlobalPara.m_Logger->info(tmpString);
    else if(tmpLogTip->level == "Warning")
        GlobalPara.m_Logger->warn(tmpString);
    else if(tmpLogTip->level == "Error")
        GlobalPara.m_Logger->error(tmpString);
    IDE_TRACE_STR(tmpString);
}

void GetApplicationDirectory()
{
    GlobalPara.m_SoftwareDir = QApplication::applicationDirPath();
    if(!GlobalPara.m_SoftwareDir.endsWith("/"))
        GlobalPara.m_SoftwareDir.append("/");
}

//>@打印当前主机信息
QString GetHostPCInfo()
{
    QString tmpString;
#ifdef WINDOWS
    QSysInfo sysInfo;
    QSysInfo::WinVersion tmpVer = sysInfo.windowsVersion();
    switch(tmpVer)
    {
    case QSysInfo::WV_4_0:
        tmpString = QString("OperateSystem  :Win_NT\n");
        break;
    case QSysInfo::WV_5_0:
        tmpString = QString("OperateSystem  :Win_2000");
        break;
    case QSysInfo::WV_5_1:
        tmpString = QString("OperateSystem  :Win_XP\n");
        break;
    case QSysInfo::WV_5_2:
        tmpString = QString("OperateSystem  :Win_2003\n");
        break;
    case QSysInfo::WV_6_0:
        tmpString = QString("OperateSystem  :Win_VISTA\n");
        break;
    case QSysInfo::WV_6_1:
        tmpString = QString("OperateSystem  :Win_WINDOWS7\n");
        break;
    default:break;
    }

    SYSTEM_INFO info;
    GetSystemInfo(&info);
    tmpString.append(QString("CPUs    :%1\nCPU type:%2")
                        .arg(info.dwNumberOfProcessors)
                        .arg(info.dwProcessorType));
#endif
    return tmpString;
}

bool GetScreenInfo()
{
#ifdef IDE
    QDesktopWidget* desktopWidget = QApplication::desktop();
    if(desktopWidget)
    {
        //获取可用桌面大小
        GlobalPara.m_DeskRect = desktopWidget->availableGeometry();
        //获取设备屏幕大小
        GlobalPara.m_ScreenRect = desktopWidget->screenGeometry();
        //获取系统设置的屏幕个数（屏幕拷贝方式该值为1）
        GlobalPara.m_ScreenNum = desktopWidget->screenCount();
        return true;
    }
#endif
    return false;
}

//>@查找日志配置文件，如果查找不到或者配置文件不合法，则退出程序
bool InitLoggerInfo()
{
    //>@读取所支持的日志tips
    QString tmpLogTipFile = D_FSRCPATH + D_BOOTDIR + logTipManager::LogTipFileName;
    logTipManager *tmpLogManager = new logTipManager(tmpLogTipFile);
    if(tmpLogManager->m_Valid == false)
    {
        IDE_TRACE_STR(tmpLogTipFile);
    }
    delete tmpLogManager;
#if 1  //>@根据自身路径设置日志存放路径
    //>@配置文件中的日志存放地址。
    QString tmpLogConfFile = D_FSRCPATH + D_BOOTDIR + D_LOGBKCONF;
    QFile tmpLogFile(tmpLogConfFile);
    if(tmpLogFile.exists() == false || tmpLogFile.open(QIODevice::ReadOnly) == false)
    {
        IDE_TRACE_STR(tmpLogConfFile);
        return false;
    }
    QString line, tmpLogFilePath, tmpLogFileName;
    int tmpMaxLogFileNum = 50;
    QByteArray tmpLogFileBuffer = tmpLogFile.readAll();
    line = QString("log4j.appender.E.MaxBackupIndex=");
    int tmpMaxLogFileNumIndex = tmpLogFileBuffer.indexOf(line);
    if(tmpMaxLogFileNumIndex)
    {
        tmpLogFile.seek(tmpMaxLogFileNumIndex);
        line = tmpLogFile.readLine();
        if(line.contains(QString("log4j.appender.E.MaxBackupIndex")))
        {
            int index = line.indexOf("=");
            QString tmpMaxString = line.mid(index+1);
            bool ok = false;
            int tmpNum = tmpMaxString.toInt(&ok, 10);
            if(ok)
               tmpMaxLogFileNum = tmpNum;
        }
    }
    if(tmpLogFileBuffer.contains(QByteArray("log4j.appender.E.File=")))
    {
        tmpLogFilePath = D_FSRCPATH + D_LOGDIR;
#ifdef IDE
        tmpLogFileName = D_LOGFILE.arg("IDE");
#endif
#ifdef DISPLAY
        tmpLogFileName = D_LOGFILE.arg("DISPLAY");
#endif
        tmpLogFileBuffer.replace(QByteArray("log4j.appender.E.File="), QString("log4j.appender.E.File=%1").arg(tmpLogFilePath+tmpLogFileName).toLatin1());
    }
    tmpLogFile.close();
    tmpLogConfFile = D_FSRCPATH + D_BOOTDIR + D_LOGCONF;
    QFile tmpNewLogFile(tmpLogConfFile);
    if(tmpNewLogFile.exists())
        tmpNewLogFile.remove();
    if(tmpNewLogFile.open(QIODevice::WriteOnly) == false)
    {
        IDE_TRACE_STR(tmpLogConfFile);
        return false;
    }
    tmpNewLogFile.write(tmpLogFileBuffer);
    tmpNewLogFile.close();
#else  //>@获取日志配置文件中的存放路径
    //>@配置文件中的日志存放地址。
    QString tmpLogConfFile = D_FSRCPATH + D_BOOTDIR + logManager::LogConfFileName;
    QFile tmpLogFile(tmpLogConfFile);
    if(tmpLogFile.exists() == false || tmpLogFile.open(QIODevice::ReadWrite) == false)
    {
        IDE_TRACE_STR(tmpLogConfFile);
        return false;
    }
    QString line, tmpLogFilePath, tmpLogFileName;
    int tmpMaxLogFileNum = 50;
    QTextStream in(&tmpLogFile);
    in.seek(0);
    while (in.atEnd() == false)
    {
        line = in.readLine();
        if(line.contains(QString("log4j.appender.E.MaxBackupIndex")))
        {
            int index = line.indexOf("=");
            QString tmpMaxString = line.mid(index+1);
            bool ok = false;
            int tmpNum = tmpMaxString.toInt(&ok, 10);
            if(ok)
               tmpMaxLogFileNum = tmpNum;
        }
        else if(line.contains(QString("log4j.appender.E.File")))
        {
            int index = line.indexOf("=");
            QString tmpLogPath = line.mid(index+1);
            //>@获取文件路径和文件名
            int index1 = tmpLogPath.lastIndexOf("/");
            int index2 = tmpLogPath.lastIndexOf("."); //>@默认为任何文件均带有后缀名，则均有.。
            if(index1 < 0)
            {
                IDE_TRACE_STR(tmpLogPath);
                return false;
            }
            if(index2 < 0)
            {
                IDE_TRACE_STR(tmpLogPath);
                return false;
            }
            if(index1 >= index2)
            {
                IDE_TRACE_STR(tmpLogPath);
                return false;
            }
            tmpLogFilePath = tmpLogPath.mid(0, index1 + 1);
            tmpLogFileName = tmpLogPath.mid(index1 + 1);
            break;
        }
    }
    tmpLogFile.close();
#endif

    //>@使用QtCreate启动应用程序时会将BST_IDE目录作为程序运行目录，而不是BST_IDE.exe所在文件夹作为运行目录，
    //>@但程序单独执行时是以BST_IDE.exe所在文件夹作为运行目录的。
    //>@针对不同运行方式，下一句要进行修改。
    //>@初始化日志文件目录
    IDE_TRACE_INT(tmpMaxLogFileNum);
    IDE_TRACE_STR(tmpLogFilePath);
    IDE_TRACE_STR(tmpLogFileName);
    InitLogPath(tmpLogFilePath, tmpLogFileName, tmpMaxLogFileNum);
    GlobalPara.m_Logger = Log4Qt::Logger::rootLogger();
    GlobalPara.m_Logger->removeAllAppenders();
    Log4Qt::PropertyConfigurator::configure(tmpLogConfFile);
    return true;
}

//>@获取程序初始化信息，初始化软件
bool GetBootConfigure()
{
    QString BootFile = GlobalPara.m_SoftwareDir + QString("/RuntimeRc/") + cfgManager::BootFileName;
    cfgManager *tmpCfgManager = new cfgManager(BootFile);
    if(tmpCfgManager->m_Valid == false)
    {
        IDE_TRACE_STR(BootFile);
        return false;
    }
    tmpCfgManager->m_IsChanged = false;
    delete tmpCfgManager;
    return true;
}

//>@语言国际化
bool InitializeLinguist()
{
    QTranslator translator;
    QString qmFile = GlobalPara.m_SoftwareDir + QString("/RuntimeRc/") + QString("IDE.qm");
    IDE_TRACE_STR(qmFile);
    translator.load(qmFile);   //将刚才发布的.qm文件加载到程序中
    qApp->installTranslator(&translator);
    return true;

//    QStringList environment = QProcess::systemEnvironment();
//    QString str;
//    bool bFinded = false;
//    foreach(str, environment)
//    {
//        if(str.startsWith("QTDIR="))
//        {
//            bFinded = true;
//            break;
//        }
//    }
//    if(bFinded)
//    {
//        str = str.mid(6);
//        bFinded = translator.load("language_" + QLocale::system().name(),str.append("/translations/"));
//        if(!bFinded)
//        {
//            //没有支持中文的Qt国际化翻译文件！
//            translator.load("./IDE.qm");   //将刚才发布的.qm文件加载到程序中
//        }
//        qApp->installTranslator(&translator);
//        return true;
//    }
//    //必须设置 QTDIR 环境变量！
//    return false;
}

Global::Global(QObject *parent) :
    QObject(parent)
{
    //>@自定义信号形参
    qRegisterMetaType<AREA_OPERATE>();
    qRegisterMetaType<TCPSERVICE_TYPE>();
    qRegisterMetaType<PH_Type>();
    qRegisterMetaType<LG_Type>();
    qRegisterMetaType<QProcess::ExitStatus>();


    DoorOpenEnable = false;
#ifdef DISPLAY
    m_DeviceManager = 0;
#endif

#ifdef IDE
    statusBar=0;
    docEditStatus=0;
    keyboardStatus=0;
    searchStatus=0;
    progressStatus=0;
    m_OutputMsgList = 0;

    m_ProjectBar = 0;
    m_DockViewToolBar = 0;
    m_SqlToolBar = 0;
    m_TabPageManager = 0;
#endif

#ifdef SURRPORT_QML
    m_QMLEngine = 0;
#endif

    m_SystemAudio = 0;
}

Global::~Global()
{
#ifdef IDE
    if(m_SqlDatabase.isOpen())
        m_SqlDatabase.close();
#endif
}

#ifdef IDE
void Global::showMessage(QString pShowMsg)
{
    if(docEditStatus)
        docEditStatus->setText(pShowMsg);
}

void Global::outputMessage(QString pShowMsg)
{
    if(pShowMsg.isNull() == false)
    {
        if(m_OutputMsgList)
        {
            m_OutputMsgList->append(pShowMsg);
        }
    }
}

void Global::updateProgress(int pProgress)
{
    if(progressStatus)
    {
        if(pProgress <= 0)
        {
            progressStatus->hide();
            return;
        }
        if(pProgress >= 100)
        {
            progressStatus->hide();
            return;
        }
        progressStatus->setValue(pProgress);
        progressStatus->show();
    }
}
void Global::updateProgress(QString pShowMsg, int pProgress)
{
    outputMessage(pShowMsg);
    updateProgress(pProgress);
}
#endif

bstproManager::bstproManager(QString fileName) : xmlParse(fileName)
{
    IDE_TRACE_STR(m_FileDir);
    IDE_TRACE_STR(m_FileName);
    int tmpProNum, tmpSuffixNum;
    tmpProNum = m_FileName.length();
    tmpSuffixNum = BstProSuffix.length();
    if(tmpProNum <= tmpSuffixNum)
    {
        IDE_TRACE_INT(tmpProNum);
        return;
    }
    QString suffix = m_FileName.mid(tmpProNum-tmpSuffixNum, tmpSuffixNum);
    if(suffix != BstProSuffix)
        m_FileName = QString("%1%2").arg(m_FileName).arg(BstProSuffix);
}

bool bstproManager::NewPro()
{
    bool flag = New();
    if(flag == false)
    {
        IDE_TRACE();
        m_Valid = false;
        return false;
    }
    m_Valid = true;

    appendChild(createProcessingInstruction("xml", XmlHead));

    QDomElement rootElement;
    rootElement = createElement("root");
    appendChild(rootElement);

    QDomElement tmpElement = createElement("FileType");
    tmpElement.appendChild(createTextNode("BST Solution File"));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("CreateTime");
        QDateTime time = QDateTime::currentDateTime();
        tmpElement.appendChild(createTextNode(time.toString("yyyy-MM-dd HH:mm:ss")));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("LastModiTime");
        tmpElement.appendChild(createTextNode(time.toString("yyyy-MM-dd HH:mm:ss")));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("FileList");
    rootElement.appendChild(tmpElement);

    m_IsChanged = true;
    Save();
    return true;
}

bool bstproManager::OpenPro()
{
    bool flag = Open();
    if(flag == false)
    {
       m_Valid = false;
       return false;
    }
    m_Valid = true;

    //>@Check FileType
    flag = GetItemElement("/FileType");
    if(flag == false)
    {
        m_Valid = false;
        return false;
    }
    QString tmpString = itemElement.text();
    if(tmpString.compare(BstProFileType, Qt::CaseInsensitive) != 0)
    {
        m_Valid = false;
        IDE_TRACE_STR(tmpString);
        return false;
    }
    return true;
}

bool bstproManager::ClosePro()
{
    SavePro();
    return true;
}

bool bstproManager::DelPro()
{
    QFile file(m_FileDir + m_FileName);
    if(file.exists())
    {
        file.remove();
        return true;
    }
    return false;
}

bool bstproManager::SavePro()
{
    QDateTime time = QDateTime::currentDateTime();
    ModifyItemElement("/LastModiTime", time.toString("yyyy-MM-dd HH:mm:ss"));
    Save();
    return true;
}

bool bstproManager::RenamePro(QString FileName)
{
    QString tmpString;
    int index = FileName.indexOf(BstProSuffix);
    if(index<0)
    {
        tmpString = FileName + BstProSuffix;
    }
    bool flag = Rename(tmpString);
    if(flag)
    {
        m_FileName = tmpString;     //>@如果重命名成功，则修改m_FileName为重命名后的值。
    }
    return flag;
}

//>@pFileName必须是一个相对地址
bool bstproManager::AddFile(QString pFileName)
{
    return AddFile(getFileSuffix(pFileName), pFileName);
}

//>@pFileName必须是一个相对地址
bool bstproManager::AddFile(QString pKey, QString pFileName)
{
    if(!m_Valid || pKey.isEmpty())
    {
        IDE_TRACE();
        return false;
    }
    if(GetItemElement("/FileList") == false)
    {
        IDE_TRACE();
        return false;
    }
    QString tmpFileName = getFileName(pFileName);
    QString tmpKey = pKey.toUpper();
    QDomElement tmpElement = itemElement.firstChildElement(tmpKey);
    if(tmpElement.isNull() == false)
    {
        return ModifyItemElement(tmpElement, tmpFileName);
    }
    else
    {
        tmpElement = createElement(tmpKey);
        tmpElement.appendChild(createTextNode(tmpFileName));
        itemElement.appendChild(tmpElement);
        m_IsChanged = true;
        return true;
    }
    return true;
}

bool bstproManager::DelFile(QString pKey)
{
    if(pKey.isEmpty())
        return false;
    if(GetItemElement("/FileList") == false)
        return false;
    QString tmpKey = pKey.toUpper();
    QDomElement tmpElement = itemElement.firstChildElement(tmpKey);
    if(tmpElement.isNull())
        return true;
    QDomElement tmpParent = tmpElement.parentNode().toElement();
    if(tmpParent.isNull())
        return false;
    tmpParent.removeChild(tmpElement);
    m_IsChanged = true;
    return true;
}

QString bstproManager::GetFile(QString pKey)
{
    if(pKey.isEmpty())
        return QString();
    if(GetItemElement(QString("/FileList/%1").arg(pKey.toUpper())) == false)
        return QString();
    if(itemElement.isNull())
        return QString();
    return itemElement.text();
}

cfgManager::cfgManager(QString fileName) : xmlParse(fileName)
{
    m_Valid = false;
    QFile file(fileName);
    bool flag = file.exists();
    if(flag == false)
    {
        IDE_TRACE();
        return ;
    }
    flag = Open();
    if(flag == false)
    {
       IDE_TRACE();
       return ;
    }
    //>@Check filetype
    flag = GetItemElement("/FileType");
    if(flag == false)
    {
        IDE_TRACE();
        return;
    }
    QString tmpString = itemElement.text();
    if(tmpString.compare(CfgFileType, Qt::CaseInsensitive) != 0)
    {
        IDE_TRACE_STR(tmpString);
        return;
    }
    m_Valid = true;

    getNewCategory();
    getSolutionCategory();
    getFileCategory();
    getComponetCategory();
    getCfgparaGroup();
    getProtocolInterfaceGroup();
}

bool cfgManager::getNewCategory()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return false;
    }
    bool flag = GetItemElement("/NewFile");
    if(flag == false)
    {
        IDE_TRACE();
        return false;
    }
    GlobalPara.NewCategoryGroup.clear();
    QString tmpString;
    QDomElement tmpElement;
    QDomNode tmpNode;
    QDomNodeList newCategoryNode = GetChildNodes();
    for(int i=0;i < newCategoryNode.count();++i)
    {
        tmpNode = newCategoryNode.at(i);
        tmpElement = tmpNode.toElement();
        tmpString = tmpElement.tagName();
        GlobalPara.NewCategoryGroup.insert(tmpString, tmpElement);
    }
    return true;
}

bool cfgManager::getSolutionCategory()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return false;
    }
    bool flag = GetItemElement("/NewFile/Solution");
    if(flag == false)
    {
        IDE_TRACE();
        return false;
    }

    GlobalPara.SolutionCategoryGroup.clear();

    QString tmpString;
    QDomElement tmpElement;
    QDomNode tmpNode;
    QDomNodeList solutionCategoryNode = itemElement.childNodes();
    for(int i=0;i < solutionCategoryNode.count();++i)
    {
        SolutionCategory tmpSolutionCategory;
        tmpNode = solutionCategoryNode.at(i);
        tmpElement = tmpNode.toElement();
        tmpSolutionCategory.Icon = tmpElement.attribute("img");
        tmpSolutionCategory.Resolution = tmpElement.firstChildElement("Resolution").text();
        tmpString = tmpElement.firstChildElement("Video").text();
        flag = (tmpString.compare(QString("yes"), Qt::CaseInsensitive) == 0)?true:false;
        tmpSolutionCategory.SupportVideo = flag;
        tmpString = tmpElement.firstChildElement("OpenGL").text();
        flag = (tmpString.compare(QString("yes"), Qt::CaseInsensitive) == 0)?true:false;
        tmpSolutionCategory.SupportOpenGL = flag;

        //>@protocol
        tmpString = tmpElement.firstChildElement("Protocol").text();
        tmpSolutionCategory.Protocol = SplitString(tmpString, ";");

        tmpString = tmpElement.firstChildElement("Description").text();
        tmpSolutionCategory.Description.append(tmpString);
        tmpString = tmpElement.tagName();
        GlobalPara.SolutionCategoryGroup.insert(tmpString, tmpSolutionCategory);
    }
    return true;
}

bool cfgManager::getFileCategory()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return false;
    }
    bool flag = GetItemElement("/NewFile/File");
    if(flag == false)
    {
        IDE_TRACE();
        return false;
    }

    GlobalPara.FileCategoryGroup.clear();

    IDE_TRACE();

    QString tmpString;
    QDomElement tmpElement;
    QDomNode tmpNode;
    QDomNodeList fileCategoryNode = itemElement.childNodes();
    for(int i=0;i < fileCategoryNode.count();i++)
    {
        FileCategory tmpFileCategory;
        tmpNode = fileCategoryNode.at(i);
        tmpElement = tmpNode.toElement();
        tmpFileCategory.Icon = tmpElement.attribute("img");
        tmpFileCategory.Suffix = tmpElement.firstChildElement("Suffix").text();
        tmpFileCategory.Description = tmpElement.firstChildElement("Description").text();
        tmpString = tmpElement.tagName();
        GlobalPara.FileCategoryGroup.insert(tmpString, tmpFileCategory);
    }
    return true;
}

//>@获取boot.xml文档中组件的状态、对应的特效以及资源
void cfgManager::getEffectAndRcTypes(QDomElement pElement, ComCategory *pComCategory)
{
    pComCategory->StateMachines.clear();
    pComCategory->Icon = pElement.attribute("img");
    QDomNodeList StateMachineNodes = pElement.childNodes();
    for(int i=0;i < StateMachineNodes.count();++i)
    {
        QDomNode tmpNode = StateMachineNodes.at(i);
        QDomElement tmpElement = tmpNode.toElement();
        if(tmpElement.isNull())
        {
            IDE_TRACE();
            continue;
        }
        if(tmpElement.tagName() == QString("Control"))
            continue;
        STATEMACHINE *tmpState = new STATEMACHINE;
        tmpState->StateName = tmpElement.tagName();
        //IDE_TRACE_STR(tmpState->StateName);
        QDomNodeList EffectNodes = tmpElement.childNodes();
        for(int j=0;j < EffectNodes.count();++j)
        {
            QDomNode tmpEffectNode = EffectNodes.at(j);
            QDomElement tmpEffectElement = tmpEffectNode.toElement();
            if(tmpEffectElement.isNull())
            {
                //IDE_TRACE();
                continue;
            }
            QString tmpEffectName = tmpEffectElement.tagName();
            if(tmpEffectName.isEmpty() || tmpEffectName.compare(D_RCINVALID, Qt::CaseInsensitive)==0)
            {
                //IDE_TRACE();
                continue;
            }
            EFFECT *tmpEffect = new EFFECT;
            tmpEffect->EffectName = tmpEffectName;
            //>@查看是否有Parameter属性
            QString tmpEffectPara = tmpEffectElement.attribute("Parameter");
            if(tmpEffectPara.isEmpty() == false)
            {
                QStringList tmpParaList = tmpEffectPara.split(';');
                for(int m=0;m<tmpParaList.count();m++)
                {
                    QStringList tmpSubParaList = tmpParaList.at(m).split(':');
                    if(tmpSubParaList.count()!=2)
                        continue;
                    EFFECTPARA *tmpEffectPara = new EFFECTPARA;
                    tmpEffectPara->ParaName = tmpSubParaList.at(0);
                    tmpEffectPara->ParaValue = tmpSubParaList.at(1);
                    tmpEffect->EffectParas.append(tmpEffectPara);
                }
            }
            QDomNodeList RcNodes = tmpEffectElement.childNodes();
            for(int k=0;k < RcNodes.count();++k)
            {
                QDomNode tmpRcNode = RcNodes.at(k);
                QDomElement tmpRcElement = tmpRcNode.toElement();
                if(tmpRcElement.isNull())
                {
                    //IDE_TRACE();
                    continue;
                }
                RESOURCE *tmpResource = new RESOURCE;
                //IDE_TRACE_STR(tmpResource.RcName);
                tmpResource->NickNames.append(tmpRcElement.tagName());
                tmpResource->NickNames.append(tmpRcElement.attribute("nickname").split('/'));
                tmpResource->RcTypes = tmpRcElement.attribute("types").split(';');
                tmpEffect->Resources.append(tmpResource);
            }
            tmpState->Effects.append(tmpEffect);
        }
        pComCategory->StateMachines.append(tmpState);
        IDE_TRACE_STR(tmpState->StateName);
    }
}

//>@获取Boot.xml文件中的Component，并使用/FuncInfo/Floor的格式进行存储。
bool cfgManager::getComponetCategory()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return false;
    }
    bool flag = GetItemElement("/Componet");
    if(flag == false)
    {
        IDE_TRACE();
        return false;
    }
    GlobalPara.m_ComCategoryGroup.clear();

    COM_TYPE tmpComType;
    QDomElement tmpElement = itemElement.firstChildElement("Desktop");
    if(tmpElement.isNull())
    {
        IDE_TRACE();
        return false;
    }
    ComCategory *uComCategory = new ComCategory;
    uComCategory->ParentType = COM_INVALID;
    uComCategory->CurType = COM_DESKTOP;
    getEffectAndRcTypes(tmpElement, uComCategory);
    GlobalPara.m_ComCategoryGroup.insert(QString("/"), uComCategory);

    tmpElement = itemElement.firstChildElement("SubUI");
    if(tmpElement.isNull())
    {
        IDE_TRACE();
        return false;
    }
    QDomNodeList comCategoryNode = tmpElement.childNodes();
    for(int i=0;i < comCategoryNode.count();++i)
    {
        QDomNode tmpNode = comCategoryNode.at(i);
        QDomElement tmpSubElement = tmpNode.toElement();
        if(tmpSubElement.isNull())
        {
            IDE_TRACE();
            continue;
        }
        ComCategory *tmpComCategory = new ComCategory;
        tmpComCategory->ParentType = COM_DESKTOP;
        QString tmpString = tmpSubElement.tagName();
        tmpComType = getComponetType(tmpString);
        tmpComCategory->CurType = tmpComType;
        QString tmpNameString = QString("/%1").arg(tmpString);
        IDE_TRACE_STR(tmpNameString);
        getEffectAndRcTypes(tmpSubElement, tmpComCategory);
        GlobalPara.m_ComCategoryGroup.insert(tmpNameString, tmpComCategory);
        QDomElement tmptmpElement = tmpSubElement.firstChildElement(QString("Control"));
        if(tmptmpElement.isNull())
        {
            IDE_TRACE();
            continue;
        }
        QDomNodeList subComCategoryNode = tmptmpElement.childNodes();
        for(int j=0;j<subComCategoryNode.count();j++)
        {
            QDomNode tmpChildNode = subComCategoryNode.at(j);
            QDomElement tmpChildElement = tmpChildNode.toElement();
            if(tmpChildElement.isNull())
            {
                IDE_TRACE();
                continue;
            }
            ComCategory *tmpSubComCategory = new ComCategory;
            tmpSubComCategory->ParentType = tmpComType;
            QString tmpChildString = tmpChildElement.tagName();
            tmpSubComCategory->CurType = getComponetType(tmpChildString);
            QString tmpChildNameString = QString("%1/%2").arg(tmpNameString).arg(tmpChildString);
            IDE_TRACE_STR(tmpChildNameString);
            getEffectAndRcTypes(tmpChildElement, tmpSubComCategory);
            GlobalPara.m_ComCategoryGroup.insert(tmpChildNameString, tmpSubComCategory);
        }
    }
    return true;
}

bool cfgManager::getCfgparaGroup()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return false;
    }
    bool flag = GetItemElement("/CfgPara");
    if(flag)
    {
        GlobalPara.m_ConfigInfoGroup.clear();
        QDomElement tmpElement;
        QDomNodeList CfgItemList = itemElement.childNodes();  //>@查找Physical、Logic
        for(int i=0;i<CfgItemList.count();i++)
        {
            tmpElement = CfgItemList.at(i).toElement();
            if(tmpElement.isNull())
                continue;
            QString tmpString = tmpElement.tagName();
            if(tmpString.isEmpty())
                continue;
            QString tmpPath = QString("/%1").arg(tmpString);
            GlobalPara.m_ConfigInfoGroup.insert(tmpPath, tmpElement);
            IDE_TRACE_STR(tmpPath);
            QString tmpSubCfgString = QString("/CfgPara/%1").arg(tmpString);
            flag = GetItemElement(tmpSubCfgString);
            if(!flag)
                continue;
            QDomNodeList SubCfgItemNodeList = itemElement.childNodes();   //>@查找CPU或者NET等
            for(int j=0;j<SubCfgItemNodeList.count();j++)              //>@插入配置页的子项元素
            {
                tmpElement = SubCfgItemNodeList.at(j).toElement();
                if(tmpElement.isNull())
                    continue;
                QString tmpSubString = tmpElement.tagName();
                if(tmpSubString.isEmpty())
                    continue;
                QString tmpSubPath = QString("/%1/%2").arg(tmpString).arg(tmpSubString);
                GlobalPara.m_ConfigInfoGroup.insert(tmpSubPath, tmpElement);
                IDE_TRACE_STR(tmpSubPath);
                if(tmpString.compare("Physical")==0)
                {
                    GlobalPara.m_PHonfigInfoGroup.insert(GetPHType(tmpSubString), tmpElement);
                }
                else if(tmpString.compare("Logic")==0)
                {
                    GlobalPara.m_LGonfigInfoGroup.insert(GetLGType(tmpSubString), tmpElement);
                }
            }
        }
    }
    return true;
}

bool cfgManager::getProtocolInterfaceGroup()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return false;
    }
    bool flag = GetItemElement("/ProtocolInterface");
    if(flag)
    {
        GlobalPara.m_ProtocolInterfaceGroup.clear();
        QDomElement tmpElement;
        QDomNodeList CfgItemList = itemElement.childNodes();
        for(int i=0;i<CfgItemList.count();i++)
        {
            tmpElement = CfgItemList.at(i).toElement();
            if(tmpElement.isNull())
                continue;
            QString tmpString = tmpElement.tagName();
            if(tmpString.isEmpty())
                continue;
            GlobalPara.m_ConfigInfoGroup.insert(tmpString, tmpElement);
            IDE_TRACE_STR(tmpString);
        }
    }
    return true;
}

uiManager::uiManager(QString fileName) :
    xmlParse(fileName)
{
    int tmpProNum, tmpSuffixNum;
    tmpProNum = m_FileName.length();
    tmpSuffixNum = UiSuffix.length();
    if(tmpProNum <= tmpSuffixNum)
    {
        IDE_TRACE_INT(tmpProNum);
        return;
    }
    QString suffix = m_FileName.mid(tmpProNum-tmpSuffixNum, tmpSuffixNum);
    if(suffix != UiSuffix)
        m_FileName = QString("%1%2").arg(m_FileName).arg(UiSuffix);
}

bool uiManager::NewUI()
{
    bool flag = New();
    if(flag == false)
    {
        IDE_TRACE();
        m_Valid = false;
        return false;
    }
    m_Valid = true;
    m_ComElementGroup.clear();

    appendChild(createProcessingInstruction("xml", XmlHead));

    QDomElement rootElement;
    rootElement = createElement("root");
    appendChild(rootElement);

    QDomElement tmpElement = createElement("FileType");
        tmpElement.appendChild(createTextNode("BST UI File"));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("CreateTime");
        QDateTime time = QDateTime::currentDateTime();
        tmpElement.appendChild(createTextNode(time.toString("yyyy-MM-dd HH:mm:ss")));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("LastModiTime");
        tmpElement.appendChild(createTextNode(time.toString("yyyy-MM-dd HH:mm:ss")));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("Solution");
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("Direction");
        tmpElement.appendChild(createTextNode("HV"));
        m_DirectionPermisson = HVBoth;
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("Ui");
        QDomElement tmpChildElement = createElement("SubUI");
        tmpElement.appendChild(tmpChildElement);
    rootElement.appendChild(tmpElement);

    m_IsChanged = true;
    Save();

    GetComponentGroup();
    return true;
}

bool uiManager::OpenUI()
{
    bool flag = Open();
    if(flag == false)
    {
       m_Valid = false;
       return false;
    }
    m_Valid = true;
    //>@Check FileType
    flag = GetItemElement("/FileType");
    if(flag == false)
    {
        m_Valid = false;
        return false;
    }
    QString tmpString = itemElement.text();
    if(tmpString.compare(UiFileType, Qt::CaseInsensitive) != 0)
    {
        m_Valid = false;
        IDE_TRACE_STR(tmpString);
        return false;
    }
    m_DirectionPermisson = GetDirectionPermisson();
    GetComponentGroup();
    return true;
}

bool uiManager::CloseUI()
{
    SaveUI();
    return true;
}

bool uiManager::DelUI()
{
    QFile file(m_FileDir + m_FileName);
    if(file.exists())
    {
        file.remove();
        return true;
    }
    return false;
}

bool uiManager::SaveUI()
{
    QDateTime time = QDateTime::currentDateTime();
    ModifyItemElement("/LastModiTime", time.toString("yyyy-MM-dd HH:mm:ss"));
    Save();
    return true;
}

bool uiManager::RenameUI(QString FileName)
{
    QString tmpString;
    int index = FileName.indexOf(UiSuffix);
    if(index<0)
    {
        tmpString = FileName + UiSuffix;
    }
    bool flag = Rename(tmpString);
    if(flag)
        m_FileName = tmpString;     //>@如果重命名成功，则修改m_FileName为重命名后的值。
    return flag;
}

bool uiManager::isValid()
{
    if(!m_Valid)
        return false;
    if(m_FileDir.endsWith('/') == false)
        m_FileDir.append('/');
    QDir tmpDir(m_FileDir);
    if(tmpDir.exists() == false)
        return tmpDir.mkpath(m_FileDir);
    return true;
}

QString uiManager::GetSolution()
{
    return GetItemElementValue("/Solution");
}

bool uiManager::SetSolution(QString pSolution)
{
    return ModifyItemElement("/Solution", pSolution);
}

DIRECTION uiManager::GetDirectionPermisson()
{
    return GetDirection(GetItemElementValue("/Direction"));
}

bool uiManager::SetDirectionPermisson(DIRECTION pDirection)
{
    if(!m_Valid)
        return false;
    QString tmpDirString = GetDirectionString(pDirection);
    if(tmpDirString.isEmpty())
        return false;
    m_DirectionPermisson = pDirection;
    return ModifyItemElement("/Direction", tmpDirString);
}

//>@用于更新Global类中的m_ComElementGroup
void uiManager::GetComponentGroup()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return;
    }
    bool flag = GetItemElement("/Ui/Desktop");
    if(flag)
    {
        m_ComElementGroup.clear();
        m_ComElementGroup.insert(COM_DESKTOP, itemElement);//>@Desktop
    }
    else
    {
        IDE_TRACE();
        return;
    }

    flag = GetItemElement("/Ui/SubUI");
    if(flag)
    {
        QDomElement tmpElement;
        QDomNodeList SubuiNodeList = itemElement.childNodes();
        for(int i=0;i<SubuiNodeList.count();i++)
        {
            tmpElement = SubuiNodeList.at(i).toElement();
            QString tmpString = tmpElement.tagName();
            COM_TYPE tmpComType = getComponetType(tmpString);
            if(tmpComType < COM_DESKTOP)
                continue;
            m_ComElementGroup.insert(tmpComType, tmpElement);//>@插入subui元素
            //IDE_TRACE_STR(tmpSubuiString);
            QString tmpControlString = QString("/Ui/SubUI/%1/Control").arg(tmpString);
            flag = GetItemElement(tmpControlString);
            if(flag)
            {
                QDomNodeList ControlNodeList = itemElement.childNodes();
                for(int i=0;i<ControlNodeList.count();i++)              //>@插入subui的子control元素
                {
                    tmpElement = ControlNodeList.at(i).toElement();
                    QString tmpSubString = tmpElement.tagName();
                    COM_TYPE tmpSubComType = getComponetType(tmpSubString);
                    if(tmpSubComType < COM_DESKTOP)
                        continue;
                    m_ComElementGroup.insert(tmpSubComType, tmpElement);
                    //IDE_TRACE_STR(tmpSubControlString);
                }
            }
        }
    }
}

QRect uiManager::GetComRect(QDomElement pComElement, DIRECTION pDirection)
{
    if(pComElement.isNull())
        return QRect();
    QDomElement tmpRectElement = pComElement.firstChildElement("Rect");
    if(tmpRectElement.isNull())
        return QRect();
    QDomElement tmpElement;
    if(pDirection == Horizontal)
        tmpElement = tmpRectElement.firstChildElement("Horizontal");
    else
        tmpElement = tmpRectElement.firstChildElement("Vertical");
    if(tmpElement.isNull())
        return QRect();
    return GetRectFromString(tmpElement.text());
}

bool uiManager::ModifyComRect(QDomElement pComElement, QRect pRect, DIRECTION pDirection)
{
    if(pComElement.isNull() || pRect.isNull())
        return false;
    COM_TYPE tmpComType = getComponetType(pComElement.tagName());
    if(tmpComType < COM_DESKTOP)
        return false;
    QDomElement tmpRectElement = pComElement.firstChildElement("Rect");
    if(tmpRectElement.isNull())
    {
        //>@不存在，则创建
        tmpRectElement = createElement("Rect");
        if(tmpRectElement.isNull())
            return false;
        pComElement.appendChild(tmpRectElement);
    }
    //>@
    if(pRect.left()<0 || pRect.top()<0 || pRect.width()<=0 || pRect.height()<=0)
    {
        IDE_DEBUG(QString("ui COM rect is invalid!"));
        return false;
    }
    //>@查看横竖坐标是否存在，不存在则创建
    //>@在同步修改另一种类型坐标时，只修改宽高度，不修改左上角坐标；
    //>@并且如果是新插入坐标，则把左上角坐标设为0；如果是修改坐标，则保持左上角坐标不变
    QDomElement tmpHElement = tmpRectElement.firstChildElement("Horizontal");
    QDomElement tmpVElement = tmpRectElement.firstChildElement("Vertical");
    if(tmpHElement.isNull())
    {
        tmpHElement = createElement("Horizontal");
        tmpRectElement.appendChild(tmpHElement);
    }
    if(tmpVElement.isNull())
    {
        tmpVElement = createElement("Vertical");
        tmpRectElement.appendChild(tmpVElement);
    }
    QString tmpHOldString = tmpHElement.text();
    QString tmpVOldString = tmpVElement.text();
    QString tmpHString, tmpVString;
    if(pDirection == Horizontal)
    {
        tmpHString = QString("%1,%2,%3,%4").arg(pRect.left()).arg(pRect.top()).arg(pRect.width()).arg(pRect.height());
        if(!ModifyItemElement(tmpHElement, tmpHString))
        {
            IDE_TRACE_STR(tmpHString);
            return false;
        }
        if(canSwitch())
        {
            QRect tmpRect = GetRectFromString(tmpVOldString);
            if(tmpComType == COM_DESKTOP)
            {
                if(tmpRect.isNull())
                    tmpVString = QString("0,0,%1,%2").arg(pRect.height()).arg(pRect.width());
                else
                    tmpVString = QString("%1,%2,%3,%4").arg(tmpRect.left()).arg(tmpRect.top()).arg(pRect.height()).arg(pRect.width());
            }
            else
            {
                if(tmpRect.isNull())
                    tmpVString = QString("0,0,%1,%2").arg(pRect.width()).arg(pRect.height());
                else
                    tmpVString = QString("%1,%2,%3,%4").arg(tmpRect.left()).arg(tmpRect.top()).arg(pRect.width()).arg(pRect.height());
            }
            if(!ModifyItemElement(tmpVElement, tmpVString))
            {
                IDE_TRACE_STR(tmpVString);
                ModifyItemElement(tmpHElement, tmpHOldString);
                return false;
            }
        }
    }
    else if(pDirection == Vertical)
    {
        tmpVString = QString("%1,%2,%3,%4").arg(pRect.left()).arg(pRect.top()).arg(pRect.width()).arg(pRect.height());
        if(!ModifyItemElement(tmpVElement, tmpVString))
        {
            IDE_TRACE_STR(tmpVString);
            return false;
        }
        if(canSwitch())
        {
            QRect tmpRect = GetRectFromString(tmpHOldString);
            if(tmpComType == COM_DESKTOP)
            {
                if(tmpRect.isNull())
                    tmpHString = QString("0,0,%1,%2").arg(pRect.height()).arg(pRect.width());
                else
                    tmpHString = QString("%1,%2,%3,%4").arg(tmpRect.left()).arg(tmpRect.top()).arg(pRect.height()).arg(pRect.width());
            }
            else
            {
                if(tmpRect.isNull())
                    tmpHString = QString("0,0,%1,%2").arg(pRect.width()).arg(pRect.height());
                else
                    tmpHString = QString("%1,%2,%3,%4").arg(tmpRect.left()).arg(tmpRect.top()).arg(pRect.width()).arg(pRect.height());
            }
            if(!ModifyItemElement(tmpHElement, tmpHString))
            {
                IDE_TRACE_STR(tmpHString);
                ModifyItemElement(tmpVElement, tmpVOldString);
                return false;
            }
        }
    }
    return true;
}

QDomNodeList uiManager::GetComStateList(QDomElement pComElement)
{
    if(pComElement.isNull())
        return QDomNodeList();
    QDomElement tmpSMElement = pComElement.firstChildElement("StateMachine");
    if(tmpSMElement.isNull())
        return QDomNodeList();
    return tmpSMElement.childNodes();
}

QDomElement uiManager::GetComState(QDomElement pComElement, QString pStateName)
{
    if(pComElement.isNull() || pStateName.isEmpty())
        return QDomElement();
    QDomElement tmpSMElement = pComElement.firstChildElement("StateMachine");
    if(tmpSMElement.isNull())
        return QDomElement();
    return tmpSMElement.firstChildElement(pStateName);
}

bool uiManager::ModifyEffectType(QDomElement pStateElement, QString pEffectType)
{
    if(pStateElement.isNull())
        return false;
    //>@不理会空的特效类型
    if(!pEffectType.isEmpty())
    {
        QString tmpEffectType = pStateElement.attribute(D_EFFECTTYPE);
        //>@如果待设置的特效类型不一样，则需要删除UI中的所有子节点
        if(pEffectType.compare(tmpEffectType))
        {
            ClearItemElement(pStateElement);
            pStateElement.setAttribute(D_EFFECTTYPE, pEffectType);
            pStateElement.removeAttribute(D_EFFECTPARA);
            return true;
        }
    }
    return false;
}

QDomElement uiManager::ModifyComState(QDomElement pComElement, QString pStateName, QString pEffectType, QString pEffectPara)
{
    if(pComElement.isNull() || pStateName.isEmpty())
        return QDomElement();
    QDomElement tmpSMElement = pComElement.firstChildElement("StateMachine");
    if(tmpSMElement.isNull())
    {
        //>@不存在，则创建
        tmpSMElement = createElement("StateMachine");
        if(tmpSMElement.isNull())
            return QDomElement();
        pComElement.appendChild(tmpSMElement);
    }
    QDomElement tmpStateElement = tmpSMElement.firstChildElement(pStateName);
    if(tmpStateElement.isNull())
    {
        //>@不存在，则创建
        tmpStateElement = createElement(pStateName);
        if(tmpStateElement.isNull())
            return QDomElement();
        tmpSMElement.appendChild(tmpStateElement);
    }
    //>@修改特效名，如果修改成功，则更新参数。
    //>@注意，如果待修改的特效名相同，则认为修改不成功
    if(ModifyEffectType(tmpStateElement, pEffectType))
    {
        QString tmpEffectPara = tmpStateElement.attribute(D_EFFECTPARA);
        if(tmpEffectPara.compare(pEffectPara))
        {
            tmpStateElement.setAttribute(D_EFFECTPARA, pEffectPara);
        }
    }
    return tmpStateElement;
}

QDomElement uiManager::GetParComDom(QString pComPath)
{
    COM_TYPE tmpType = getParentComType(getCurComType(pComPath));
    return m_ComElementGroup.value(tmpType);
}

QDomElement uiManager::GetParComDom(QDomElement pComElement)
{
    QDomElement tmpElement = pComElement.parentNode().toElement();
    if(tmpElement.isNull())
        return QDomElement();
    QString tmpName = tmpElement.tagName();
    if(tmpName.compare("Ui") == 0)  //>@为桌面组件
    {
        return QDomElement();
    }
    else if(tmpName.compare("Control") == 0)  //>@为其它组件
    {
        return tmpElement.parentNode().toElement();
    }
    else if(tmpName.compare("SubUI") == 0)  //>@为区域组件
    {
        return tmpElement.parentNode().firstChildElement("Desktop");
    }
    return QDomElement();
}

FILEPATH_TYPE uiManager::GetRcFileType(QString pFileName)
{
    FILEPATH_TYPE tmpType = getRcFileType(pFileName);
    if(PATH_TYPENONE != tmpType)
        return tmpType;
    if(pFileName.isEmpty())
        return PATH_TYPENONE;
    //>@4、查看是否为当前UI下的文件
    QString tmpString = m_FileDir + pFileName;
    if(isFileExist(tmpString))
    {
        return PATH_TYPERC;
    }
    return PATH_TYPENONE;
}

FILEPATH_TYPE uiManager::RegulatRcFile(QString& pFileName)
{
    FILEPATH_TYPE tmpType = GetRcFileType(pFileName);
    if(tmpType == PATH_TYPERC)
    {
        pFileName.insert(0, m_FileDir);
    }
    return tmpType;
}

bool uiManager::IsRelativePath(QString pFileName)
{
    if(pFileName.isEmpty())
        return false;
    QString tmpString = m_FileDir + pFileName;
    if(isFileExist(tmpString))
    {
        return true;
    }
    return false;
}

bool uiManager::GetParentElement(QDomElement &pParentElement, QString pComPath)
{
    //>@规整父节点路径
    COM_TYPE tmpType = getCurComType(pComPath);
    if(tmpType == COM_DESKTOP)
    {
        if(!GetItemElement(QString("/Ui")))
        {
            if(!CreateItemElement(QString("/Ui")))
                return false;
        }
    }
    else if(tmpType > COM_AREA && tmpType < COM_COMPONENT)
    {
        //>@如果没有桌面，则认为没有父
        QDomElement tmpElement = m_ComElementGroup.value(COM_DESKTOP);
        if(tmpElement.isNull())
            return false;
        if(!GetItemElement(QString("/Ui/SubUI")))
        {
            if(!CreateItemElement(QString("/Ui/SubUI")))
                return false;
        }
    }
    else if(tmpType > COM_COMPONENT)
    {
        COM_TYPE tmpParType = getParentComType(tmpType);
        if(tmpParType < COM_AREA)
            return false;
        //>@如果没有父控件，则返回假
        QDomElement parentElement = m_ComElementGroup.value(tmpParType);
        if(parentElement.isNull())
            return false;
        QString tmpComString = QString("/Ui/SubUI/%1/Control").arg(parentElement.tagName());
        if(GetItemElement(tmpComString) == false)
        {
            if(!CreateItemElement(tmpComString))
                return false;
        }
    }
    else
    {
        return false;
    }
    pParentElement = itemElement;
    return true;
}

QDomElement uiManager::AddComDom(COM_TYPE pComType, QRect pRect, DIRECTION pDirection)
{
    if(pComType < COM_DESKTOP)
        return QDomElement();
    //>@如果已经有此控件，返回真，不再重复创建
    QDomElement tmpComElement = m_ComElementGroup.value(pComType);
    if(!tmpComElement.isNull())
        return tmpComElement;
    //>@
    if(pRect.isNull())
        return QDomElement();
    //>@如果没有父控件，则退出，但是对于Desktop需要单独处理
    if(pComType > COM_DESKTOP)
    {
        QDomElement tmpParElement = m_ComElementGroup.value(getParentComType(pComType));
        if(tmpParElement.isNull())
            return QDomElement();
    }
    //>@
    QDomElement tmpParent;
    if(!GetParentElement(tmpParent, getComponetPath(pComType)))
        return QDomElement();
    QString tmpComName = getComponetName(pComType);
    tmpComElement = createElement(tmpComName);
    if(tmpComElement.isNull())
        return QDomElement();
    //>@添加Rect
    if(!ModifyComRect(tmpComElement, pRect, pDirection))
        return QDomElement();
    tmpParent.appendChild(tmpComElement);
    m_IsChanged = true;
    m_ComElementGroup.insert(pComType, tmpComElement);
    return tmpComElement;
}

bool uiManager::DeleteComDom(COM_TYPE pComType)
{
    QDomElement tmpElement = m_ComElementGroup.take(pComType);
    if(tmpElement.isNull())
        return true;
    return DeleteItemElement(tmpElement);
}

bool uiManager::SaveRelativeComDom(QDomElement pComElement)
{
    if(!isValid())
        return false;
    QString tmpRelativePath = m_FileDir;
    COM_TYPE tmpType = getComponetType(pComElement.tagName());
    QString tmpComPath = getComponetPath(tmpType);
    QDomNodeList tmpStateList = GetComStateList(pComElement);
    for(int i=0;i<tmpStateList.count();i++)
    {
        QDomElement tmpStateElement = tmpStateList.at(i).toElement();
        if(tmpStateElement.isNull())
            continue;
        QString tmpStateName = tmpStateElement.tagName();
        QString tmpEffectName = tmpStateElement.attribute(D_EFFECTTYPE);
        QDomNodeList tmpRcList = tmpStateElement.childNodes();
        for(int j=0;j<tmpRcList.count();j++)
        {
            QDomElement tmpRcElement = tmpRcList.at(j).toElement();
            if(tmpRcElement.isNull())
                continue;
            QString tmpRcName = tmpRcElement.tagName();
            QString tmpRcFile = tmpRcElement.text();
            if(tmpRcName.compare(QString(D_RCINVALID)) && !IsRelativePath(tmpRcFile)) //针对于不需要资源的特效以及已经变换为相对路径的资源
            {
                QString tmpString;
                bool needCopy = true;
                if(tmpType == COM_DESKTOP)
                {
                    //>@如果是Desktop
                    tmpString = QString("Desktop") + QString("/") +
                                       tmpStateName + QString("/") +
                                       tmpEffectName + QString("/") +
                                       getFileName(tmpRcFile);
                }
                else if(tmpType == COM_QML || tmpType == COM_OPENGL) //>@
                {
                    FILEPATH_TYPE tmpType = GetRcFileType(tmpRcFile);
                    if(tmpType != PATH_TYPEABS)
                    {
                        IDE_TRACE_STR(tmpRcFile);
                        continue;
                    }
                    tmpString = tmpComPath.mid(1) + QString("/") +
                                       tmpStateName + QString("/") +
                                       tmpEffectName + QString("/") +
                                       getFileName(tmpRcFile);
                    QString tmpDst = tmpRelativePath + tmpString;
                    QString tmpSrc = tmpRcFile;  //>@
                    int index1 = tmpSrc.lastIndexOf('.');
                    if(index1 < 0)
                    {
                        IDE_TRACE_STR(tmpSrc);
                        continue;
                    }
                    QString tmpSrcDir = tmpSrc.mid(0, index1);
                    int index2 = tmpDst.lastIndexOf('.');
                    if(index2 < 0)
                    {
                        IDE_TRACE_STR(tmpDst);
                        continue;
                    }
                    QString tmpDstDir = tmpDst.mid(0, index2);
                    if(CopyFolder(tmpSrcDir, tmpDstDir, true) == false)
                    {
                        IDE_TRACE_STR(tmpSrcDir);
                        continue;
                    }
                }
                else
                {
                    FILEPATH_TYPE tmpType = GetRcFileType(tmpRcFile);
                    if(tmpType == PATH_TYPENONE)
                    {
                        continue;
                    }
                    else if(tmpType == PATH_TYPENETADDR)
                    {
                        needCopy = false;
                        tmpString = tmpRcFile;
                    }
                    else
                    {
                        tmpString = tmpComPath.mid(1) + QString("/") +
                                       tmpStateName + QString("/") +
                                       tmpEffectName + QString("/") +
                                       getFileName(tmpRcFile);
                    }
                }
                if(needCopy)
                {
                    QString tmpDst = tmpRelativePath + tmpString;
                    if(!CopyFile(tmpRcFile, tmpDst))
                    {
                        IDE_TRACE_STR(tmpRcFile);
                        continue;
                    }
                }
                ModifyItemElement(tmpRcElement, tmpString);
            }
        }
    }
    return true;
}

bool uiManager::RegularFile(COM_TYPE pComType, QString pRcName, QString &pRcFile, QString pDstPath, QSize pSize)
{
    if(pRcName == QString(D_RCINVALID)) //针对于不需要资源的特效
    {
        return true;
    }
    else if(pComType >= COM_DESKTOP)
    {
        if(pDstPath.endsWith('/') == false)
            pDstPath.append('/');
        QString tmpSrc = m_FileDir + pRcFile;
        //>@根据文件类型规整资源
        RC_TYPE tmpRcType = getRcType(pRcFile);
        switch(tmpRcType)
        {
            case RC_BMP:
            case RC_JPG:
            case RC_PNG:
            {
                //>@修改目标文件名为RcName，后缀为png
                int index = pRcFile.lastIndexOf(".");
                if(index < 0)
                {
                    pRcFile = pRcName + QString(".png");
                }
                else
                {
                    QString tmpString = pRcFile.mid(0, index+1);
                    pRcFile = tmpString + QString("png");
                }
                QString tmpDst = pDstPath + pRcFile;
                if(ConvertImage(tmpSrc, tmpDst, pSize) == false)
                {
                    IDE_TRACE_STR(tmpSrc);
                    return false;
                }
                break;
            }
            case RC_GIF:
            {
                int index = pRcFile.lastIndexOf(".");
                if(index < 0)
                {
                    pRcFile = pRcName + QString(".gif");
                }
                else
                {
                    QString tmpString = pRcFile.mid(0, index+1);
                    pRcFile = tmpString + QString("gif");
                }
                QString tmpDst = pDstPath + pRcFile;
                if(CopyFile(tmpSrc, tmpDst) == false)
                {
                    IDE_TRACE_STR(tmpSrc);
                    return false;
                }
                break;
            }
            case RC_SVG:
            {
                //>@固定图像资源以svg文件名命名
                int index = pRcFile.lastIndexOf(".");
                if(index < 0)
                {
                    pRcFile = pRcName + QString(".svg");
                }
                else
                {
                    QString tmpString = pRcFile.mid(0, index+1);
                    pRcFile = tmpString + QString("svg");
                }
                QString tmpDst = pDstPath + pRcFile;
                if(CopyFile(tmpSrc, tmpDst) == false)
                {
                    IDE_TRACE_STR(tmpSrc);
                    return false;
                }
                break;
            }
            case RC_QML:
            {
                //>@转换QML文件以及所使用的图像资源，固定图像资源以QML文件名命名
                QString tmpDst = pDstPath + pRcFile;
                if(CopyFile(tmpSrc, tmpDst) == false)
                {
                    IDE_TRACE_STR(tmpSrc);
                    return false;
                }
                int index1 = tmpSrc.lastIndexOf('.');
                if(index1 < 0)
                {
                    IDE_TRACE_STR(tmpSrc);
                    return false;
                }
                QString tmpSrcDir = tmpSrc.mid(0, index1);
                int index2 = tmpDst.lastIndexOf('.');
                if(index2 < 0)
                {
                    IDE_TRACE_STR(tmpDst);
                    return false;
                }
                QString tmpDstDir = tmpDst.mid(0, index2);
                if(CopyFolder(tmpSrcDir, tmpDstDir, true) == false)
                {
                    IDE_TRACE_STR(tmpSrcDir);
                    return false;
                }
                break;
            }
            case RC_FONT:
            {
                int index = pRcFile.lastIndexOf("/");
                if(index < 0)
                {
                    pRcFile = pRcName + QString(".font");
                }
                else
                {
                    QString tmpString = pRcFile.mid(0, index+1);
                    pRcFile = tmpString + pRcName + QString(".font");
                }
                QString tmpDst = pDstPath + pRcFile;
                if(CopyFile(tmpSrc, tmpDst) == false)
                {
                    IDE_TRACE_STR(tmpSrc);
                    return false;
                }
                break;
            }
            case RC_MP3:
            case RC_WAV:
            {
                //>@只转换相对和绝对路径下的文件，QRC以及网络文件都不可以转换
                FILEPATH_TYPE tmpType = GetRcFileType(pRcFile);
                if(tmpType == PATH_TYPERC || tmpType == PATH_TYPEABS)
                {
                    //>@修改目标文件名为RcName，后缀为wav
                    int index = pRcFile.lastIndexOf("/");
                    if(index < 0)
                    {
                        pRcFile = pRcName + QString(".wav");
                    }
                    else
                    {
                        QString tmpString = pRcFile.mid(0, index+1);
                        pRcFile = tmpString + pRcName + QString(".wav");
                    }
                    QString tmpDst = pDstPath + pRcFile;
                    if(ConvertAudio(tmpSrc, tmpDst) == false)
                    {
                        IDE_TRACE_STR(tmpSrc);
                        return false;
                    }
                }
                break;
            }
            case RC_MP4:
            case RC_AVI:
            case RC_RMVB:
            case RC_FLV:
            {
                //>@只转换相对和绝对路径下的文件，QRC以及网络文件都不可以转换
                FILEPATH_TYPE tmpType = GetRcFileType(pRcFile);
                if(tmpType == PATH_TYPERC || tmpType == PATH_TYPEABS)
                {
                    //>@修改目标文件名为RcName，后缀为mp4
                    int index = pRcFile.lastIndexOf("/");
                    if(index < 0)
                    {
                        pRcFile = pRcName + QString(".mp4");
                    }
                    else
                    {
                        QString tmpString = pRcFile.mid(0, index+1);
                        pRcFile = tmpString + pRcName + QString(".mp4");
                    }
                    QString tmpDst = pDstPath + pRcFile;
                    if(ConvertVideo(tmpSrc, tmpDst, pSize) == false)
                    {
                        IDE_TRACE_STR(tmpSrc);
                        return false;
                    }
                }
                break;
            }
            default:break;  //>@对于QRC以及网址等文件不进行转换
        }
        return true;
    }
    return false;
}

mdpManager::mdpManager(QString fileName) : xmlParse(fileName)
{
    int tmpProNum, tmpSuffixNum;
    tmpProNum = m_FileName.length();
    tmpSuffixNum = MdpSuffix.length();
    if(tmpProNum <= tmpSuffixNum)
    {
        IDE_TRACE_INT(tmpProNum);
        return;
    }
    QString suffix = m_FileName.mid(tmpProNum-tmpSuffixNum, tmpSuffixNum);
    if(suffix != MdpSuffix)
        m_FileName = QString("%1%2").arg(m_FileName).arg(MdpSuffix);
}

bool mdpManager::NewMDP()
{
    bool flag = New();
    if(flag == false)
    {
        IDE_TRACE();
        m_Valid = false;
        return false;
    }
    m_Valid = true;

    appendChild(createProcessingInstruction("xml", XmlHead));

    QDomElement rootElement;
    rootElement = createElement("root");
    appendChild(rootElement);

    QDomElement tmpElement = createElement("FileType");
        tmpElement.appendChild(createTextNode("BST MDP File"));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("CreateTime");
        QDateTime time = QDateTime::currentDateTime();
        tmpElement.appendChild(createTextNode(time.toString("yyyy-MM-dd HH:mm:ss")));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("LastModiTime");
        tmpElement.appendChild(createTextNode(time.toString("yyyy-MM-dd HH:mm:ss")));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("NickName");
    rootElement.appendChild(tmpElement);
    tmpElement = createElement("Passwd");
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("CfgPara");
        tmpElement.appendChild(createElement("Physical"));
        tmpElement.appendChild(createElement("Logic"));
    rootElement.appendChild(tmpElement);

    m_IsChanged = true;
    Save();
    return true;
}

bool mdpManager::OpenMDP()
{
    bool flag = Open();
    if(flag == false)
    {
       m_Valid = false;
       return false;
    }
    m_Valid = true;
    //>@Check FileType
    flag = GetItemElement("/FileType");
    if(flag == false)
    {
        m_Valid = false;
        return false;
    }
    QString tmpString = itemElement.text();
    if(tmpString.compare(MdpFileType, Qt::CaseInsensitive) != 0)
    {
        m_Valid = false;
        IDE_TRACE_STR(tmpString);
        return false;
    }
    return GetCfgparaGroup();
}

bool mdpManager::CloseMDP()
{
    SaveMDP();
    return true;
}

bool mdpManager::DelMDP()
{
    QFile file(m_FileDir + m_FileName);
    if(file.exists())
    {
        file.remove();
        return true;
    }
    return false;
}

bool mdpManager::SaveMDP()
{
    QDateTime time = QDateTime::currentDateTime();
    ModifyItemElement("/LastModiTime", time.toString("yyyy-MM-dd HH:mm:ss"));
    Save();
    return true;
}

bool mdpManager::RenameMDP(QString FileName)
{
    QString tmpString;
    int index = FileName.indexOf(MdpSuffix);
    if(index<0)
    {
        tmpString = FileName + MdpSuffix;
    }
    bool flag = Rename(tmpString);
    if(flag)
        m_FileName = tmpString;     //>@如果重命名成功，则修改m_FileName为重命名后的值。
    return flag;
}

bool mdpManager::GetCfgparaGroup()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return false;
    }
    bool flag = GetItemElement("/CfgPara");
    if(flag)
    {
        m_CfgparaElementGroup.clear();
        QDomElement tmpElement;
        QDomNodeList CfgItemList = itemElement.childNodes();  //>@查找BGIMG、Physical、Logic
        for(int i=0;i<CfgItemList.count();i++)
        {
            tmpElement = CfgItemList.at(i).toElement();
            if(tmpElement.isNull())
                continue;
            QString tmpString = tmpElement.tagName();
            if(tmpString.isEmpty())
                continue;
            QString tmpPath = QString("/%1").arg(tmpString);
            m_CfgparaElementGroup.insert(tmpPath, tmpElement);
            IDE_TRACE_STR(tmpPath);
            QString tmpSubCfgString = QString("/CfgPara/%1").arg(tmpString);
            flag = GetItemElement(tmpSubCfgString);
            if(!flag)
                continue;
            QDomNodeList SubCfgItemNodeList = itemElement.childNodes();   //>@查找CPU或者NET等
            for(int j=0;j<SubCfgItemNodeList.count();j++)              //>@插入配置页的子项元素
            {
                tmpElement = SubCfgItemNodeList.at(j).toElement();
                if(tmpElement.isNull())
                    continue;
                QString tmpSubString = tmpElement.tagName();
                if(tmpSubString.isEmpty())
                    continue;
                QString tmpSubPath = QString("/%1/%2").arg(tmpString).arg(tmpSubString);
                m_CfgparaElementGroup.insert(tmpSubPath, tmpElement);
                IDE_TRACE_STR(tmpSubPath);
                if(tmpString.compare("Physical")==0)
                {
                    IDE_TRACE();
                    m_PHElementGroup.insert(GetPHType(tmpSubString), tmpElement);
                }
                else if(tmpString.compare("Logic")==0)
                {
                    IDE_TRACE();
                    m_LGElementGroup.insert(GetLGType(tmpSubString), tmpElement);
                }
                IDE_TRACE();
            }
        }
    }
    return true;
}

bool mdpManager::ClrMDP()
{
    if(GetItemElement("/CfgPara"))
    {
        ClrDomCldNodes(itemElement);
        return true;
    }
    return false;
}

bool mdpManager::SetNickName(QString pNickName)
{
    if(!m_Valid)
        return false;
    return ModifyItemElement("/NickName", pNickName);
}

QString mdpManager::GetNickName()
{
    if(!m_Valid)
        return QString();
    bool flag = GetItemElement("/NickName");
    if(flag == false)
        return QString();
    return itemElement.text();
}

bool mdpManager::SetPassword(QString pPasswd)
{
    if(!m_Valid)
        return false;
    return ModifyItemElement("/Passwd", pPasswd);
}

QString mdpManager::GetPassword()
{
    if(!m_Valid)
        return QString();
    bool flag = GetItemElement("/Passwd");
    if(flag == false)
        return QString();
    return itemElement.text();
}

void mdpManager::DelDeviceDom(QString pPath)
{
    DEV_TYPE tmpDevType = GetDevType(pPath);
    if(tmpDevType == DEV_PHYSICAL)
    {
        PH_Type tmpType = GetPHType(getFileName(pPath));
        DelDeviceDom(tmpType, pPath);
    }
    else if(tmpDevType == DEV_LOGIC)
    {
        LG_Type tmpType = GetLGType(getFileName(pPath));
        DelDeviceDom(tmpType, pPath);
    }
}

void mdpManager::DelDeviceDom(PH_Type pType, QString pPath)
{
    QDomElement tmpElement = m_CfgparaElementGroup.take(pPath);
    if(tmpElement.isNull())
        return;
    if(pType != P_INVALID)
        m_PHElementGroup.take(pType);
    DelDom(tmpElement);
    m_IsChanged = true;
}

void mdpManager::DelDeviceDom(LG_Type pType, QString pPath)
{
    QDomElement tmpElement = m_CfgparaElementGroup.take(pPath);
    if(tmpElement.isNull())
        return;
    if(pType != L_INVALID)
        m_LGElementGroup.take(pType);
    DelDom(tmpElement);
    m_IsChanged = true;
}

QDomElement mdpManager::AddDeviceDom(QString pPath, QDomElement pInitElement)
{
    if(pPath.isEmpty() || pInitElement.isNull())
        return QDomElement();
    //>@获取父节点
    QString tmpParPath = getParentDirectory(pPath);
    QString tmpParName = GetDirectoryName(tmpParPath);
    tmpParPath = QString("/%1").arg(tmpParName);
    QDomElement tmpParElement = m_CfgparaElementGroup.value(tmpParPath);
    if(tmpParElement.isNull())
    {
        tmpParElement = createElement(tmpParName);
        if(!GetItemElement("/CfgPara"))
            return QDomElement();
        itemElement.appendChild(tmpParElement);
        m_CfgparaElementGroup.insert(tmpParPath, tmpParElement);
    }

    QDomElement deviceElement = createElement(pInitElement.tagName());
    //>@创建属性
    QDomNodeList CfgItemList = pInitElement.childNodes();
    for(int i=0;i<CfgItemList.count();i++)
    {
        QDomElement tmpSubInitElement = CfgItemList.at(i).toElement();
        if(tmpSubInitElement.isNull())
            continue;
        QDomElement tmpSubElement = CreateParaDom(tmpSubInitElement);
        if(tmpSubElement.isNull())
            continue;
        deviceElement.appendChild(tmpSubElement);
    }
    tmpParElement.appendChild(deviceElement);
    m_IsChanged = true;
    m_CfgparaElementGroup.insert(pPath, deviceElement);
    DEV_TYPE tmpDevType = GetDevType(pPath);
    if(tmpDevType == DEV_PHYSICAL)
    {
        PH_Type tmpType = GetPHType(getFileName(pPath));
        if(tmpType != P_INVALID)
            m_PHElementGroup.insert(tmpType, deviceElement);
    }
    else if(tmpDevType == DEV_LOGIC)
    {
        LG_Type tmpType = GetLGType(getFileName(pPath));
        if(tmpType != L_INVALID)
            m_LGElementGroup.insert(tmpType, deviceElement);
    }
    return deviceElement;
}

QDomElement mdpManager::CreateParaDom(QDomElement pInitElement)
{
    if(pInitElement.isNull())
        return QDomElement();
    QDomElement tmpElement;
    tmpElement = createElement(pInitElement.tagName());
    QString tmpParaString = pInitElement.attribute("Para");
    if(!tmpParaString.isEmpty())
        tmpElement.setAttribute("Para", tmpParaString);
    QString tmpListString = pInitElement.attribute("List");
    if(!tmpListString.isEmpty())
        tmpElement.setAttribute("List", tmpListString);
    tmpElement.appendChild(createTextNode(pInitElement.text()));
    return tmpElement;
}

bool mdpManager::ClrDomCldNodes(QDomElement pElement)
{
    if(pElement.isNull())
        return true;
    QDomNodeList cldNodeList = pElement.childNodes();
    for(int i=cldNodeList.count()-1;i>=0;i--)
    {
        pElement.removeChild(cldNodeList.at(i));
    }
    return true;
}

bool mdpManager::DelDom(QDomElement pElement)
{
    if(pElement.isNull())
        return true;
    if(!ClrDomCldNodes(pElement))
        return false;
    QString key = m_CfgparaElementGroup.key(pElement);
    QDomNode parNode = pElement.parentNode();
    parNode.removeChild(pElement);
    m_CfgparaElementGroup.remove(key);
    return true;
}

devManager::devManager(QString fileName) : xmlParse(fileName)
{
    int tmpProNum, tmpSuffixNum;
    tmpProNum = m_FileName.length();
    tmpSuffixNum = DevSuffix.length();
    if(tmpProNum <= tmpSuffixNum)
    {
        IDE_TRACE_INT(tmpProNum);
        return;
    }
    QString suffix = m_FileName.mid(tmpProNum-tmpSuffixNum, tmpSuffixNum);
    if(suffix != DevSuffix)
        m_FileName = QString("%1%2").arg(m_FileName).arg(DevSuffix);
}

bool devManager::NewDEV()
{
    bool flag = New();
    if(flag == false)
    {
        IDE_TRACE();
        m_Valid = false;
        return false;
    }
    m_Valid = true;

    appendChild(createProcessingInstruction("xml", XmlHead));

    QDomElement rootElement;
    rootElement = createElement("root");
    appendChild(rootElement);

    QDomElement tmpElement = createElement("FileType");
    tmpElement.appendChild(createTextNode("BST DEV File"));
    rootElement.appendChild(tmpElement);

    m_IsChanged = true;
    Save();
    return true;
}

bool devManager::OpenDEV()
{
    bool flag = Open();
    if(flag == false)
    {
       m_Valid = false;
       return false;
    }
    m_Valid = true;
    //>@Check FileType
    flag = GetItemElement("/FileType");
    if(flag == false)
    {
        m_Valid = false;
        return false;
    }
    QString tmpString = itemElement.text();
    if(tmpString.compare(DevFileType, Qt::CaseInsensitive) != 0)
    {
        m_Valid = false;
        IDE_TRACE_STR(tmpString);
        return false;
    }
    return GetDevGroup();
}

bool devManager::CloseDEV()
{
    SaveDEV();
    return true;
}

bool devManager::DelDEV()
{
    QFile file(m_FileDir + m_FileName);
    if(file.exists())
    {
        file.remove();
        return true;
    }
    return false;
}

bool devManager::SaveDEV()
{
    QDateTime time = QDateTime::currentDateTime();
    ModifyItemElement("/LastModiTime", time.toString("yyyy-MM-dd HH:mm:ss"));
    Save();
    return true;
}

bool devManager::RenameDEV(QString FileName)
{
    QString tmpString;
    int index = FileName.indexOf(DevSuffix);
    if(index<0)
    {
        tmpString = FileName + DevSuffix;
    }
    bool flag = Rename(tmpString);
    if(flag)
        m_FileName = tmpString;     //>@如果重命名成功，则修改m_FileName为重命名后的值。
    return flag;
}


rcManager::rcManager(QString fileName) : xmlParse(fileName)
{
    int tmpProNum, tmpSuffixNum;
    tmpProNum = m_FileName.length();
    tmpSuffixNum = RcSuffix.length();
    if(tmpProNum <= tmpSuffixNum)
    {
        IDE_TRACE_INT(tmpProNum);
        return;
    }
    QString suffix = m_FileName.mid(tmpProNum-tmpSuffixNum, tmpSuffixNum);
    if(suffix != RcSuffix)
        m_FileName = QString("%1%2").arg(m_FileName).arg(RcSuffix);
}

bool rcManager::NewRC()
{
    bool flag = New();
    if(flag == false)
    {
        IDE_TRACE();
        m_Valid = false;
        return false;
    }
    m_Valid = true;

    appendChild(createProcessingInstruction("xml", XmlHead));

    QDomElement rootElement;
    rootElement = createElement("root");
    appendChild(rootElement);

    QDomElement tmpElement = createElement("FileType");
        tmpElement.appendChild(createTextNode("BST RC File"));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("CreateTime");
        QDateTime time = QDateTime::currentDateTime();
        tmpElement.appendChild(createTextNode(time.toString("yyyy-MM-dd HH:mm:ss")));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("LastModiTime");
        tmpElement.appendChild(createTextNode(time.toString("yyyy-MM-dd HH:mm:ss")));
    rootElement.appendChild(tmpElement);

    Save();
    return true;
}

bool rcManager::OpenRC()
{
    bool flag = Open();
    if(flag == false)
    {
       m_Valid = false;
       return false;
    }
    m_Valid = true;
    //>@Check FileType
    flag = GetItemElement("/FileType");
    if(flag == false)
    {
        m_Valid = false;
        return false;
    }
    QString tmpString = itemElement.text();
    if(tmpString.compare(RcFileType, Qt::CaseInsensitive) != 0)
    {
        m_Valid = false;
        IDE_TRACE_STR(tmpString);
        return false;
    }
    return true;
}

bool rcManager::CloseRC()
{
    SaveRC();
    return true;
}

bool rcManager::DelRC()
{
    QFile file(m_FileDir + m_FileName);
    if(file.exists())
    {
        file.remove();
        return true;
    }
    return false;
}

bool rcManager::SaveRC()
{
    QDateTime time = QDateTime::currentDateTime();
    ModifyItemElement("/LastModiTime", time.toString("yyyy-MM-dd HH:mm:ss"));
    Save();
    return true;
}

bool rcManager::RenameRC(QString FileName)
{
    QString tmpString;
    int index = FileName.indexOf(RcSuffix);
    if(index<0)
    {
        tmpString = FileName + RcSuffix;
    }
    bool flag = Rename(tmpString);
    if(flag)
        m_FileName = tmpString;     //>@如果重命名成功，则修改m_FileName为重命名后的值。
    return flag;
}

bool rcManager::GetRcGroup()
{
    return true;
}

stcManager::stcManager(QString fileName) : xmlParse(fileName)
{
    int tmpProNum, tmpSuffixNum;
    tmpProNum = m_FileName.length();
    tmpSuffixNum = StcSuffix.length();
    if(tmpProNum <= tmpSuffixNum)
    {
        IDE_TRACE_INT(tmpProNum);
        return;
    }
    QString suffix = m_FileName.mid(tmpProNum-tmpSuffixNum, tmpSuffixNum);
    if(suffix != StcSuffix)
        m_FileName = QString("%1%2").arg(m_FileName).arg(StcSuffix);
}

bool stcManager::NewSTC()
{
    bool flag = New();
    if(flag == false)
    {
        IDE_TRACE();
        m_Valid = false;
        return false;
    }
    m_Valid = true;

    appendChild(createProcessingInstruction("xml", XmlHead));

    QDomElement rootElement;
    rootElement = createElement("root");
    appendChild(rootElement);

    QDomElement tmpElement = createElement("FileType");
        tmpElement.appendChild(createTextNode("BST STC File"));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("CreateTime");
        QDateTime time = QDateTime::currentDateTime();
        tmpElement.appendChild(createTextNode(time.toString("yyyy-MM-dd HH:mm:ss")));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("LastModiTime");
        tmpElement.appendChild(createTextNode(time.toString("yyyy-MM-dd HH:mm:ss")));
    rootElement.appendChild(tmpElement);

    m_IsChanged = true;
    Save();
    return true;
}

bool stcManager::OpenSTC()
{
    bool flag = Open();
    if(flag == false)
    {
       m_Valid = false;
       return false;
    }
    m_Valid = true;
    //>@Check FileType
    flag = GetItemElement("/FileType");
    if(flag == false)
    {
        m_Valid = false;
        return false;
    }
    QString tmpString = itemElement.text();
    if(tmpString.compare(StcFileType, Qt::CaseInsensitive) != 0)
    {
        m_Valid = false;
        IDE_TRACE_STR(tmpString);
        return false;
    }
    return true;
}

bool stcManager::CloseSTC()
{
    SaveSTC();
    return true;
}

bool stcManager::DelSTC()
{
    QFile file(m_FileDir + m_FileName);
    if(file.exists())
    {
        file.remove();
        return true;
    }
    return false;
}

bool stcManager::SaveSTC()
{
    QDateTime time = QDateTime::currentDateTime();
    ModifyItemElement("/LastModiTime", time.toString("yyyy-MM-dd HH:mm:ss"));
    Save();
    return true;
}

bool stcManager::RenameSTC(QString FileName)
{
    QString tmpString;
    int index = FileName.indexOf(StcSuffix);
    if(index<0)
    {
        tmpString = FileName + StcSuffix;
    }
    bool flag = Rename(tmpString);
    if(flag)
        m_FileName = tmpString;     //>@如果重命名成功，则修改m_FileName为重命名后的值。
    return flag;
}

bool stcManager::GetStcGroup()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return false;
    }
    bool flag = GetItemElement("/TestCase");
    if(flag)
    {
        m_CaseItemGroup.clear();
        QDomElement tmpElement;
        QDomNodeList CaseItemList = itemElement.childNodes();
        for(int i=0;i<CaseItemList.count();i++)
        {
            tmpElement = CaseItemList.at(i).toElement();
            QString tmpString = tmpElement.tagName();
            QString tmpCaseNameString = QString("/%1").arg(tmpString);
            m_CaseItemGroup.insert(tmpCaseNameString, tmpElement);//>@插入配置页根元素
            //IDE_TRACE_STR(tmpCaseNameString);
            QString tmpSubCaseString = QString("/TestCase/%1").arg(tmpString);
            flag = GetItemElement(tmpSubCaseString);
            if(flag)
            {
                QDomNodeList SubCaseItemNodeList = itemElement.childNodes();
                for(int j=0;j<SubCaseItemNodeList.count();j++)              //>@插入配置页的子项元素
                {
                    tmpElement = SubCaseItemNodeList.at(j).toElement();
                    QString tmpSubString = tmpElement.tagName();
                    QString tmpSubCaseNameString = QString("/%1/%2").arg(tmpString).arg(tmpSubString);
                    m_CaseItemGroup.insert(tmpSubCaseNameString, tmpElement);
                    //IDE_TRACE_STR(tmpSubCaseNameString);
                }
            }
        }
    }
    return true;
}

logTipManager::logTipManager(QString fileName) :
    xmlParse(fileName)
{
    m_Valid = false;
    QFile file(fileName);
    bool flag = file.exists();
    if(flag == false)
    {
        IDE_TRACE();
        return ;
    }
    flag = Open();
    if(flag == false)
    {
       IDE_TRACE();
       return ;
    }
    //>@Check filetype
    flag = GetItemElement("/FileType");
    if(flag == false)
    {
        IDE_TRACE();
        return;
    }
    QString tmpString = itemElement.text();
    if(tmpString.compare(LogTipFileType, Qt::CaseInsensitive) != 0)
    {
        IDE_TRACE_STR(tmpString);
        return;
    }
    m_Valid = true;

    getNewLogTipCategory();
}

bool logTipManager::getNewLogTipCategory()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return false;
    }
    bool flag = GetItemElement("/Tip");
    if(flag == false)
    {
        IDE_TRACE();
        return false;
    }
    GlobalPara.m_LogTipGroup.clear();
    QDomElement tmpElement;
    QDomNode tmpNode;
    QDomNodeList newCategoryNode = GetChildNodes();
    for(int i=0;i < newCategoryNode.count();++i)
    {
        tmpNode = newCategoryNode.at(i);
        tmpElement = tmpNode.toElement();
        QString tmpTipName = tmpElement.tagName();
        LOGTIP_INFO *tmpLogTip = new LOGTIP_INFO;
        tmpLogTip->level = tmpElement.attribute("Para");
        tmpLogTip->content = tmpElement.text();
        GlobalPara.m_LogTipGroup.insert(tmpTipName, tmpLogTip);
        IDE_TRACE_STR(tmpTipName);
    }
    return true;
}


logManager::logManager(QString fileName) :
    xmlParse(fileName)
{
    if(m_FileName.endsWith(".log") == false)
        m_FileName.append(".log");
}

bool logManager::NewLog()
{
    bool flag = New();
    if(flag == false)
    {
        IDE_TRACE();
        m_Valid = false;
        return false;
    }
    m_Valid = true;

    appendChild(createProcessingInstruction("xml", XmlHead));

    QDomElement rootElement;
    rootElement = createElement("root");
    appendChild(rootElement);

    QDomElement tmpElement = createElement("FileType");
        tmpElement.appendChild(createTextNode("Analysis Log File"));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("CreateTime");
        QDateTime time = QDateTime::currentDateTime();
        tmpElement.appendChild(createTextNode(time.toString("yyyy-MM-dd HH:mm:ss")));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("LastModiTime");
        tmpElement.appendChild(createTextNode(time.toString("yyyy-MM-dd HH:mm:ss")));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("Log");
        tmpElement.appendChild(createElement("History"));
        tmpElement.appendChild(createElement("Error"));
    rootElement.appendChild(tmpElement);

    m_IsChanged = true;
    Save();
    return true;
}

bool logManager::OpenLog()
{
    bool flag = Open();
    if(flag == false)
    {
       m_Valid = false;
       return false;
    }
    m_Valid = true;
    //>@Check FileType
    flag = GetItemElement("/FileType");
    if(flag == false)
    {
        m_Valid = false;
        return false;
    }
    QString tmpString = itemElement.text();
    if(tmpString.compare("Analysis Log File", Qt::CaseInsensitive) != 0)
    {
        m_Valid = false;
        IDE_TRACE_STR(tmpString);
        return false;
    }
    return true;
}

bool logManager::CloseLog()
{
    SaveLog();
    return true;
}

bool logManager::SaveLog()
{
    QDateTime time = QDateTime::currentDateTime();
    ModifyItemElement("/LastModiTime", time.toString("yyyy-MM-dd HH:mm:ss"));
    Save();
    return true;
}

cmdManager::cmdManager() : xmlParse()
{

}

cmdManager::cmdManager(QString fileName) : xmlParse(fileName)
{
    int tmpProNum, tmpSuffixNum;
    tmpProNum = m_FileName.length();
    tmpSuffixNum = CmdSuffix.length();
    if(tmpProNum <= tmpSuffixNum)
    {
        IDE_TRACE_INT(tmpProNum);
        return;
    }
    QString suffix = m_FileName.mid(tmpProNum-tmpSuffixNum, tmpSuffixNum);
    if(suffix != CmdSuffix)
        m_FileName = QString("%1%2").arg(m_FileName).arg(CmdSuffix);
}

bool cmdManager::NewCMD()
{
    bool flag = New();
    if(flag == false)
    {
        IDE_TRACE();
        m_Valid = false;
        return false;
    }
    m_Valid = true;

    appendChild(createProcessingInstruction("xml", XmlHead));

    QDomElement rootElement;
    rootElement = createElement("root");
    appendChild(rootElement);

    QDomElement tmpElement = createElement("FileType");
        tmpElement.appendChild(createTextNode("BST DEV File"));
    rootElement.appendChild(tmpElement);

    m_IsChanged = true;
    Save();
    return true;
}

bool cmdManager::OpenCMD()
{
    bool flag = Open();
    if(flag == false)
    {
       m_Valid = false;
       return false;
    }
    m_Valid = true;
    //>@Check FileType
    flag = GetItemElement("/FileType");
    if(flag == false)
    {
        m_Valid = false;
        return false;
    }
    QString tmpString = itemElement.text();
    if(tmpString.compare(CmdFileType, Qt::CaseInsensitive) != 0)
    {
        m_Valid = false;
        IDE_TRACE_STR(tmpString);
        return false;
    }
    return true;
}

bool cmdManager::CloseCMD()
{
    SaveCMD();
    return true;
}

bool cmdManager::DelCMD()
{
    QFile file(m_FileDir + m_FileName);
    if(file.exists())
    {
        file.remove();
        return true;
    }
    return false;
}

bool cmdManager::SaveCMD()
{
    QDateTime time = QDateTime::currentDateTime();
    ModifyItemElement("/LastModiTime", time.toString("yyyy-MM-dd HH:mm:ss"));
    Save();
    return true;
}

bool cmdManager::RenameCMD(QString FileName)
{
    QString tmpString;
    int index = FileName.indexOf(CmdSuffix);
    if(index<0)
    {
        tmpString = FileName + CmdSuffix;
    }
    bool flag = Rename(tmpString);
    if(flag)
        m_FileName = tmpString;     //>@如果重命名成功，则修改m_FileName为重命名后的值。
    return flag;
}

//>@   /Ui/Upgrade/Video
bool cmdManager::GetCMDGroup()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return false;
    }
    bool flag = GetItemElement("/Device");
    if(flag)
    {
        m_CfgItemGroup.clear();
        QDomElement tmpElement;
        QDomNodeList CfgItemList = itemElement.childNodes();
        for(int i=0;i<CfgItemList.count();i++)
        {
            tmpElement = CfgItemList.at(i).toElement();
            QString tmpString = tmpElement.tagName();
            QString tmpCfgNameString = QString("/%1").arg(tmpString);
            m_CfgItemGroup.insert(tmpCfgNameString, tmpElement);//>@插入配置页根元素
            //IDE_TRACE_STR(tmpCfgNameString);
        }
    }
    return true;
}

mapManager::mapManager(QString fileName) : xmlParse(fileName)
{
    int tmpProNum, tmpSuffixNum;
    tmpProNum = m_FileName.length();
    tmpSuffixNum = MapSuffix.length();
    if(tmpProNum <= tmpSuffixNum)
    {
        IDE_TRACE_INT(tmpProNum);
        return;
    }
    QString suffix = m_FileName.mid(tmpProNum-tmpSuffixNum, tmpSuffixNum);
    if(suffix != MapSuffix)
        m_FileName = QString("%1%2").arg(m_FileName).arg(MapSuffix);
}

bool mapManager::NewMAP()
{
    bool flag = New();
    if(flag == false)
    {
        IDE_TRACE();
        m_Valid = false;
        return false;
    }
    m_Valid = true;

    appendChild(createProcessingInstruction("xml", XmlHead));

    QDomElement rootElement;
    rootElement = createElement("root");
    appendChild(rootElement);

    QDomElement tmpElement = createElement("FileType");
        tmpElement.appendChild(createTextNode("BST MAP File"));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("CreateTime");
        QDateTime time = QDateTime::currentDateTime();
        tmpElement.appendChild(createTextNode(time.toString("yyyy-MM-dd HH:mm:ss")));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("UiPath");
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("Map");
    rootElement.appendChild(tmpElement);

    m_IsChanged = true;
    Save();
    return true;
}

bool mapManager::OpenMAP()
{
    bool flag = Open();
    if(flag == false)
    {
       m_Valid = false;
       return false;
    }
    m_Valid = true;
    //>@Check FileType
    flag = GetItemElement("/FileType");
    if(flag == false)
    {
        m_Valid = false;
        return false;
    }
    QString tmpString = itemElement.text();
    if(tmpString.compare(MapFileType, Qt::CaseInsensitive) != 0)
    {
        m_Valid = false;
        IDE_TRACE_STR(tmpString);
        return false;
    }
    return GetCfgparaGroup();
}

bool mapManager::CloseMAP()
{
    SaveMAP();
    return true;
}

bool mapManager::DelMAP()
{
    QFile file(m_FileDir + m_FileName);
    if(file.exists())
    {
        file.remove();
        return true;
    }
    return false;
}

bool mapManager::SaveMAP()
{
    QDateTime time = QDateTime::currentDateTime();
    ModifyItemElement("/LastModiTime", time.toString("yyyy-MM-dd HH:mm:ss"));
    Save();
    return true;
}

bool mapManager::RenameMAP(QString FileName)
{
    QString tmpString;
    int index = FileName.indexOf(MapSuffix);
    if(index<0)
    {
        tmpString = FileName + MapSuffix;
    }
    bool flag = Rename(tmpString);
    if(flag)
        m_FileName = tmpString;     //>@如果重命名成功，则修改m_FileName为重命名后的值。
    return flag;
}

bool mapManager::SetUIPath(QString UiPath)
{
    ModifyItemElement("/UiPath", UiPath);
    return false;
}

QString mapManager::GetUIPath()
{
    bool flag = GetItemElement("/UiPath");
    if(flag == false)
    {
        return QString();
    }
    return itemElement.text();
}

bool mapManager::GetCfgparaGroup()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return false;
    }
    bool flag = GetItemElement("/Map");
    if(flag)
    {
        m_CfgparaElementGroup.clear();
        QDomElement tmpElement;
        QDomNodeList CfgItemList = itemElement.childNodes();
        for(int i=0;i<CfgItemList.count();i++)
        {
            tmpElement = CfgItemList.at(i).toElement();
            QString tmpString = tmpElement.tagName();
            if(tmpString.isEmpty())
                continue;
            QString tmpCfgNameString = QString("/%1").arg(tmpString);
            m_CfgparaElementGroup.insert(tmpCfgNameString, tmpElement);//>@插入配置页根元素
            QString tmpSubCfgString = QString("/Map/%1").arg(tmpString);
            flag = GetItemElement(tmpSubCfgString);
            if(flag)
            {
                QDomNodeList SubCfgItemNodeList = itemElement.childNodes();
                for(int j=0;j<SubCfgItemNodeList.count();j++)              //>@插入配置页的子项元素
                {
                    tmpElement = SubCfgItemNodeList.at(j).toElement();
                    QString tmpSubString = tmpElement.tagName();
                    if(tmpSubString.isEmpty())
                        continue;
                    QString tmpSubCfgNameString = QString("/%1/%2").arg(tmpString).arg(tmpSubString);
                    m_CfgparaElementGroup.insert(tmpSubCfgNameString, tmpElement);
                }
            }
        }
    }
    return true;
}

bool mapManager::ClrMap()
{
    if(GetItemElement("/Map"))
        ClrDomCldNodes(itemElement);
    return false;
}

bool mapManager::ClrDomCldNodes(QDomElement pElement)
{
    if(pElement.isNull())
        return true;
    QDomNodeList cldNodeList = pElement.childNodes();
    for(int i=cldNodeList.count()-1;i>=0;i--)
    {
        pElement.removeChild(cldNodeList.at(i));
    }
    return true;
}

bool mapManager::DelDom(QDomElement pElement)
{
    if(pElement.isNull())
        return true;
    if(!ClrDomCldNodes(pElement))
        return false;
    QString key = m_CfgparaElementGroup.key(pElement);
    QDomNode parNode = pElement.parentNode();
    parNode.removeChild(pElement);
    m_CfgparaElementGroup.remove(key);
    return true;
}

bool mapManager::AddDomNewNode(DomInfo pDomInfo)
{
    if(!CreateItemElement(pDomInfo.mParLabel))
       return false;

    QString newLabel = pDomInfo.mParLabel;
    if(!newLabel.endsWith("/"))
        newLabel.append("/");
    newLabel.append(pDomInfo.mTagName);

    if(GetItemElement(newLabel))
        DelDom(itemElement);

    if(pDomInfo.mTagName.isEmpty())
        return false;
    QDomElement element = createElement(pDomInfo.mTagName);
    if(!pDomInfo.mAtPara.isEmpty())
        element.setAttribute("Para", pDomInfo.mAtPara);
    if(!pDomInfo.mAtList.isEmpty())
        element.setAttribute("List", pDomInfo.mAtList);
    element.appendChild(createTextNode(pDomInfo.mText));
    itemElement.appendChild(element);
    return true;
}

bool mapManager::MapItemToElement(MAPITEM_INFO *pMapItemInfo, QDomElement &pParentElement)
{
    if(pMapItemInfo == 0 || pParentElement.isNull())
        return false;
    QString tmpMapContent;
    QString tmpEventName = pParentElement.tagName();
    if(pMapItemInfo->mMediaInfo.mFuncMap.isEmpty() == false)
        tmpMapContent.append(QString("%1:%2").arg(tmpEventName).arg(pMapItemInfo->mMediaInfo.mFuncMap));
    if(pMapItemInfo->mMediaInfo.mImage.isEmpty() == false)
        tmpMapContent.append(QString(";Image:%1").arg(pMapItemInfo->mMediaInfo.mImage));
    if(pMapItemInfo->mMediaInfo.mAudio.isEmpty() == false)
        tmpMapContent.append(QString(";Audio:%1").arg(pMapItemInfo->mMediaInfo.mAudio));
    if(pMapItemInfo->mMediaInfo.mVideo.isEmpty() == false)
        tmpMapContent.append(QString(";Video:%1").arg(pMapItemInfo->mMediaInfo.mVideo));
    if(pMapItemInfo->mMediaInfo.mLogo.isEmpty() == false)
        tmpMapContent.append(QString(";Logo:%1").arg(pMapItemInfo->mMediaInfo.mLogo));
    if(pMapItemInfo->mMediaInfo.mText.isEmpty() == false)
        tmpMapContent.append(QString(";Text:%1").arg(pMapItemInfo->mMediaInfo.mText));
    QString tmpString = QString("Key_%1").arg(pMapItemInfo->mEventValue);
    QDomElement tmpElement = createElement(tmpString);
    tmpElement.appendChild(createTextNode(tmpMapContent));
    pParentElement.appendChild(tmpElement);
    return true;
}

MAPITEM_INFO *mapManager::ElementToMapItem(QDomElement pElement)
{
    if(pElement.isNull())
        return (MAPITEM_INFO *)0;
    QString tmpEventValue = pElement.tagName(); //Floor/key_1
    if(tmpEventValue.isEmpty())
        return (MAPITEM_INFO *)0;
    QStringList tmpKeyList = tmpEventValue.split("_");
    if(tmpKeyList.count() == 2 && tmpKeyList.at(0).compare("Key", Qt::CaseInsensitive) == 0)
    {
        MAPITEM_INFO *tmpMapItemInfo = new MAPITEM_INFO;
        tmpMapItemInfo->mEventValue = tmpKeyList.at(1);
        QString tmpMapContent = pElement.text();
        QStringList tmpActionList = tmpMapContent.split(";");
        for(int k=0;k<tmpActionList.count();k++)   //content
        {
            QStringList tmpChildActionList = tmpActionList.at(k).split(":");
            if(tmpChildActionList.count() != 2)
                continue;
            QString tmpChildKey = tmpChildActionList.at(0);
            if(tmpChildKey.isEmpty())
                continue;
            QString tmpChildContent = tmpChildActionList.at(1);
            if(tmpChildContent.isEmpty())
                continue;
            tmpMapItemInfo->mMediaInfo.mFuncMap = tmpChildContent;
        }
        return tmpMapItemInfo;
    }
    return (MAPITEM_INFO *)0;
}

bool mapManager::GetMapItemGroup()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return false;
    }
    bool flag = GetItemElement("/Map");
    if(flag == false)
    {
        IDE_TRACE();
        return false;
    }
    m_MapItemInfoGroup.clear();
    QDomNodeList tmpList = itemElement.childNodes();
    for(int i=0;i<tmpList.count();i++)
    {
        QDomElement tmpElement = tmpList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpEventName = tmpElement.tagName();
        if(tmpEventName.isEmpty())
            continue;
        QDomNodeList ChildrenList = tmpElement.childNodes();
        for(int j=0;j<ChildrenList.count();j++)
        {
            QDomElement tmpChildElement = ChildrenList.at(j).toElement();
            MAPITEM_INFO *tmpMapItemInfo = ElementToMapItem(tmpChildElement);
            if(tmpMapItemInfo == 0)
                continue;
            m_MapItemInfoGroup.insertMulti(tmpEventName, tmpMapItemInfo);
            IDE_TRACE_STR(tmpEventName);
        }
    }
    IDE_TRACE_INT(m_MapItemInfoGroup.count("Floor"));
    return true;
}

bool mapManager::SaveMapItemGroup()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return false;
    }
    bool flag = GetItemElement("/Map");
    if(flag == false)
    {
        IDE_TRACE();
        return false;
    }
    //>@清除Map下的元素
    QDomNodeList tmpNodeList = itemElement.childNodes();
    for(int j=tmpNodeList.count()-1;j>=0;j--)
    {
        QDomNode tmp = itemElement.removeChild(tmpNodeList.at(j));
        tmp.clear();
    }
    //>@从m_MapItemInfoGroup添加元素
    QList<QString> tmpKeyList = m_MapItemInfoGroup.uniqueKeys();
    for(int i=0;i<tmpKeyList.count();i++)
    {
        QString tmpEventName = tmpKeyList.at(i);
        QList<MAPITEM_INFO *> tmpMapItemInfoList = m_MapItemInfoGroup.values(tmpEventName);
        if(tmpMapItemInfoList.count() > 0)
        {
            //>@没有则创建
            QDomElement tmpParentElement = itemElement.firstChildElement(tmpEventName);
            if(tmpParentElement.isNull())
            {
                tmpParentElement = createElement(tmpEventName);
                itemElement.appendChild(tmpParentElement);
            }
            for(int j=0;j<tmpMapItemInfoList.count();j++)
            {
                MAPITEM_INFO * tmpMapItemInfo = tmpMapItemInfoList.at(j);
                if(tmpMapItemInfo)
                {
                    MapItemToElement(tmpMapItemInfo, tmpParentElement);
                }
            }
        }
    }
    m_IsChanged = true;
    return true;
}

bool mapManager::AddMapItem(QString pEventName, QString pEventValue, MEDIA_INFO pMediaInfo)
{
    QList<MAPITEM_INFO *> tmpMapItemInfoList = m_MapItemInfoGroup.values(pEventName);
    for(int i=0;i<tmpMapItemInfoList.count();i++)
    {
        MAPITEM_INFO * tmpMapItemInfo = tmpMapItemInfoList.at(i);
        if(tmpMapItemInfo && tmpMapItemInfo->mEventValue.compare(pEventValue, Qt::CaseInsensitive) == 0)
        {
            return false;
        }
    }
    MAPITEM_INFO * tmpMapItemInfo = new MAPITEM_INFO;
    tmpMapItemInfo->mEventValue = pEventValue;
    tmpMapItemInfo->mMediaInfo = pMediaInfo;
    m_MapItemInfoGroup.insertMulti(pEventName, tmpMapItemInfo);
    return true;
}

bool mapManager::DelMapItem(QString pEventName, QString pEventValue)
{
    QList<MAPITEM_INFO *> tmpMapItemInfoList = m_MapItemInfoGroup.values(pEventName);
    for(int i=0;i<tmpMapItemInfoList.count();i++)
    {
        MAPITEM_INFO * tmpMapItemInfo = tmpMapItemInfoList.at(i);
        if(tmpMapItemInfo && tmpMapItemInfo->mEventValue.compare(pEventValue, Qt::CaseInsensitive) == 0)
        {
            m_MapItemInfoGroup.remove(pEventName, tmpMapItemInfo);
            delete tmpMapItemInfo;
        }
    }
    return true;
}

MAPITEM_INFO *mapManager::GetMapItem(QString pEventName, QString pEventValue)
{
    QList<MAPITEM_INFO *> tmpMapItemInfoList = m_MapItemInfoGroup.values(pEventName);
    for(int i=0;i<tmpMapItemInfoList.count();i++)
    {
        MAPITEM_INFO * tmpMapItemInfo = tmpMapItemInfoList.at(i);
        if(tmpMapItemInfo && tmpMapItemInfo->mEventValue.compare(pEventValue, Qt::CaseInsensitive) == 0)
        {
            return tmpMapItemInfo;
        }
    }
    return (MAPITEM_INFO *)0;
}

bool mapManager::ModifyMapItem(QString pEventName, QString pEventValue, MEDIA_INFO mMediaInfo)
{
    QList<MAPITEM_INFO *> tmpMapItemInfoList = m_MapItemInfoGroup.values(pEventName);
    for(int i=0;i<tmpMapItemInfoList.count();i++)
    {
        MAPITEM_INFO * tmpMapItemInfo = tmpMapItemInfoList.at(i);
        if(tmpMapItemInfo && tmpMapItemInfo->mEventValue.compare(pEventValue, Qt::CaseInsensitive) == 0)
        {
            tmpMapItemInfo->mMediaInfo = mMediaInfo;
            return true;
        }
    }
    return false;
}

devInfoManager::devInfoManager(QString fileName) : xmlParse(fileName)
{
    int tmpProNum, tmpSuffixNum;
    tmpProNum = m_FileName.length();
    tmpSuffixNum = DevInfoSuffix.length();
    if(tmpProNum <= tmpSuffixNum)
    {
        IDE_TRACE_INT(tmpProNum);
        return;
    }
    QString suffix = m_FileName.mid(tmpProNum-tmpSuffixNum, tmpSuffixNum);
    if(suffix != DevInfoSuffix)
        m_FileName = QString("%1%2").arg(m_FileName).arg(DevInfoSuffix);
}

bool devInfoManager::NewDevInfo()
{
    bool flag = New();
    if(flag == false)
    {
        IDE_TRACE();
        m_Valid = false;
        return false;
    }
    m_Valid = true;

    appendChild(createProcessingInstruction("xml", XmlHead));

    QDomElement rootElement;
    rootElement = createElement("root");
    appendChild(rootElement);

    QDomElement tmpElement = createElement("FileType");
        tmpElement.appendChild(createTextNode("BST DEVINFO File"));
    rootElement.appendChild(tmpElement);

    m_IsChanged = true;
    Save();
    return true;
}

bool devInfoManager::OpenDevInfo()
{
    bool flag = Open();
    if(flag == false)
    {
       m_Valid = false;
       return false;
    }
    m_Valid = true;
    //>@Check FileType
    flag = GetItemElement("/FileType");
    if(flag == false)
    {
        m_Valid = false;
        return false;
    }
    QString tmpString = itemElement.text();
    if(tmpString.compare(DevInfoFileType, Qt::CaseInsensitive) != 0)
    {
        m_Valid = false;
        IDE_TRACE_STR(tmpString);
        return false;
    }
    return true;
}

bool devInfoManager::CloseDevInfo()
{
    SaveDevInfo();
    return true;
}

bool devInfoManager::DelDevInfo()
{
    QFile file(m_FileDir + m_FileName);
    if(file.exists())
    {
        file.remove();
        return true;
    }
    return false;
}

bool devInfoManager::SaveDevInfo()
{
    Save();
    return true;
}

bool devInfoManager::RenameDevInfo(QString FileName)
{
    QString tmpString;
    int index = FileName.indexOf(DevInfoSuffix);
    if(index<0)
    {
        tmpString = FileName + DevInfoSuffix;
    }
    bool flag = Rename(tmpString);
    if(flag)
        m_FileName = tmpString;     //>@如果重命名成功，则修改m_FileName为重命名后的值。
    return flag;
}

fontManager::fontManager(QString fileName) : xmlParse(fileName)
{
    int tmpProNum, tmpSuffixNum;
    tmpProNum = m_FileName.length();
    tmpSuffixNum = FontSuffix.length();
    if(tmpProNum <= tmpSuffixNum)
    {
        IDE_TRACE_INT(tmpProNum);
        return;
    }
    QString suffix = m_FileName.mid(tmpProNum-tmpSuffixNum, tmpSuffixNum);
    if(suffix != FontSuffix)
        m_FileName = QString("%1%2").arg(m_FileName).arg(FontSuffix);
}

bool fontManager::NewFont()
{
    bool flag = New();
    if(flag == false)
    {
        IDE_TRACE();
        m_Valid = false;
        return false;
    }
    m_Valid = true;

    appendChild(createProcessingInstruction("xml", XmlHead));

    QDomElement rootElement;
    rootElement = createElement("root");
    appendChild(rootElement);

    QDomElement tmpElement = createElement("FileType");
        tmpElement.appendChild(createTextNode("BST Font File"));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("CreateTime");
        QDateTime time = QDateTime::currentDateTime();
        tmpElement.appendChild(createTextNode(time.toString("yyyy-MM-dd HH:mm:ss")));
    rootElement.appendChild(tmpElement);

    tmpElement = createElement("Content");
    rootElement.appendChild(tmpElement);

    m_IsChanged = true;
    Save();
    return true;
}

bool fontManager::OpenFont()
{
    bool flag = Open();
    if(flag == false)
    {
       m_Valid = false;
       return false;
    }
    m_Valid = true;
    //>@Check FileType
    flag = GetItemElement("/FileType");
    if(flag == false)
    {
        m_Valid = false;
        return false;
    }
    QString tmpString = itemElement.text();
    if(tmpString.compare(FontFileType, Qt::CaseInsensitive) != 0)
    {
        m_Valid = false;
        IDE_TRACE_STR(tmpString);
        return false;
    }
    return GetParagraphGroup();
}

bool fontManager::CloseFont()
{
    SaveFont();
    return true;
}

bool fontManager::DelFont()
{
    QFile file(m_FileDir + m_FileName);
    if(file.exists())
    {
        file.remove();
        return true;
    }
    return false;
}

bool fontManager::SaveFont()
{
    Save();
    return true;
}

bool fontManager::RenameFont(QString FileName)
{
    QString tmpString;
    int index = FileName.indexOf(FontSuffix);
    if(index<0)
    {
        tmpString = FileName + FontSuffix;
    }
    bool flag = Rename(tmpString);
    if(flag)
        m_FileName = tmpString;     //>@如果重命名成功，则修改m_FileName为重命名后的值。
    return flag;
}

bool fontManager::GetParagraphGroup()
{
    if(!m_Valid)
    {
        IDE_TRACE();
        return false;
    }
    bool flag = GetItemElement("/Content");
    if(flag)
    {
        m_ParagraphGroup.clear();
        QDomElement tmpElement;
        QDomNodeList PrargraphList = itemElement.childNodes();
        for(int i=0;i<PrargraphList.count();i++)
        {
            tmpElement = PrargraphList.at(i).toElement();
            QString tmpDesc = tmpElement.tagName();
            m_ParagraphGroup.insert(tmpDesc, tmpElement);//>@插入配置页根元素
            //IDE_TRACE_STR(tmpDesc);
        }
    }
    return true;
}

bool fontManager::AddParagraph(QString pDesc, QString pString, FONT_INFO pInfo)
{
    if(GetItemElement("/Content") == false)
    {
        IDE_TRACE();
        return false;
    }
    QDomElement tmpElement = m_ParagraphGroup.value(pDesc);
    if(tmpElement.isNull() == false)
    {
        IDE_TRACE();//>@如果已经存在则返回false
        return false;
    }
    tmpElement = createElement(pDesc);
    tmpElement.setAttribute("Para", pInfo.toString());
    tmpElement.appendChild(createTextNode(pString));
    itemElement.appendChild(tmpElement);
    m_ParagraphGroup.insert(pDesc, tmpElement);
    return true;
}

bool fontManager::ModifyParagraph(QString pDesc, QString pString, FONT_INFO pInfo)
{
    QDomElement tmpElement = m_ParagraphGroup.value(pDesc);
    if(tmpElement.isNull())
    {
        IDE_TRACE();//>@如果不存在则返回false
        return false;
    }
    tmpElement.setAttribute("Para", pInfo.toString());
    return ModifyItemElement(tmpElement, pString);
}

bool fontManager::DelParagraph(QString pDesc)
{
    QDomElement tmpElement = m_ParagraphGroup.value(pDesc);
    if(tmpElement.isNull())
    {
        IDE_TRACE();//>@如果不存在则返回true
        return true;
    }
    if(GetItemElement("/Content") == false)
    {
        IDE_TRACE();
        return false;
    }
    itemElement.removeChild(tmpElement);
    m_ParagraphGroup.remove(pDesc);
    return true;
}

bool fontManager::GetParagraph(QString pDesc, QString &pString, FONT_INFO &pInfo)
{
    QDomElement tmpElement = m_ParagraphGroup.value(pDesc);
    if(tmpElement.isNull())
    {
        IDE_TRACE();//>@如果不存在则返回false
        return false;
    }
    pInfo = tmpElement.attribute("Para");
    pString = tmpElement.text();
    return true;
}

Global GlobalPara;
