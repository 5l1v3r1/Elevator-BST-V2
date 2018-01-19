#ifndef LOGICBASE_H
#define LOGICBASE_H

#include "iodevicebase.h"
#include "physicalbase.h"

#define D_CMDSUBSPLIT  QString("#")

class LogicDevice : public IODeviceBase
{
    Q_OBJECT
public:
    LogicDevice(QObject *parent = 0);
    ~LogicDevice();

    bool InitDevice(LG_Type pLGType, QDomElement pElement);

    virtual QStringList AppendPHDevices(QStringList pPHDevs);
    QStringList PHDevicesAdded(QStringList pList, CheckList *pComponent);
    void PHDeviceDeleted(PhysicalDevice *pPHDev, bool pSync=true);
    virtual void DeletePHDevices();
    void SetPHDeviceEditble(bool flag);

public slots:
    virtual void ParseDataStream(){ IDE_TRACE(); }
    virtual void DeviceStateChanged() { IDE_TRACE(); }

signals:
    void sPHDeviceChanged();

public:
    QHash<PH_Type, PhysicalDevice*> mPHDevices;       //>@协议类型节点表
    LG_Type                         mDevType;
};

class EventInfo
{
public:
    EventInfo()
    {
        mFatherType =  Event_Unknow;
        mRatio = 1.0;
        mEndFlag = true;
        mEventType = Event_Unknow;
        mEventPara.clear();
        mEventDietime = 0;
    }
    ~EventInfo()
    {
        mEventPara.clear();
    }
    bool isSuccess()
    {
        if(mEventPara.isEmpty())
            return false;
        QString report = mEventPara.at(mEventPara.count()-1);
        if(report.compare(D_Success, Qt::CaseInsensitive) == 0)
            return true;
        return false;
    }
    bool operator&(const EventInfo &s) const
    {
        if(mEventType != s.mEventType)
            return false;
        quint8 paraCnt = qMin(mEventPara.count(), s.mEventPara.count());
        for(int i=0;i<paraCnt;i++)
        {
            if(mEventPara.at(i).compare(s.mEventPara.at(i), Qt::CaseInsensitive) != 0)
                return false;
        }
        return true;
    }
    bool operator==(const EventInfo &s) const
    {
        if((mFatherType == s.mFatherType)&&
           (mRatio == s.mRatio)&&
           (mEndFlag == s.mEndFlag)&&
           (mEventType == s.mEventType)&&
           (mEventPara == s.mEventPara)&&
           (mEventDietime == s.mEventDietime))
            return true;
        return false;
    }
    bool operator!=(const EventInfo &s) const{return !operator==(s);}
    EventInfo & operator=(const EventInfo &s)
    {
        mFatherType = s.mFatherType;
        mRatio = s.mRatio;
        mEndFlag = s.mEndFlag;
        mEventType = s.mEventType;
        mEventPara = s.mEventPara;
        mEventDietime = s.mEventDietime;
        return *this;
    }
    bool fromString(QString pString)
    {
        QStringList tmpList = pString.split(D_CMDSUBSPLIT);
        if(tmpList.isEmpty())
            return false;
        mEventType = StrToEventType(tmpList.at(0));
        for(int i=1;i<tmpList.count();i++)
            mEventPara.append(tmpList.at(i));
        return true;
    }
    QString toFrame()
    {
        QString frame = EventTypeToStr(mEventType);
        for(int i=0;i<mEventPara.count();i++)
        {
            frame.append(D_CMDSUBSPLIT);
            frame.append(mEventPara.at(i));
        }
        return frame;
    }
    QString toContent()
    {
        QString frame = mEventPara.at(0);
        for(int i=1;i<mEventPara.count();i++)
        {
            frame.append(D_CMDSUBSPLIT);
            frame.append(mEventPara.at(i));
        }
        return frame;
    }
public:
    EVENT_TYPE      mFatherType;
    qreal           mRatio;
    bool            mEndFlag;
    EVENT_TYPE      mEventType;
    QStringList     mEventPara;
    quint32         mEventDietime;
};

class ReadFileOp :public QObject
{
    Q_OBJECT
public:
    ReadFileOp(QObject *parent = 0)
    {
        mPath.clear();
        mSize = mLeftSize =0;
    }
    ~ReadFileOp()
    {
        mPath.clear();
        mSize = mLeftSize =0;
        close();
    }

