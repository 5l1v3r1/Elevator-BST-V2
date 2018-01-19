#ifndef INTERFACE3GPP_H
#define INTERFACE3GPP_H

#include "interfacebase.h"
#include "logicelevator.h"

//>@通过ErrorParse类来查找错误。

class Interface3GPP : public InterfaceBase
{
    Q_OBJECT
public:
    Interface3GPP(QWidget *parent=0);

    quint32 InitSubAnalysis(QDomNodeList pParaList);
    quint32 InitSubGenerator(QDomNodeList pParaList);

    //>@根据界面的当前配置生成指令帧
    QByteArray GenerateFrame();

signals:

public slots:
    //>@解析pArray并显示在界面上
    void ParseFrame(QByteArray pArray);

    void UpdateAnalysis(QObject* pObject);
    void UpdateGenerator(QObject* pObject);
};

#endif // INTERFACE3GPP_H
