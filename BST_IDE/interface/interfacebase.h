#ifndef INTERFACEBASE_H
#define INTERFACEBASE_H

#include "global.h"
#include "tool.h"
#include "errorparsebase.h"

class InterfaceBase : public Groupbox
{
    Q_OBJECT
public:
    typedef enum {ANALYSIS=0, GENERATOR} TYPE;
    explicit InterfaceBase(QWidget *parent = 0);

    QFrame* CreateLine();
    //>@创建并初始化控件
    QHBoxLayout* CreateButton(QString pName, QStringList pBtnListTag);
    QHBoxLayout* CreateTextEdit(QString pName, QString pValue);
    QHBoxLayout* CreateEnum(QString pName, QStringList pParaList, QString pSelectPara);
    QHBoxLayout* CreateBool(QString pName, QString pPara);
    QHBoxLayout* CreateSwitch(QString pName, QString pPara);
    QHBoxLayout* CreateCheck(QString pName, QStringList pParaList, QStringList pSelectParaList);
    QHBoxLayout* CreateNum(QString pName, int pValue, int pMinValue, int pMaxValue);

    bool InitAnalysis(QDomElement pElement);
    virtual quint32 InitSubAnalysis(QDomNodeList pParaList);  //>@返回动态创建的窗体的高度
    bool InitGenerator(QDomElement pElement);
    virtual quint32 InitSubGenerator(QDomNodeList pParaList);

    //>@根据界面的当前配置生成指令帧
    virtual QByteArray GenerateFrame() {return QByteArray();}

public slots:
    //>@解析pArray并显示在界面上
    virtual void ParseFrame(QByteArray pFrame) {}

    virtual void UpdateAnalysis(QObject* pObject) {}
    virtual void UpdateGenerator(QObject* pObject) {}

public:
    QHash<QString, ErrorParseBase*>  mErrorParses;
    TYPE                             mUIType;
};

#endif // INTERFACEBASE_H
