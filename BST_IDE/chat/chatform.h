#ifndef CHATFORM_H
#define CHATFORM_H

#include "tabpage.h"

namespace Ui {
    class ChatForm;
}

class DeviceTreeItem;

#define BROADCASTIP   QString("0.0.0.0")  //>@广播地址

class ChatForm : public TabPage
{
    Q_OBJECT
public:
    typedef enum{ Message, NewParticipant, ParticipantLeft } MessageType;//>@枚举变量标志信息的类型，分别为消息，新用户加入，和用户退出

    ChatForm(QString pIP, QWidget *parent = 0);  //>@pIP:仅仅写IP地址即可
    ChatForm(DeviceTreeItem *pDevItem, QWidget *parent = 0);
    ~ChatForm();

    void InitUI();
    void InitDevice(QString pAddr);
    void InitDevice(DeviceTreeItem *pDevItem);
    QMenu* CreateSendMenu();

    void changeEvent(QEvent *e);
    bool sendMessage(QString pMsg, QString pSendType);
    bool eventFilter(QObject *target, QEvent *event);//事件过滤器

    void CreatWebCamProcess();
    void ReleaseWebCamProcess();

private slots:
    void slot_ConnStateChanged(bool pConn);

    void slot_WebCamSendProcessFinished();
    void slot_WebCamRecvProcessFinished();

    void slot_SendMethodChanged();
    void slot_SendTriggered();

    void processMessage(QString pMsg);
    void processCommand(QString pMsg);
    void processInform(QString pMsg);

    void currentFormatChanged(const QTextCharFormat &format);
    void on_fontComboBox_currentFontChanged(QFont f);
    void on_fontsizecomboBox_currentIndexChanged(const QString &arg1);
    void on_textbold_clicked(bool checked);
    void on_textitalic_clicked(bool checked);
    void on_textUnderline_clicked(bool checked);
    void on_textcolor_clicked();

    void on_SendFile_clicked();
    void on_GetFile_clicked();
    void on_SaveTalkRecord_clicked();

    void on_WebCamera_clicked(bool checked);

public:
    Ui::ChatForm    *ui;

    DeviceTreeItem  *m_DevItem;

    QProcess        *m_WebCamSendProcess, *m_WebCamRecvProcess;
    QString          m_WebCamSendCmd, m_WebCamRecvCmd;

    QString          fileName;
    QColor           color;//颜色
};

#endif // CHATFORM_H
