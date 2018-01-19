#ifndef GLOBAL_H
#define GLOBAL_H

#include "globalfun.h"

#include "logger.h"
#include "propertyconfigurator.h"
#include "fileappender.h"

class bstproManager;
class uiManager;
class mdpManager;
class stcManager;
class rcManager;
class cfgManager;
class logTipManager;
class mapManager;

class IMAGEITEMINFO;
class Global;
class COM_INFO;

extern Global GlobalPara;

FILEPATH_TYPE GetRcFileType(QString pFileName, uiManager *pManager = 0);
FILEPATH_TYPE RegulatRcFile(QString& pFileName, uiManager *pManager = 0);

void Logger(QString Key = QString(), QStringList tmplist = QStringList());

void GetApplicationDirectory();
QString GetHostPCInfo();
bool GetScreenInfo();
bool InitLoggerInfo();
bool GetBootConfigure();
bool InitializeLinguist();

class baseaudio;
extern void SystemAudio(QString pAudioPath, bool pPrior=false, bool pRepeat=false, quint32 pInterval=0);

class Global : public QObject
{
    Q_OBJECT
public:
    explicit Global(QObject *parent=0);
    ~Global();

    QRect m_DeskRect;
    QRect m_ScreenRect;
    int   m_ScreenNum;

    QString m_SoftwareDir;          //>@应用程序目录

    //>@日志相关
    Log4Qt::Logger* m_Logger;
    QHash<QString, LOGTIP_INFO*> m_LogTipGroup;

    //>@音频器
    baseaudio *m_SystemAudio;

    //>@QML相关
#ifdef SURRPORT_QML
    QDeclarativeEngine *m_QMLEngine;
#endif

    //>@BOOT.XML的全局变量
    QHash<QString, QDomElement>         NewCategoryGroup;               //>@新建form中支持的类型集合
    QHash<QString, SolutionCategory>    SolutionCategoryGroup;          //>@支持的项目类型
    QHash<QString, FileCategory>        FileCategoryGroup;              //>@支持的文件类型

    QHash<QString, ComCategory*>        m_ComCategoryGroup;             //>@可用的界面组件

    QHash<QString, QDomElement>         m_ConfigInfoGroup;              //>@可用的设备组件
    QHash<PH_Type, QDomElement>         m_PHonfigInfoGroup;
    QHash<LG_Type, QDomElement>         m_LGonfigInfoGroup;

    QHash<QString, QDomElement>         m_ProtocolInterfaceGroup;       //>@接口协议配置项

    IMAGEITEMINFO *m_ActiveImageItemInfo;

#ifdef DISPLAY
    DeviceManager *m_DeviceManager;
#endif

#ifdef IDE
    QScriptEngine *m_ScriptEngine;

    QSqlDatabase   m_SqlDatabase;   //>@全局的数据库管理连接。

    QMainWindow   *m_MainWindow;

    QToolBar      *m_ProjectBar;
    QTabWidget    *m_TabPageManager;
    QToolBar      *m_SqlToolBar;
    QToolBar      *m_DockViewToolBar;

    //状态栏中标签
    QStatusBar    *statusBar;
    QLabel        *docEditStatus,
                  *keyboardStatus,
                  *searchStatus;
    QProgressBar  *progressStatus;
    QTextEdit     *m_OutputMsgList;

public slots:
    void showMessage(QString pShowMsg);
    void outputMessage(QString pShowMsg);
    void updateProgress(int pProgress);
    void updateProgress(QString pShowMsg, int pProgress);
#endif
};

/*
    XX.bstpro
        xx.ui
        xx.map
        xx.mdp
        xx.stc
*/

class bstproManager : public xmlParse
{
public:
    bstproManager(QString fileName);

    ~bstproManager(){}

    static const QString BstProSuffix;
    static const QString BstProFileType;

    bool NewPro();
    bool OpenPro();
    bool ClosePro();
    bool DelPro();
    bool SavePro();
    bool RenamePro(QString FileName);

    bool AddFile(QString pFileName);
    bool AddFile(QString pKey, QString pFileName);
    bool DelFile(QString pKey);
    QString GetFile(QString pKey);
};

class uiManager : public xmlParse
{
public:
    uiManager(QString fileName);
    ~uiManager(){}

    static const QString UiSuffix;
    static const QString UiFileType;

    //>@操作ui文件
    bool NewUI();
    bool OpenUI();
    bool CloseUI();
    bool DelUI();
    bool SaveUI();
    bool RenameUI(QString FileName);

    bool isValid();

    //>@操作文件中的元素
    void GetComponentGroup();
    void GetComInfoGroup();
    void GetComStructGroup();

    QRect GetDesktopRect();
    bool SetDesktopRect(QRect pRect);

