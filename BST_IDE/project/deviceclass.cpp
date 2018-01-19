#include "deviceclass.h"
#include "treeitemclass.h"

BaseDevice::BaseDevice(QString pAddr, QString pBufferPath, QObject *parent) :
    QObject(parent)
{
    mDevTreeItem = 0;
    mDevState = S_OFFLINE;
    mDevType = TYPE_LOCAL;
    ChangeDevAddr(pAddr);
    ChangeBuffer(pBufferPath);
    mIsBusy = false;
}

BaseDevice::~BaseDevice()
{
}

void BaseDevice::SetDeviceTreeItem(DeviceTreeItem *pDevItem, DEVICESTATE pState)
{
    mDevTreeItem = pDevItem;
    if(mDevTreeItem)
        mDevTreeItem->SetState(pState);
}

bool BaseDevice::Scan()
{
    if(!mDevTreeItem)
        return false;
    //>@删除当前设备列表中的项目
    mDevTreeItem->mDelFileFlag = false;
    mDevTreeItem->Clear();
    //>@遍历缓冲目录中的可用工程
    mIsBusy = true;
    if(!ScanDirectiory(mBufferPath))
    {
        mIsBusy = false;
        return false;
    }
    mIsBusy = false;
    return true;
}

bool BaseDevice::ScanDirectiory(QString pPath)
{
    if(!mDevTreeItem)
        return false;
    if(pPath.isEmpty())
        return false;
    if(!pPath.endsWith('/'))
        pPath.append('/');
    QDir dir(pPath);
    if(!dir.exists())
    {
        IDE_TRACE();
        return false;
    }
    QFileInfoList fileInfoList = dir.entryInfoList();
    int dirCount = fileInfoList.count();
    int diff = 100 / ((dirCount==0)?100:dirCount);
    int i = 0;
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        GlobalPara.updateProgress(i++*diff);
        QtSleep(1);
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;
        if(fileInfo.isDir())
        {
            QString tmpFindDirPath = pPath + fileInfo.fileName();
            if(!tmpFindDirPath.endsWith('/'))
                tmpFindDirPath.append('/');
            QDir tmpDir(tmpFindDirPath);
            if(!dir.exists())
            {
                IDE_TRACE();
                continue;
            }
            QFileInfoList findFileInfoList = tmpDir.entryInfoList();
            foreach(QFileInfo findFileInfo, findFileInfoList)
            {
                if(findFileInfo.fileName() == "." || findFileInfo.fileName() == "..")
                    continue;
                if(findFileInfo.isFile())
                {
                    QString tmpFindFilePath = tmpFindDirPath + findFileInfo.fileName();
                    if(getFileType(tmpFindFilePath) == TYPE_PRO)
                    {
                        //>@添加一个工程
                        mDevTreeItem->AddProject(tmpFindFilePath);
                    }
                }
            }
        }
    }
    GlobalPara.updateProgress(0);
    return true;
}

void BaseDevice::ChangeDevAddr(QString pDevAddr)
{
    //>@查看是否相同
    if(mDevAddr.compare(pDevAddr) == 0)
        return;
    //>@删除已有状态信息
    Abort();
    mDevAddr = pDevAddr;
}

void BaseDevice::ChangeBuffer(QString pBufferPath)
{
    mBufferPath = pBufferPath;
    if(!mBufferPath.endsWith('/'))
        mBufferPath.append('/');
}

bool BaseDevice::Sync(quint32 pTimeout)
{
    Sync();
    while(pTimeout-=100)  //>@100ms监测一次
    {
        if(isSync())
            return true;
    }
    return false;
}

bool BaseDevice::Async(quint32 pTimeout)
{
    Async();
    while(pTimeout-=100)  //>@100ms监测一次
    {
        if(!isSync())
            return true;
    }
    return false;
}

bool BaseDevice::Abort()
{
    if(!mDevTreeItem)
        return false;
    mDevTreeItem->SetState(S_OFFLINE);
    mIsBusy = false;
    return true;
}

