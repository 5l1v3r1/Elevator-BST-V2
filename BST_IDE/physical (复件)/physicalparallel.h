#ifndef PHYSICALPARALLEL_H
#define PHYSICALPARALLEL_H

#include "physicalbase.h"
#include "QTimer"
#include "QIODevice"

class SAFMonitorThread;

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
              .arg(m_FloorFunctionCoincide?(QString("true")):(QString("false")))
              .arg(m_DooropenSoundEnable?(QString("true")):(QString("false")));
    }

public slots:
    void UpdatePara(QWidget* pWidget);
    void slot_Frame51();
    void slot_Frame51Change();

public:
    SAFMonitorThread*      m_MonitorThread;
    QFile                 *m_File;
    QByteArray             m_ReadByteArray;

    bool                   m_FloorFunctionCoincide;
    bool                   m_DooropenSoundEnable;

    FRAME51TYPE            m_Frame51Type;
    quint32                m_Frame51Freq;
    QTimer                 *m_Frame51Timer;
    quint32                m_Frame51Timeout;
    QTimer                 *m_Frame51ChangeTimer;
    quint32                m_Frame51Count;

    quint32                m_OldFloor, m_OldFunction;
    int                     sendtalk;

    quint32                    m_NewSgoFlag;
    int                         autotest ;
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
        IDE_TRACE();
        while (mRun)
        {
            mQesp->Monitor();
        }
        mRun = true;
    }
};

#endif // NODESAF_H