    void savePath(QString pSrc)
    {
        mPath =  pSrc;
    }
    bool isExist(QString pSrc)
    {
        close();
        mFile.setFileName(pSrc);
        return (mFile.exists());
    }
    bool open(QString pSrc)
    {
        close();
        mFile.setFileName(pSrc);
        if(!mFile.open(QFile::ReadOnly))
            return false;
        mSize = size();
        mLeftSize = mSize;
        return true;
    }
    bool read(qint64 maxlen, QByteArray &byteArray)
    {
        byteArray.clear();
        if(!mFile.isOpen())
            return false;
        byteArray = mFile.read(maxlen);
        return true;
    }
    void close()
    {
        if(mFile.isOpen())
            mFile.close();
    }
private:
    qint64 size()
    {
        return mFile.size();
    }
public:
    QFile       mFile;
    QString     mPath;
    qint64      mSize;
    qint64      mLeftSize;
};

class WriteFileOp :public QObject
{
    Q_OBJECT
public:
    WriteFileOp(QObject *parent = 0)
    {
        mPath.clear();
        mSize = mRcvSize =0;
    }
    ~WriteFileOp()
    {
        mPath.clear();
        mSize = mRcvSize =0;
        close();
    }

    void savePath(QString pSrc)
    {
        mPath =  pSrc;
    }
    bool isExist(QString pSrc)
    {
        close();
        mFile.setFileName(pSrc);
        return (mFile.exists());
    }
    bool isPathExist(QString pSrc)
    {
        close();
        QString path = getFileDirectory(pSrc);
        QDir dir;
        return dir.mkpath(path);
    }
    bool open(QString pSrc)
    {
        close();
        mFile.setFileName(pSrc);
        if(!mFile.open(QFile::WriteOnly))
        {
            if(!mFile.setPermissions(QFile::WriteOther))
                return false;
            if(!mFile.open(QFile::WriteOnly))
                return false;
            return true;
        }
        return true;
    }
    bool write(QByteArray pByteArray)
    {
        if(!mFile.isOpen())
            return false;
        mFile.write(pByteArray);
        mFile.flush();
        return true;
    }
    void close()
    {
        if(mFile.isOpen())
            mFile.close();
    }
private:
    qint64 size()
    {
        return mFile.size();
    }
public:
    QFile       mFile;
    QString     mPath;
    bool        mEnable;
    qint64      mSize;
    qint64      mRcvSize;
};
/*************************************************************
//>@GlobalFun
**************************************************************/
inline bool StrToLongLong(QString pStr, qint64 &pLongLong)
{
    bool ok = false;
    pLongLong = pStr.toLongLong(&ok,10);
    if(!ok)
        return false;
    return true;
}

inline bool CheckCapacity(QString path, quint64 pSize)
{
    return true;
}

inline bool PrintSence(QString pPath)
{
    if(pPath.isEmpty())
        return false;
    QPixmap Screen = QPixmap::grabWindow
            (QApplication::desktop()->winId(),
             0,
             0,
             QApplication::desktop()->size().width(),
             QApplication::desktop()->size().height());
    if(Screen.save(pPath,"JPG"))
        return true;
    return false;
}

inline QByteArray PrintSence()
{
    QPixmap Screen = QPixmap::grabWindow
            (QApplication::desktop()->winId(),
             0,
             0,
             QApplication::desktop()->size().width(),
             QApplication::desktop()->size().height());

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    Screen.save(&buffer, "PNG");
    return byteArray;
}

inline QString Ls(QString pPath)
{
    QString result = QString("Failed");
    QDir dir(pPath);
    if(!dir.exists())
        return result;
    QFileInfoList tmpFileInfoList = dir.entryInfoList(QDir::Dirs);
    quint32 tmpCnt = tmpFileInfoList.count();
    result = QString("Ls");
    for(quint32 i=0;i<tmpCnt;i++)
    {
        if(tmpFileInfoList.at(i).fileName() == "." || tmpFileInfoList.at(i).fileName() == "..")
            continue;
        result.append(";");
        result.append(tmpFileInfoList.at(i).fileName());
        result.append("/");
    }
    tmpFileInfoList.clear();
    tmpFileInfoList = dir.entryInfoList(QDir::Files);
    tmpCnt = tmpFileInfoList.count();
    for(quint32 i=0;i<tmpCnt;i++)
    {
        if(tmpFileInfoList.at(i).fileName() == "." || tmpFileInfoList.at(i).fileName() == "..")
            continue;
        result.append(";");
        result.append(tmpFileInfoList.at(i).fileName());
    }
    result.append(";Success");
    return result;
}
#endif // BASEPARSER_H
