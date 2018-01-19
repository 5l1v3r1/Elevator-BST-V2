#include "interface3gpp.h"

Interface3GPP::Interface3GPP(QWidget *parent) :
    InterfaceBase(parent)
{
    //>@从GlobalPara中获取Element

}

void Interface3GPP::ParseFrame(QByteArray pArray)
{

}

QByteArray Interface3GPP::GenerateFrame()
{

}

quint32 Interface3GPP::InitSubAnalysis(QDomNodeList pParaList)
{
    quint32 validHeight = InterfaceBase::InitSubAnalysis(pParaList);

    return validHeight;
}

quint32 Interface3GPP::InitSubGenerator(QDomNodeList pParaList)
{
    quint32 validHeight = InterfaceBase::InitSubAnalysis(pParaList);

    return validHeight;
}

void Interface3GPP::UpdateAnalysis(QObject* pObject)
{

}

void Interface3GPP::UpdateGenerator(QObject* pObject)
{

}