bool BaseDevice::LocalCompress(QString pSrc, QString pDst, bool pCover)
{
    QThread ZipThread;
    ZipTask compressZipTask;
    connect(this,SIGNAL(sZip(QString, QString, bool)),&compressZipTask,SLOT(Zip(QString, QString, bool)));
    connect(&compressZipTask,SIGNAL(end()),this,SLOT(slot_ZipTaskEnd()));
    connect(&compressZipTask,SIGNAL(error()),this,SLOT(slot_ZipTaskErr()));
    connect(&compressZipTask,SIGNAL(progress(int)),this,SLOT(slot_ZipTaskProgress(int)));
    compressZipTask.moveToThread(&ZipThread);
    ZipThread.start(QThread::LowPriority);
    ZipThread.wait(10);
    mZipTaskState = Start;
    emit sZip(pSrc, pDst, pCover);
    while(mZipTaskState == Start)
        QtSleep(1);
    if(mZipTaskState == Success)
        return true;
    return false;
}
bool BaseDevice::LocalDecompress(QString pSrc, QString pDir, bool pCover)
{
    QThread ZipThread;
    ZipTask decompressZipTask;
    connect(this,SIGNAL(sUnzip(QString, QString, bool)),&decompressZipTask,SLOT(Unzip(QString, QString, bool)));
    connect(&decompressZipTask,SIGNAL(end()),this,SLOT(slot_ZipTaskEnd()));
    connect(&decompressZipTask,SIGNAL(error()),this,SLOT(slot_ZipTaskErr()));
    connect(&decompressZipTask,SIGNAL(progress(int)),this,SLOT(slot_ZipTaskProgress(int)));
    decompressZipTask.moveToThread(&ZipThread);
    ZipThread.start(QThread::LowPriority);
    ZipThread.wait(10);
    mZipTaskState = Start;
    emit sUnzip(pSrc, pDir, pCover);
    while(mZipTaskState == Start)
        QtSleep(1);
    if(mZipTaskState == Success)
        return true;
    return false;
}

void BaseDevice::slot_ZipTaskEnd()
{
    mZipTaskState = Success;
}

void BaseDevice::slot_ZipTaskErr()
{
    mZipTaskState = Failed;
}

void BaseDevice::slot_ZipTaskProgress(int ratio)
{
#ifdef IDE
    GlobalPara.updateProgress(QString("ZipTaskProgress"), ratio);
#endif
}


NetDevice::NetDevice(QString pAddr, QString pBufferPath, QObject *parent) :
    BaseDevice(pAddr, pBufferPath, parent)
{
    mDevType = TYPE_NET;
    mTcpClient = 0;
    mNetManager = 0;
    ChangeDevAddr(pAddr);
}

NetDevice::~NetDevice()
{
    Abort();
}

void NetDevice::UpdateDevAddr()
{
    if(!mTcpClient)
        return;
    mDevAddr = QString("%1:%2").arg(mTcpClient->GetPeerIP().toString()).arg(mTcpClient->GetPeerPort());
}

bool NetDevice::Scan()
{
    if(!BaseDevice::Scan())
        return false;
    if(isSync())
    {
        QString tmpThemeInfo = mNetManager->m_DevInfoList.value("ThemeManager");
        QStringList tmpThemeParas = tmpThemeInfo.split(',');
        if(tmpThemeParas.count() < 1)
            return false;
        QString tmpThemePath = tmpThemeParas.at(0);
        if(tmpThemePath.endsWith('/'))
            tmpThemePath.append('/');
        for(int i=1;i<tmpThemeParas.count();i++)
        {
            QString tmpThemeName = tmpThemeParas.at(i);
            //>@添加一个工程
            QString tmpProjectPath = tmpThemePath + tmpThemeName + QString("/") + tmpThemeName + QString(".bstpro");
            mDevTreeItem->AddProject(tmpProjectPath);
        }
    }
    return true;
}

bool NetDevice::isSync()
{
    if(!(mTcpClient && mNetManager))
        return false;
    if(!mTcpClient->isConnected())
        return false;
    return mNetManager->m_SFConn;
}

