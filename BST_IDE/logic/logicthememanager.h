#ifndef LOGICTHEMEMANAGER_H
#define LOGICTHEMEMANAGER_H

#include <QGraphicsView>
#include "zipfun.h"
#include "eventmap.h"
#include "graphicsinc.h"
#include "logicbase.h"

typedef enum{
    STATE_NONE = 0,
    STATE_ITEMUI,      //>@一级设置菜单
    STATE_CTRLUI,      //>@二级设置菜单
    STATE_FOCUSUI      //>@
} MANUAL_STATE;

typedef enum{
    KEY_SET = 0,
    KEY_UP,
    KEY_DN,
    KEY_BACK
} KEY_OPERATE;

typedef enum{
    RenderQPainter=0,
    RenderOpenGL,
    RenderGDI,
    RenderDriect2D,
    RenderXV
} RenderDev;

class LogicThemeManager;

class UiScene : public GraphicsScene
{
    Q_OBJECT
public:
    UiScene(QObject *parent=0);
    ~UiScene();

    bool Initialize(QString pFileName);

    //>@界面函数
    bool InitUI();
    bool PlayUI();
    bool PauseUI();

    bool AddComponent(COM_TYPE pComType, QDomElement &pComElement, QPointF pScenePos=QPointF());
    bool RemoveComponent(COM_TYPE pComType);

    void UpdateProgress(QString pMsg, int pValue);

    void KeyProcess(KEY_OPERATE pKey);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void sUpdateProgress(quint32 pValue);
    void sShowInfo(QString pMsg, int pDelay);
    void sShowWarn(QString pMsg, int pDelay);
    void sShowError(QString pMsg, int pDelay);
    void sBeep(QString);

    //>@基础函数
public slots:
    void ShowMessage(QString Key, int pDelay=1000);
    void UpdateProgress(quint32 pValue);

public:
#ifdef DISPLAY
    LogicThemeManager  *m_LogicThemeManager;
#endif
    QHash<COM_TYPE, GraphicsOperate*>   m_ComDiagramGroup;

    EventMap           *m_EventMap;

    //>@标识符
    MANUAL_STATE        m_ManualState;
    //>@XML解析类
    iniParse            *m_IniParse;
    //>@应用协议类
    //>@控件
    GraphicsSetBar      *m_SetBar;
    GraphicsMessage     *m_MessageBar;
    GraphicsProgress    *m_ProgressBar;
    GraphicsKeyboard    *m_Keyboard;
    GraphicsDesktop     *m_Desktop;
};

class UiView : public QGraphicsView
{
    Q_OBJECT
public:
    UiView(LogicThemeManager *pLogicDev, RenderDev pRenderDev=RenderQPainter, QWidget *parent = 0);
    ~UiView();
    void SetRenderDev(RenderDev pRenderDev);
    void keyPressEvent(QKeyEvent *event);
public:
    UiScene            *mScene;
};

class LogicThemeManager : public LogicDevice
{
    Q_OBJECT
public:
    LogicThemeManager(QObject *parent = 0);
    ~LogicThemeManager();

    RenderDev GetRenderDev(QString pMode)
    {
        if(pMode.compare("QPainter", Qt::CaseInsensitive) == 0)
            return RenderQPainter;
        else if(pMode.compare("OpenGL", Qt::CaseInsensitive) == 0)
            return RenderOpenGL;
        else if(pMode.compare("GDI+", Qt::CaseInsensitive) == 0)
            return RenderGDI;
        else if(pMode.compare("Driect2D", Qt::CaseInsensitive) == 0)
            return RenderDriect2D;
        else if(pMode.compare("XV", Qt::CaseInsensitive) == 0)
            return RenderXV;
    }
    QString GetRenderDevString(RenderDev pMode)
    {
        if(pMode == RenderQPainter)
            return "QPainter";
        else if(pMode == RenderOpenGL)
            return "OpenGL";
        else if(pMode == RenderGDI)
            return "GDI+";
        else if(pMode == RenderDriect2D)
            return "Driect2D";
        else if(pMode == RenderXV)
            return "XV";
    }

    quint32 InitPara(); //>@返回处理的参数个数

    QStringList SearchThemes(QString pDirPath);
    void UpdateThemeList();

    //>@返回主题文件夹路径、活动主题以及所有主题名
    QString GetInformation()
    {
        if(m_UiView && m_UiView->mScene)
        {
            //>@遍历所有主题
            QString tmpThemeNameString;
            for (int i=0; i<mThemeNameList.size(); ++i)
            {
                QString tmpThemeName = mThemeNameList.at(i);
                tmpThemeNameString.append(',');
                tmpThemeNameString.append(tmpThemeName);
            }
            return QString("%1:%2%3").arg(mDevName).arg(mThemesPath).arg(tmpThemeNameString);
        }
        return QString("%1:").arg(mDevName);
    }

public:
    void ReleaseImageBuffer() {} //>@释放主题中的图片缓存
    void CloseVideo() {}
    void RestoreVideo() {}
    void EnableEffect() {}
    void DisableEffect() {}

public slots:
    void UpdatePara(QObject* pObject);
    void DeviceStateChanged();

    void slot_ProcessConnect();

    void InitTheme();
    void ReleaseTheme();
    void StartTheme();
    void PauseTheme();
    void ResumeTheme();
    void ShowTheme();

    bool ModifyCurTheme(QString pThemeName) {return true;}

public:
    UiView             *m_UiView;

    RANKS               mLocation;
    DIRECTION           mDirection;
    RenderDev           mRenderDev;
    QString             mThemesPath;        //>@主题包所在的路径
    QStringList         mThemeNameList;     //>@主题包所在的路径下的所有主题文件夹名
    QString             mActiveTheme;       //>@当前活动主题文件夹名
    QString             mActiveThemePath;   //>@当前活动主题文件的全路径
};

#endif // PARSERTHEMEOP_H
