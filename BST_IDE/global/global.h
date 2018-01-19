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

extern Global GlobalPara;

void Logger(QString Key = QString(), QStringList tmplist = QStringList());

void GetApplicationDirectory();
QString GetHostPCInfo();
bool GetScreenInfo();
bool InitLoggerInfo();
bool GetBootConfigure();
bool InitializeLinguist();

#ifdef DISPLAY
    class DeviceManager;
#endif

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
    bool                DoorOpenEnable;

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

/*
    <Desktop>
        <Rect>
            <Horizontal>0,0,800,600</Horizontal>
            <Vertical>0,0,600,800</Vertical>
        </Rect>
        <StateMachine>
            <Common EffectType="Image" EffectPara="">
                <RC></RC>
            </Common>
            <Change EffectType="Roll" EffectPara="">
                <RC></RC>
            </Change>
        </StateMachine>
    </Desktop>

    横竖显自动布局算法： 除桌面外，对于具有图像元素的组件必须保证宽高相同，修改坐标时需要实时判断是否宽高超出横竖版桌面的宽高
    1、竖屏自动布局原则：
        如果所有有区域的左上角坐标都为0，则表示从未自动布局过，则进行一次自动布局；否则不对区域进行自动布局
            所有区域进行列排放，优先级取决于COM_TYPE的大小
        区域内部的组件进行多行排放，也即排完第一行再排第二行的原则，优先级取决于COM_TYPE的大小
            通过二维数组记录
            一行最多有三个组件，每一行都是平均分配空隙（除因子为行组件个数+1），
            在列方面，每个组件在自己所在的行中要居中
        最终由区域内部已排列的组件计算区域的最小大小，如果区域的大小大于此最小大小，则不变，否则区域最终的坐标中高度使用最小高度，宽度使用桌面宽度

    2、横屏自动布局原则
        从不对区域进行自动布局，完全由用户进行拖放设置，因此一般设计界面时先设计横版，再设计竖版
        区域内部的组件进行多行排放，也即排完第一行再排第二行的原则，优先级取决于COM_TYPE的大小
        最终由区域内部已排列的组件计算区域的最小大小，如果区域的大小大于此最小大小，则不变，否则区域最终的坐标中高度使用最小高度，宽带使用最小宽度

    3、拖动组件，同步原则
        对于区域，不需要进行任何检测
        对于组件，需要实时检测其宽度或者高度，查看其所在方位是否已经超出当前横竖版的桌面，如果是，则提示注意，如果执意修改，则锁定当前主题的横竖切换功能使之仅能适应一种界面

    4、界面初始化调整区域原则
        首先针查看自己所有的子控件是否在区域外，如果是，则自动调整到子控件的最大大小，
        然后查看区域是否在桌面外，如果是，则调整到桌面内

*/
inline QRectF ToSceneRect(QRectF pRect)
{
#ifdef IDE
    return QRectF(pRect.left() + StartPosX, pRect.top() + StartPosY, pRect.width(), pRect.height());
#elif DISPLAY
    return pRect;
#endif
}
inline QRectF FromSceneRect(QRectF pRect)
{
#ifdef IDE
    return QRectF(pRect.left() - StartPosX, pRect.top() - StartPosY, pRect.width(), pRect.height());
#elif DISPLAY
    return pRect;
#endif
}

#define D_RCINVALID      QString("Invalid")
#define D_EFFECTTYPE     QString("EffectType")
#define D_EFFECTPARA     QString("EffectPara")

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

    QString GetSolution();
    bool SetSolution(QString pSolution);
    DIRECTION GetDirectionPermisson();
    bool SetDirectionPermisson(DIRECTION pDirection);
    bool canSwitch() { return (m_DirectionPermisson == HVBoth); }

    //>@操作文件中的元素
    void GetComponentGroup();

    //>@查找文件的全路径
    FILEPATH_TYPE GetRcFileType(QString pFileName);
    FILEPATH_TYPE RegulatRcFile(QString& pFileName);
    //>@查看是否为相对路径
    bool IsRelativePath(QString pFileName);
    //>@规整资源到目标路径
    bool RegularFile(COM_TYPE pComType, QString pRcName, QString &pRcFile, QString pDstPath, QSize pSize);

    QRect GetComRect(QDomElement pComElement, DIRECTION pDirection);
    bool ModifyComRect(QDomElement pComElement, QRect pRect, DIRECTION pDirection);

    QDomNodeList GetComStateList(QDomElement pComElement);
    QDomElement GetComState(QDomElement pComElement, QString pStateName);
    bool ModifyEffectType(QDomElement pStateElement, QString pEffectType);
    QDomElement ModifyComState(QDomElement pComElement, QString pStateName, QString pEffectType, QString pEffectPara);

    //>@获取某个组件的父QDomElement，同时查看是否为规整的资源路径，否则进行创建此路径
    bool GetParentElement(QDomElement &pParentElement, QString pComPath);
    //>@获取组件的父组件
    QDomElement GetParComDom(QDomElement pComElement);
    QDomElement GetParComDom(QString pComPath);

    //>@添加一个控件的Dom资源
    QDomElement AddComDom(COM_TYPE pComType, QRect pRect, DIRECTION pDirection);
    //>@在Dom中删除一个控件的Dom资源
    bool DeleteComDom(COM_TYPE pComType);

    //>@将资源全部保存为相对路径，并存储在UI对应的文件夹下
    bool SaveRelativeComDom(QDomElement pComElement);

public:
    //>@xx.ui文件中的组件（subui和control）的Dom元素(xx.ui)
    QHash<COM_TYPE, QDomElement>  m_ComElementGroup;
    DIRECTION                     m_DirectionPermisson;
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

