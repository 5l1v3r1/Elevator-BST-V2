#include "logicvideophone.h"
#include "logicthememanager.h"
#include "graphicsvideo.h"
#include "logicnetmanager.h"
#ifdef DISPLAY
#include "devicemanager.h"
#endif

LogicVideoPhone::LogicVideoPhone(QObject *parent)
    :LogicDevice(parent)
{
}

LogicVideoPhone::~LogicVideoPhone()
{
}

quint32 LogicVideoPhone::InitPara()
{
    QDomNodeList tmpParaNodeList = m_ParaElement.childNodes();
    if(tmpParaNodeList.count() < 1)
        return 0;
    quint32 validcount = 0;
    QHBoxLayout* tmpHBoxLayout;
    for(int i=0;i<tmpParaNodeList.count();i++)
    {
        QDomElement tmpElement = tmpParaNodeList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpName = tmpElement.tagName();
        if(!tmpName.compare("Size"))
        {
            QString tmpValue = tmpElement.text();
            mSize = RESOLUTION(tmpValue);
            tmpHBoxLayout = CreateTextEdit(tmpName, tmpValue);
        }
        else if(!tmpName.compare("Video"))
        {
            tmpHBoxLayout = CreateBool(tmpName, tmpElement.text());
        }
        else if(!tmpName.compare("Audio"))
        {
            tmpHBoxLayout = CreateBool(tmpName, tmpElement.text());
        }
        else if(!tmpName.compare("Device"))
        {
            QStringList tmpDevList = tmpElement.text().split(';');
            AppendPHDevices(tmpDevList);
            tmpHBoxLayout = CreateCheck(tmpName, tmpElement.attribute("List").split(';'), tmpDevList);
        }
        else
        {
            continue;
        }
        validcount++;
        m_SettingGroup->m_VLayout->addWidget(CreateLine());
        m_SettingGroup->m_VLayout->addLayout(tmpHBoxLayout);
    }
    validcount++;
    return validcount;
}

bool LogicVideoPhone::SetValue(QString pValue)
{
    if(pValue.isEmpty())
        return true;
    if(!pValue.compare("start"))
    {
#ifdef DISPLAY
        LogicThemeManager* tmpThemeManager = VAR_CAST<LogicThemeManager*>(m_DevManager->m_LGDevList.value(L_ThemeManager));
        if(!tmpThemeManager)
            return false;
        GraphicsOperate* tmpVideoPrt = tmpThemeManager->m_UiView->mScene->m_ComDiagramGroup.value(COM_VIDEO);
        GraphicsVideo *tmpVideo = VAR_CAST<GraphicsVideo*>(tmpVideoPrt);
        if(!tmpVideo)
            return false;
        GuestNetManager *tmpNetManager = VAR_CAST<GuestNetManager *>(m_DevManager->m_LGDevList.value(L_NetManager));
        if(!(tmpNetManager && tmpNetManager->m_PHDev))
            return false;
        QTcpSocket *tmpSocket = (QTcpSocket*)(tmpNetManager->m_PHDev);
        if(!tmpSocket)
            return false;
        QString tmpIP = tmpSocket->peerAddress().toString();
        //>@开启流发送程序
        QString tmpPath = QString("%1 -y -fflags +genpts -f video4linux2 -vcodec mjpeg -s %2 -i %3 -vcodec copy -an -f rtp rtp://%4:%5 -f alsa -ar 8000 -ac 2 -i hw:0 -strict -2 -vn -acodec mp2 -f rtp rtp://%6:%7 < /dev/null > /dev/null &")
                                .arg(D_FFMPEG_PATH)
                                .arg(mSize.toString())
                                .arg("/dev/video1")
                                .arg(tmpIP)
                                .arg(D_VPVPORT)
                                .arg(tmpIP)
                                .arg(D_VPAPORT);
        IDE_TRACE_STR(tmpPath);
        system(tmpPath.toLatin1().data());
        //>@开启流接收程序
        tmpVideo->PlayVideo(QString("-nocache %1").arg(D_VideoPhonePath));
#endif
    }
    else if(!pValue.compare("stop"))
    {
        system("killall ffmpeg mplayer");
    }
    return true;
}

void LogicVideoPhone::UpdatePara(QObject* pObject)
{
    QString tmpObjectName = pObject->objectName();
    QDomElement tmpElement = m_ParaElement.firstChildElement(tmpObjectName);
    if(tmpElement.isNull())
        return;
    QString tmpName = tmpElement.tagName();
    if(!tmpName.compare("Size"))
    {
        TextEdit *tmpComponent = VAR_CAST<TextEdit *>(pObject);
        if(tmpComponent)
        {
            QString tmpSize;
            if(!ModifyTextEdit(tmpComponent, tmpElement, tmpSize, tmpComponent->text()))
            {
            }
            else
            {
                mSize = RESOLUTION(tmpSize);
            }
        }
    }
    else if(!tmpName.compare("Video"))
    {
        Switch *tmpComponent = VAR_CAST<Switch *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyBool(tmpComponent, tmpElement, mVideo, tmpComponent->isChecked()))
            {
            }
        }
    }
    else if(!tmpName.compare("Audio"))
    {
        Switch *tmpComponent = VAR_CAST<Switch *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyBool(tmpComponent, tmpElement, mAudio, tmpComponent->isChecked()))
            {
            }
        }
    }
    else if(!tmpName.compare("Device"))
    {
        CheckList *tmpComponent = VAR_CAST<CheckList *>(pObject);
        if(tmpComponent)
        {
            QStringList tmpDevList = tmpComponent->GetSelectList();
            QStringList tmpList = PHDevicesAdded(tmpDevList, tmpComponent);
            if(!ModifyCheck(tmpComponent, tmpElement, tmpComponent->m_SelectList, tmpList))
            {
            }
        }
    }
}