    bool CreateEffect(EFFECT_INFO &pEffectInfo, QDomElement &pElement);

    bool AddChildDom(QDomElement pParentElement, QString pType, double pAttr, QString pText);
    bool AddChildDom(QDomElement pParentElement, QString pType, QString pAttr, QString pText);
    bool ModifyChildDom(QDomElement pParentElement, QString pType, double pAttr, QString pText);

    bool AddPosDom(QDomElement parentElement, QRectF pRect);
    bool ModifyPosDom(QDomElement parentElement, QRectF pRect);
    bool DelPosDom(QDomElement parentElement);
    bool AddStateDom(QDomElement parentElement, STATE_INFO* pState);
    bool ModifyStateDom(QDomElement parentElement, STATE_INFO* pState);
    bool DelStateDom(QDomElement parentElement, STATE_INFO* pState);

    //>@在Dom中添加一个控件的Dom资源，如果pOperateChild为真，则同步添加其子控件
    bool AddComDom(COM_INFO *pComInfo, bool pOperateChild);
    //>@在Dom中修改一个控件的Dom资源
    bool ModifyComDom(COM_INFO *pComInfo, bool pOperateChild);
    //>@在Dom中删除一个控件的Dom资源
    bool DeleteComDom(COM_INFO *pComInfo);

    bool CreateComDom(QDomElement pElement, COM_STRUCT *&pComInfo);
    bool CreateComDom(QDomElement pElement, COM_INFO *&pComInfo);
    bool CreateComDom(QString ComPath, COM_INFO *&pComInfo);

    //>@将COM_INFO中的资源全部保存为相对路径，并存储在UI对应的文件夹下
    bool SaveRelativeComDom(COM_INFO *pComInfo);

    //>@查找文件的全路径
    FILEPATH_TYPE GetFullFilePath(QString& pFileName);
    //>@查看是否为相对路径
    bool IsRelativePath(QString pFileName);
    //>@规整资源到目标路径
    bool RegularFile(COM_TYPE pComType, RESOURCE_INFO &pRcInfo, QString pDstPath, QSize pSize);

    bool GetParentDomElement(QString ComPath, QDomElement& pElement);
    bool RegularizeUi();
    bool RegularizeSubui();
    bool RegularizeControl(QString ComPath);

public:
    QHash<QString, QDomElement>  m_ComElementGroup;    //>@xx.ui文件中的组件（subui和control）的Dom元素(xx.ui)
    QHash<QString, COM_INFO*>    m_ComInfoGroup;
    QHash<COM_TYPE, COM_STRUCT*> m_ComStructGroup;
};

class mdpManager : public xmlParse
{
public:
    mdpManager(QString fileName);
    ~mdpManager(){}

    static const QString MdpSuffix;
    static const QString MdpFileType;

    //>@操作文件
    bool NewMDP();
    bool OpenMDP();
    bool CloseMDP();
    bool DelMDP();
    bool SaveMDP();
    bool RenameMDP(QString FileName);

    //>@操作文件中的元素
    bool GetCfgparaGroup();

    bool SetNickName(QString pNickName);
    QString GetNickName();
    bool SetPassword(QString pPasswd);
    QString GetPassword();

    QDomElement AddDeviceDom(QString pPath, QDomElement pInitElement);
    void DelDeviceDom(QString pPath);
    void DelDeviceDom(PH_Type pType, QString pPath);
    void DelDeviceDom(LG_Type pType, QString pPath);
    QDomElement CreateParaDom(QDomElement pInitElement);
    bool ClrMDP();
    bool ClrDomCldNodes(QDomElement pElement);
    bool DelDom(QDomElement pElement);

public:
    QHash<QString, QDomElement> m_CfgparaElementGroup;
    QHash<PH_Type, QDomElement> m_PHElementGroup;
    QHash<LG_Type, QDomElement> m_LGElementGroup;
};

class devManager : public xmlParse
{
public:
    devManager(QString fileName);

    ~devManager(){}

    static const QString DevSuffix;
    static const QString DevFileType;

    //>@操作ui文件
    bool NewDEV();
    bool OpenDEV();
    bool CloseDEV();
    bool DelDEV();
    bool SaveDEV();
    bool RenameDEV(QString FileName);

    //>@操作文件中的元素
    bool GetDevGroup() {return true;}

public:
    QHash<PH_Type, QDomElement>   m_DevInfoGroup;
};

class stcManager : public xmlParse
{
public:
    stcManager(QString fileName);
    ~stcManager(){}

    static const QString StcSuffix;
    static const QString StcFileType;

    //>@操作ui文件
    bool NewSTC();
    bool OpenSTC();
    bool CloseSTC();
    bool DelSTC();
    bool SaveSTC();
    bool RenameSTC(QString FileName);