void NetDevice::Sync()
{
    if(!mTcpClient)
    {
        mTcpClient = new NetManagerSocket(this);
        connect(mTcpClient, SIGNAL(sConnState(bool)), this, SLOT(slot_PHConnState(bool)));
    }
    if(!mNetManager)
    {
        mNetManager = new HostNetManager(this);
        mNetManager->SyncDev(mTcpClient);
        connect(mTcpClient, SIGNAL(readyRead()), mNetManager, SLOT(ParseDataStream()));
        connect(mNetManager, SIGNAL(sConnState(bool)), this, SLOT(slot_LGConnState(bool)));
        connect(mNetManager, SIGNAL(sProgress(int)), &GlobalPara, SLOT(updateProgress(int)));
    }
    if(!mTcpClient->isConnected())
    {
        QHostAddress tmpIP;
        quint16 tmpPort;
        if(!ParseIP(mDevAddr, tmpIP, tmpPort))
        {
            IDE_TRACE();
            return;
        }
        if(tmpPort == 0)
            tmpPort = D_NMPORT;
        mTcpClient->Open(tmpIP, tmpPort);
    }
    else
    {
        mNetManager->SFConn();
    }
}

void NetDevice::Async()
{
    if(isSync())
    {
        mNetManager->SFDisconn();
    }
}

bool NetDevice::Abort()
{
    if(mNetManager)
    {
        mNetManager->deleteLater();
        mNetManager = 0;
    }
    if(mTcpClient)
    {
        mTcpClient->deleteLater();
        mTcpClient = 0;
    }
    BaseDevice::Abort();
    emit sConnState(false);
    return true;
}

void NetDevice::slot_PHConnState(bool pState)
{
    if(pState)
    {
        if(mNetManager)
            mNetManager->SFConn();
    }
    else
    {
        if(mNetManager)
            mNetManager->m_SFConn = false;
        if(mDevTreeItem)
            mDevTreeItem->SetState(S_ONLINE);
        emit sConnState(false);
    }
}

void NetDevice::slot_LGConnState(bool pState)
{
    if(pState)
    {
        if(mDevTreeItem)
        {
            mDevTreeItem->SetState(S_DEVSYNC);
            emit sConnState(false);
        }
    }
    else
    {
        if(mDevTreeItem)
        {
            mDevTreeItem->SetState(S_ONLINE);
            emit sConnState(false);
        }
    }
}

bool NetDevice::Download(QString pThemeName)
{
    if(!isSync())
        return false;
    QString tmpThemeInfo = mNetManager->m_DevInfoList.value("ThemeManager");
    QStringList tmpThemeParas = tmpThemeInfo.split(',');
    if(tmpThemeParas.count() < 1)
        return false;
    mIsBusy = true;
    //>@下载到当前工程此主题的OutPut目录下
    QString tmpSrcPath = tmpThemeParas.at(0)+pThemeName;
    QString tmpDstPath = QString("%1%2/%3").arg(mBufferPath).arg(pThemeName).arg(OUTPUTDIR);
    if(mNetManager->DownLoadThemeZip(tmpSrcPath, tmpDstPath, 600000))
    {
        QString src = QString("%1%2.zip").arg(tmpDstPath).arg(pThemeName);
        QString dst = QString("%1%2/").arg(mBufferPath).arg(pThemeName);
        if(LocalDecompress(src, dst, true))
        {
            mIsBusy = false;
            return true;
        }
    }
    mIsBusy = false;
    return false;
}

