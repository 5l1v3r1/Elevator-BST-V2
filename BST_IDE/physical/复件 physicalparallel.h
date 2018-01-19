#ifndef PHYSICALPARALLEL_H
#define PHYSICALPARALLEL_H

#include "physicalbase.h"

class SAFMonitorThread;

#define SAFTEST

class PhysicalSodimasSAF : public PhysicalDevice
{
    Q_OBJECT
public:
    enum FRAME51TYPE {NONE=0, FLOOR, INSPECTION};
    PhysicalSodimasSAF(QObject *parent = 0);
    ~PhysicalSodimasSAF();

public:
    bool open(OpenMode mode);
    void close();
    QByteArray readAll();
    void Monitor();
    bool ConvertProtocol(QByteArray pByteArray);

    void StartMonitor();
    void ReleaseMonitor();
    void StopMonitor();

    FRAME51TYPE GetFrame51Type(QString pType)
    {
        if(pType.compare("Floor", Qt::CaseInsensitive)==0)
            return FLOOR;
        else if(pType.compare("Inspection", Qt::CaseInsensitive)==0)
            return INSPECTION;
        else
            return NONE;
    }
    QString GetFrame51TypeString(FRAME51TYPE pType)
    {
        if(pType == FLOOR)
            return "Floor";
        else if(pType == INSPECTION)
            return "Inspection";
        else
            return "None";
    }

    quint32 InitPara();
    QString GetInformation()
    {
        return QString("%1:Coincide(%2)  DooropenSoundEnable(%3)")
              .arg(mDevName)
              .arg(mFloorFunctionCoincide?(QString("true")):(QString("false")))
              .arg(mDooropenSoundEnable?(QString("true")):(QString("false")));
    }

public slots:
    void UpdatePara(QObject* pObject);
    void slot_Frame51();

public:
    SAFMonitorThread*      mMonitorThread;
    QFile                 *mFile;
    QByteArray             mReadByteArray;

    bool                   mFloorFunctionCoincide;
    bool                   mDooropenSoundEnable;

    FRAME51TYPE            mFrame51Type;
    int                    mFrame51Freq, mMinFrame51Freq, mMaxFrame51Freq;
    int                    mFrame51Timeout, mMinFrame51Timeout, mMaxFrame51Timeout;
    QTimer                 mFrame51Timer;
    int                    mFrame51Count;

    quint32                mOldFloor, mOldFunction;
};

class SAFMonitorThread : public QThread
{
    Q_OBJECT
public:
    SAFMonitorThread(PhysicalSodimasSAF *qesp)
        :QThread()
    {
        mQesp = qesp;
        mRun = true;
    }
    ~SAFMonitorThread()
    {
        Stop();
    }
    void Stop()
    {
        mRun = false;
    }
    PhysicalSodimasSAF* mQesp;
    bool                mRun;
protected:
    virtual void run()
    {
        while (mRun)
        {
            mQesp->Monitor();
        }
        mRun = true;
    }
};

#endif // NODESAF_H
