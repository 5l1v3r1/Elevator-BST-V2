#ifndef LOGICCONTENTUPGRADES_H
#define LOGICCONTENTUPGRADES_H

#include "logicbase.h"
#include <QMultiHash>
#include "zipfun.h"

class GraphicsScene;

//>@内容更新操作只针对当前主题
//>@主题升级以及程序升级可任意

class LogicContentUpgrades : public LogicDevice
{
    Q_OBJECT
public:
    typedef enum{UpgradeNone=0, ExecCMD, UpgradeExe, UpgradeTheme, UpgradeArea} STORAGESTATE;
    typedef enum{NONE=0, CMD, EXE, THEME, AREALOGO, AREAVIDEO, AREAIMAGE, AREATEXT} UPGRADECONTENT;   //>@升级内容

    LogicContentUpgrades(QObject *parent = 0);
    ~LogicContentUpgrades();

    void SetSrcPath(QString pPath);
    void SetDstPath(QString pPath);

    GraphicsScene *GetThemeScene();

    bool copy(PhysicalDevice *pPHDev, QString pSrc, QString pDst);
    bool cut(PhysicalDevice *pPHDev, QString pSrc, QString pDst);
    bool del(PhysicalDevice *pPHDev, QString pPath);

    bool mkdir(PhysicalDevice *pPHDev, QString pPath);
    bool copydir(PhysicalDevice *pPHDev, QString pSrc, QString pDst, bool subfolder);
    bool deldir(PhysicalDevice *pPHDev, QString pDir);
    bool cleardir(PhysicalDevice *pPHDev, QString pDir);


    bool GetCmdList(QString pString);
    bool GetContentList(QString pDirPath, UPGRADECONTENT pContentType, QStringList pSuffix);
    bool GetContentList(QString pDirPath); //>@专门针对Theme，兼容zip以及文件夹模式
    STORAGESTATE StorageIsValid();

    bool ExecInstruc(QString pString);
    bool ExecCmdList(PhysicalDevice *pSrcPHDev, PhysicalDevice *pDstPHDev);
    bool UpgradeExeList(PhysicalDevice *pSrcPHDev, PhysicalDevice *pDstPHDev, QStringList pExeList, QString pDstPath, bool pBackup);
    bool UpgradeThemeList(PhysicalDevice *pSrcPHDev, PhysicalDevice *pDstPHDev, QStringList pThemeList, QString pDstPath, bool pBackup);
    bool UpgradeAreaList(PhysicalDevice *pSrcPHDev, PhysicalDevice *pDstPHDev, UPGRADECONTENT pContentType, QStringList pContentList, bool pBackup);

public:
    quint32 InitPara(); //>@返回处理的参数个数

public slots:
    void UpdatePara(QObject* pObject);
    void DeviceStateChanged();

public:
    bool                                    mBackStage;

    QString                                 m_SrcPath;
    QString                                 m_DstPath;
    QMultiHash<UPGRADECONTENT, QString>     m_UpgradeContentList;
};

#endif // PARSERUPGRADE_H