    //>@操作文件中的元素
    bool GetStcGroup();

    QHash<QString, QDomElement> m_CaseItemGroup;
};

class rcManager : public xmlParse
{
public:
    rcManager(QString fileName);

    ~rcManager(){}

    static const QString RcSuffix;
    static const QString RcFileType;

    //>@操作ui文件
    bool NewRC();
    bool OpenRC();
    bool CloseRC();
    bool DelRC();
    bool SaveRC();
    bool RenameRC(QString FileName);

    //>@操作文件中的元素
    bool GetRcGroup();
};

class cfgManager : public xmlParse
{
public:
    cfgManager(QString fileName);

    ~cfgManager(){}

    static const QString BootFileName;
    static const QString CfgFileType;

    bool getNewCategory();
    bool getSolutionCategory();
    bool getFileCategory();

    void getEffectAndRcTypes(QDomElement pElement, ComCategory *pComCategory);
    bool getComponetCategory();

    //>@ /Device/RMII
    bool getCfgparaGroup();

    bool getProtocolInterfaceGroup();
};

class logTipManager : public xmlParse
{
public:
    logTipManager(QString fileName);

    ~logTipManager(){}

    static const QString LogTipFileName;
    static const QString LogTipFileType;

    bool getNewLogTipCategory();
};

class logManager : public xmlParse
{
public:
    logManager(QString fileName);
    ~logManager(){}

    bool NewLog();
    bool OpenLog();
    bool CloseLog();
    bool SaveLog();
};

class cmdManager : public xmlParse
{
public:
    cmdManager();                   //>@解析XML格式的内容
    cmdManager(QString fileName);   //>@解析XML文件

    ~cmdManager(){}

public:
    //>@解析XML文件
    static const QString CmdSuffix;
    static const QString CmdFileType;

    //>@操作ui文件
    bool NewCMD();
    bool OpenCMD();
    bool CloseCMD();
    bool DelCMD();
    bool SaveCMD();
    bool RenameCMD(QString FileName);

    //>@操作文件中的元素
    bool GetCMDGroup();

    QHash<QString, QDomElement> m_CfgItemGroup;
};

class MEDIA_INFO
{
public:
    MEDIA_INFO()
    {
    }
    ~MEDIA_INFO()
    {
    }
    QString mFuncMap;
    QString mImage;
    QString mAudio;
    QString mVideo;
    QString mLogo;
    QString mText;
    MEDIA_INFO & operator=(const MEDIA_INFO &s)
    {
        mFuncMap = s.mFuncMap;
        mImage = s.mImage;
        mAudio = s.mAudio;
        mVideo = s.mVideo;
        mLogo = s.mLogo;
        mText = s.mText;
        return *this;
    }
    bool operator==(const MEDIA_INFO &s)
    {
        if(mFuncMap != s.mFuncMap)
            return false;
        if(mImage != s.mImage)
            return false;
        if(mAudio != s.mAudio)
            return false;
        if(mVideo != s.mVideo)
            return false;
        if(mLogo != s.mLogo)
            return false;
        if(mText != s.mText)
            return false;
        return true;
    }
};

typedef struct{
    QString    mEventValue;
    MEDIA_INFO mMediaInfo;
} MAPITEM_INFO;

class mapManager : public xmlParse
{
public:
    mapManager(QString fileName);

    ~mapManager(){}

    static const QString MapSuffix;
    static const QString MapFileType;

    QHash<QString, QDomElement> m_CfgparaElementGroup;
    QMultiHash<QString, MAPITEM_INFO*> m_MapItemInfoGroup;

    //>@操作map文件
    bool NewMAP();
    bool OpenMAP();
    bool CloseMAP();
    bool DelMAP();
    bool SaveMAP();
    bool RenameMAP(QString FileName);

    bool SetUIPath(QString UiPath);
    QString GetUIPath();
    bool GetCfgparaGroup();

    bool ClrMap();
    bool ClrDomCldNodes(QDomElement pElement);
    bool DelDom(QDomElement pElement);
    bool AddDomNewNode(DomInfo pDomInfo);

    bool MapItemToElement(MAPITEM_INFO *pMapItemInfo, QDomElement &pParentElement);
    MAPITEM_INFO * ElementToMapItem(QDomElement pElement);

    bool GetMapItemGroup();
    bool SaveMapItemGroup();

    bool AddMapItem(QString pEventName, QString pEventValue, MEDIA_INFO mMediaInfo);
    bool DelMapItem(QString pEventName, QString pEventValue);
    bool ModifyMapItem(QString pEventName, QString pEventValue, MEDIA_INFO mMediaInfo);
    MAPITEM_INFO *GetMapItem(QString pEventName, QString pEventValue);
};