//>@如果系统已经存在同名主题，则覆盖，否则直接上传
bool NetDevice::Upload(QString pThemeName)
{
    if(!isSync())
        return false;
    QString tmpThemeInfo = mNetManager->m_DevInfoList.value("ThemeManager");
    QStringList tmpThemeParas = tmpThemeInfo.split(',');
    if(tmpThemeParas.count() < 1)
        return false;
    mIsBusy = true;
    //>@首先查看是否有此主题的zip文件，如果没有则先压缩
    QString tmpSrcDir = QString("%1%2/%3/%4").arg(mBufferPath).arg(pThemeName).arg(OUTPUTDIR).arg(pThemeName);
    QString tmpSrcZip = tmpSrcDir + QString(".zip");
    if(!isFileExist(tmpSrcZip))
    {
        if(!LocalCompress(tmpSrcDir, tmpSrcZip, true))
        {
            mIsBusy = false;
            return false;
        }
    }
    QString tmpDstThemePath = tmpThemeParas.at(0) + pThemeName;
    if(mNetManager->UpLoadThemeZip(tmpSrcZip, tmpDstThemePath, 600000))
    {
        mIsBusy = false;
        return true;
    }
    mIsBusy = false;
    return false;
}

bool NetDevice::DownloadConfigure(QString pThemeName, QString pMdpName)
{
    if(!isSync())
        return false;
    QString tmpThemeInfo = mNetManager->m_DevInfoList.value("FSRC");
    QStringList tmpThemeParas = tmpThemeInfo.split(',');
    if(tmpThemeParas.count() < 1)
        return false;
    mIsBusy = true;
    //>@下载到当前工程此主题目录下
    QString tmpSrcPath = tmpThemeParas.at(0)+D_BOOTDIR+pMdpName;
    QString tmpDstPath = QString("%1%2/%3").arg(mBufferPath).arg(pThemeName).arg(pMdpName);
    if(mNetManager->GetFile("y", tmpSrcPath, tmpDstPath, 1000))
    {
        mIsBusy = false;
        return true;
    }
    mIsBusy = false;
    return false;
}

bool NetDevice::UploadConfigure(QString pThemeName, QString pMdpName)
{
    if(pThemeName.isEmpty() || pMdpName.isEmpty())
        return false;
    if(!isSync())
        return false;
    QString tmpThemeInfo = mNetManager->m_DevInfoList.value("FSRC");
    QStringList tmpThemeParas = tmpThemeInfo.split(',');
    if(tmpThemeParas.count() < 1)
        return false;
    //>@首先查看是否有mdp文件
    QString tmpSrcFile = QString("%1%2/%3").arg(mBufferPath).arg(pThemeName).arg(pMdpName);
    if(isFileExist(tmpSrcFile))
        return false;
    mIsBusy = true;
    QString tmpDstFile = tmpThemeParas.at(0) + D_BOOTDIR + pThemeName;
    if(mNetManager->SendFile("y", tmpSrcFile, tmpDstFile, 1000))
    {
        mIsBusy = false;
        return true;
    }
    mIsBusy = false;
    return false;
}

bool NetDevice::ParseIP(QString pIP, QHostAddress &pIPAddr, quint16 &pPort)
{
    QStringList tmpList = pIP.split(':');
    if(tmpList.count() < 1)
        return false;
    QHostAddress tmpAddr(tmpList.at(0));
    if(tmpAddr.isNull())
        return false;
    if(tmpList.count() == 2)
    {
        bool ok=false;
        quint16 tmpPort = tmpList.at(1).toUShort(&ok, 10);
        if(!ok)
            tmpPort = 0;
        pPort = tmpPort;
    }
    pIPAddr = tmpAddr;
    return true;
}

bool NetDevice::CheckIP(QString pIP)
{
    QHostAddress tmpIP;
    quint16 tmpPort;
    return ParseIP(pIP, tmpIP, tmpPort);
}

StorageDevice::StorageDevice(QString pAddr, QString pBufferPath, QObject *parent) :
    BaseDevice(pAddr, pBufferPath, parent)
{
    mDevType = TYPE_STORAGE;
    mDevAddr = pAddr;
}

StorageDevice::~StorageDevice()
{

}

