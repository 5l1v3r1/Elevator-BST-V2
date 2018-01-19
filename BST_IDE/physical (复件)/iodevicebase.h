#ifndef DEVICEBASE_H
#define DEVICEBASE_H

#include "globalfun.h"
#include "tool.h"

#ifdef IDE
    class DeviceScene;
    class DeviceItem;
#elif DISPLAY
    class DeviceManager;
#endif

class IODeviceBase : public QIODevice
{
    Q_OBJECT
public:
    IODeviceBase(QObject *parent = 0);
    ~IODeviceBase();

    virtual bool isSequential() const { return true; }
    virtual bool open(OpenMode mode){ return true; }
    virtual void close(){}

    virtual qint64 pos() const  {return 0;}
    virtual qint64 size() const  {return 0;}
    virtual bool seek(qint64 pos)  {return true;}
    virtual bool atEnd() const  {return true;}
    virtual bool reset()  {return true;}

    virtual qint64 bytesAvailable() const  {return 0;}
    virtual qint64 bytesToWrite() const  {return 0;}

    virtual bool canReadLine() const  {return true;}

    virtual qint64 write(QByteArray data, QString pAddr) {return 0;} //>@向某一地址的设备发送数据

    virtual bool waitForReadyRead(int msecs)  {return true;}
    virtual bool waitForBytesWritten(int msecs)  {return true;}

    virtual qint64 readData(char *data, qint64 maxlen)  {return 0;}
    virtual qint64 readLineData(char *data, qint64 maxlen)  {return 0;}
    virtual qint64 writeData(const char *data, qint64 len)  {return 0;}

    virtual bool isOpen()const{return (QIODevice::isOpen());}
    virtual inline qint64 write(const QByteArray &data){return (QIODevice::write(data));}
    virtual QByteArray readAll(){return (QIODevice::readAll());}

    QFrame* CreateLine();

    //>@创建并初始化控件
    QHBoxLayout* CreateButton(QString pName, QStringList pBtnListTag);
    //>@更改控件，控件会自动发送信号更新XML，
    //>@执行流程：更改控件，控件自动判断逻辑上是否可以更改，如果可以则更新XML，否则不更新XML并恢复控件为上一个状态，此时，还会再次进入自动判断逻辑上是否可以更改，但不会有什么消耗了。
    //bool SetButton(ButtonList *pComponent, QDomElement &pElement);
    //>@当控件更改时（通过点击或者网络更新），更新XML等
    bool ModifyButton(ButtonList *pComponent, QDomElement &pElement, QStringList &pDstValue, QStringList pNewValue);

    QHBoxLayout* CreateTextEdit(QString pName, QString pValue);
    bool ModifyTextEdit(TextEdit *pComponent, QDomElement &pElement, QString &pDstValue, QString pNewValue);

    QHBoxLayout* CreateEnum(QString pName, QStringList pParaList, QString pSelectPara);
    bool ModifyEnum(ComboBox *pComponent, QDomElement &pElement, QString &pDstValue, QString pNewValue);

    QHBoxLayout* CreateBool(QString pName, QString pPara);
    bool ModifyBool(Switch *pComponent, QDomElement &pElement, bool &pDstValue, bool pNewValue);

    QHBoxLayout* CreateSwitch(QString pName, QString pPara);
    bool ModifySwitch(Switch *pComponent, QDomElement &pElement, bool &pDstValue, bool pNewValue);

    QHBoxLayout* CreateENetAddrEdit(QString pName, QString pIP);
    bool ModifyENetAddrEdit(ENetAddrEdit *pComponent, QDomElement &pElement, ENET_ADDR &pDstValue, ENET_ADDR pNewValue);

    QHBoxLayout* CreateDateTimeEdit(QString pName, QDateTime pDateTime);
    bool ModifyDateTimeEdit(DateTimeEdit *pComponent, QDomElement &pElement, QDateTime &pDstValue, QDateTime pNewValue);

    QHBoxLayout* CreateCheck(QString pName, QStringList pParaList, QStringList pSelectParaList);
    bool ModifyCheck(CheckList *pComponent, QDomElement &pElement, QStringList &pDstValue, QStringList pNewValue);

    QHBoxLayout* CreateNum(QString pName, int pValue, int pMinValue, int pMaxValue);
    bool ModifyNum(SpinEdit *pComponent, QDomElement &pElement, int &pDstValue, int pNewValue, int pMinValue, int pMaxValue);

    virtual quint32 InitPara() {return 0;}  //>@返回处理的参数个数
    bool SetPara(QDomElement &pElement, xmlParse *pParse); //>@修改pElement为结果Element
    bool GetPara(QDomElement &pElement, xmlParse *pParse);
    virtual bool SetValue(QString pValue) {return false;}

    bool SetElementText(QDomElement pElement, QString pValue);
    void UpdateProgress(QString pMsg, int pValue);

    //>@获取本节点中的重要信息
    virtual QString GetInformation() { return QString("%1").arg(mDevName); }

public slots:
    virtual void UpdatePara(QObject* pObject) {}

public:
#ifdef IDE
    DeviceScene                 *m_DeviceScene;
    DeviceItem                  *m_DeviceItem;
    void DeleteDevItem();
#elif DISPLAY
    DeviceManager               *m_DevManager;
#endif
    QDomElement                  m_ParaElement;
    Groupbox                    *m_SettingGroup;
    QHash<QString, QObject*>     mParaItems;       //>@保存参数对应的控件

    QString                      mDevName;
};

#endif // PHYSICALBASE_H