class devInfoManager : public xmlParse
{
public:
    devInfoManager(QString fileName);
    ~devInfoManager(){}

    static const QString DevInfoSuffix;
    static const QString DevInfoFileType;

    //>@操作ui文件
    bool NewDevInfo();
    bool OpenDevInfo();
    bool CloseDevInfo();
    bool DelDevInfo();
    bool SaveDevInfo();
    bool RenameDevInfo(QString FileName);
};

#define D_FONTFAMILY  QString("family")
#define D_FONTSIZE    QString("size")
#define D_FONTCOLOR   QString("color")

class FONT_INFO
{
public:
    FONT_INFO()
    {
        mFont = QFont("Arial", 11, QFont::DemiBold, false/*italic*/);
        mColor = Qt::black;
        mValid = true;
    }
    FONT_INFO(QFont pFont, QColor pColor)
    {
        mFont = pFont;
        mColor = pColor;
        mValid = true;
    }
    FONT_INFO(QString pPara)
    {
        *this = pPara;
    }
    FONT_INFO & operator=(const FONT_INFO &s)
    {
        mFont = s.mFont;
        mColor = s.mColor;
        mValid = s.mValid;
        return *this;
    }
    bool isValid()
    {
        return mValid;
    }
    FONT_INFO & operator=(QString pPara)  /*family:Arial;size:11;color:0,0,0,0*/
    {
        if(pPara.isEmpty())
            mValid = false;
        mValid = true;
        QStringList tmpParaList = pPara.split(';');
        for(int i=0;i<tmpParaList.count();i++)
        {
            QString tmpString = tmpParaList.at(i);
            QStringList tmpChildParaList = tmpString.split(':');
            if(tmpChildParaList.count() != 2)
            {
                IDE_TRACE();
                mValid = false;
                continue;
            }
            QString tmpChildString = tmpChildParaList.at(0);
            if(tmpChildString.compare(D_FONTFAMILY, Qt::CaseInsensitive) == 0)
            {
                mFont.setFamily(tmpChildParaList.at(1));
            }
            else if(tmpChildString.compare(D_FONTSIZE, Qt::CaseInsensitive) == 0)
            {
                mFont.setPointSize(tmpChildParaList.at(1).toInt());
            }
            else if(tmpChildString.compare(D_FONTCOLOR, Qt::CaseInsensitive) == 0)
            {
                QString tmpColorString = tmpChildParaList.at(1);
                QStringList tmpColorItems = tmpColorString.split(',');
                if(tmpColorItems.count() == 3)
                    mColor.setRgb(tmpColorItems.at(0).toInt(),
                                  tmpColorItems.at(1).toInt(),
                                  tmpColorItems.at(2).toInt());
                else if(tmpColorItems.count() == 4)
                    mColor.setRgb(tmpColorItems.at(0).toInt(),
                                  tmpColorItems.at(1).toInt(),
                                  tmpColorItems.at(2).toInt(),
                                  tmpColorItems.at(3).toInt());
                else
                {
                    IDE_TRACE();
                    mValid = false;
                }
            }
        }
        return *this;
    }
    QString toString()
    {
        if(mValid == false)
            return QString();
        return QString("%1:%2;%3:%4;%5:%6,%7,%8,%9")
                      .arg(D_FONTFAMILY).arg(mFont.family())
                      .arg(D_FONTSIZE).arg(mFont.pointSize())
                      .arg(D_FONTCOLOR).arg(mColor.red()).arg(mColor.green()).arg(mColor.blue()).arg(mColor.alpha());
    }
    bool operator==(const FONT_INFO &s)
    {
        if(mFont != s.mFont)
            return false;
        if(mColor != s.mColor)
            return false;
        if(mValid != s.mValid)
            return false;
        return true;
    }
    QFont  mFont;
    QColor mColor;
    bool   mValid;
};

class fontManager : public xmlParse
{
public:
    fontManager(QString fileName);
    ~fontManager(){}

    static const QString FontSuffix;
    static const QString FontFileType;

    //>@操作ui文件
    bool NewFont();
    bool OpenFont();
    bool CloseFont();
    bool DelFont();
    bool SaveFont();
    bool RenameFont(QString FileName);

    bool GetParagraphGroup();

    bool AddParagraph(QString pDesc, QString pString, FONT_INFO pInfo);
    bool ModifyParagraph(QString pDesc, QString pString, FONT_INFO pInfo);
    bool DelParagraph(QString pDesc);

    bool GetParagraph(QString pDesc, QString &pString, FONT_INFO &pInfo);

    QHash<QString, QDomElement> m_ParagraphGroup;
};


#endif // GLOBAL_H