bool StorageDevice::Scan()
{
    if(!BaseDevice::Scan())
        return false;
    if(isSync())
    {
        QString tmpDevThemePath = mDevAddr+D_THEMEDIR;
        QDir dir(tmpDevThemePath);
        if(!dir.exists())
        {
            IDE_TRACE();
            return false;
        }
        QFileInfoList fileInfoList = dir.entryInfoList();
        int dirCount = fileInfoList.count();
        int diff = 100 / ((dirCount==0)?100:dirCount);
        int i = 0;
        foreach(QFileInfo fileInfo, fileInfoList)
        {
            GlobalPara.updateProgress(i++*diff);
            QtSleep(1);
            if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
                continue;
            if(fileInfo.isDir())
            {
                QString tmpDirName = fileInfo.fileName();
                QString tmpProjectPath = tmpDevThemePath + tmpDirName + QString("/") + tmpDirName + QString(".bstpro");
                mDevTreeItem->AddProject(tmpProjectPath);
            }
        }
        GlobalPara.updateProgress(0);
        return true;
    }
    return true;
}

bool StorageDevice::isSync()
{
    return (mDevState == S_DEVSYNC);
}

void StorageDevice::Sync()
{
    if(isFileExist(mDevAddr))
    {
        mDevTreeItem->SetState(S_DEVSYNC);
    }
}

void StorageDevice::Async()
{
    if(isSync())
        mDevTreeItem->SetState(S_ONLINE);
}

bool StorageDevice::Download(QString pThemeName)
{
    if(!isSync())
        return false;
    mIsBusy = true;
    //>@下载到当前工程目录下
    QString tmpSrcPath = mDevAddr + D_THEMEDIR + pThemeName;
    QString tmpDstPath = QString("%1%2/").arg(mBufferPath).arg(pThemeName);
    if(CopyFolder(tmpSrcPath, tmpDstPath, true))
    {
        mIsBusy = false;
        return true;
    }
    mIsBusy = false;
    return false;
}

//>@如果系统已经存在同名主题，则覆盖，否则直接上传
bool StorageDevice::Upload(QString pThemeName)
{
    if(!isSync())
        return false;
    mIsBusy = true;
    //>@首先查看Output中是否有此主题的文件夹
    QString tmpSrcPath = QString("%1%2/%3%4").arg(mBufferPath).arg(pThemeName).arg(OUTPUTDIR).arg(pThemeName);
    QDir tmpSrcDir(tmpSrcPath);
    if(!tmpSrcDir.exists())
    {
        mIsBusy = false;
        return false;
    }
    QString tmpDstPath = mDevAddr + D_THEMEDIR + pThemeName;
    if(CopyFolder(tmpSrcPath, tmpDstPath, true))
    {
        mIsBusy = false;
        return true;
    }
    mIsBusy = false;
    return false;
}

bool StorageDevice::DownloadConfigure(QString pThemeName, QString pMdpName)
{
    if(!isSync())
        return false;
    mIsBusy = true;
    //>@下载到当前工程目录下
    QString tmpSrcPath = mDevAddr + D_BOOTDIR + pMdpName;
    QString tmpDstPath = QString("%1%2/%3").arg(mBufferPath).arg(pThemeName).arg(pMdpName);
    if(CopyFile(tmpSrcPath, tmpDstPath))
    {
        mIsBusy = false;
        return true;
    }
    mIsBusy = false;
    return false;
}

bool StorageDevice::UploadConfigure(QString pThemeName, QString pMdpName)
{
    if(!isSync())
        return false;
    mIsBusy = true;
    QString tmpSrcPath = QString("%1%2/%3").arg(mBufferPath).arg(pThemeName).arg(pMdpName);
    QString tmpDstPath = mDevAddr + D_BOOTDIR + pMdpName;
    if(CopyFile(tmpSrcPath, tmpDstPath))
    {
        mIsBusy = false;
        return true;
    }
    mIsBusy = false;
    return false;
}


ComDevice::ComDevice(QString pAddr, QString pBufferPath, QObject *parent) :
    BaseDevice(pAddr, pBufferPath, parent)
{
    mDevType = TYPE_COM;
}

ComDevice::~ComDevice()
{

}

bool ComDevice::Scan()
{
    return false;
}

bool ComDevice::isSync()
{
    return false;
}

void ComDevice::Sync()
{

}

void ComDevice::Async()
{

}

bool ComDevice::Download(QString pThemeName)
{
    return false;
}

bool ComDevice::Upload(QString pThemeName)
{
    return false;
}
