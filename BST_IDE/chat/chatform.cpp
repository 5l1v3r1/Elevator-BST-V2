#include "chatform.h"
#include "treeitemclass.h"
#include "ui_chatform.h"
#include "toolbar.h"
#include "projectmanager.h"

ChatForm::ChatForm(QString pIP, QWidget *parent) :
    TabPage(parent),
    ui(new Ui::ChatForm)
{
    ui->setupUi(this);
    m_WebCamSendProcess = m_WebCamRecvProcess = 0;
    m_DevItem = 0;
    InitUI();
    InitDevice(pIP);
}

ChatForm::ChatForm(DeviceTreeItem *pDevItem, QWidget *parent) :
    TabPage(parent),
    ui(new Ui::ChatForm)
{
    ui->setupUi(this);
    m_WebCamSendProcess = m_WebCamRecvProcess = 0;
    m_DevItem = 0;
    InitUI();
    InitDevice(pDevItem);
}

ChatForm::~ChatForm()
{
    ReleaseWebCamProcess();
    delete ui;
}

void ChatForm::InitUI()
{
    //>@设置发送按钮
    ui->BtnSend->setPopupMode(QToolButton::MenuButtonPopup);
    ui->BtnSend->setMenu(CreateSendMenu());
    connect(ui->BtnSend, SIGNAL(clicked()), this, SLOT(slot_SendTriggered()));

    ui->SendTextEdit->setFocusPolicy(Qt::StrongFocus);
    ui->TalkRecordBrowser->setFocusPolicy(Qt::NoFocus);
    ui->MediaSplitter->hide();

    ui->SendTextEdit->setFocus();
    ui->SendTextEdit->installEventFilter(this);//设置完后自动调用其eventFilter函数
    connect(ui->SendTextEdit,SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this,SLOT(currentFormatChanged(const QTextCharFormat)));
}

void ChatForm::InitDevice(QString pAddr)
{
    if(pAddr.compare(BROADCASTIP) == 0)
    {
        //>@广播操作
    }
    else
    {
        ProjectBar *tmpProBar = VAR_CAST<ProjectBar *>(GlobalPara.m_ProjectBar);
        if(!(tmpProBar && tmpProBar->mProjectManager))
        {
            setEnabled(false);
        }
        else
        {
            InitDevice(tmpProBar->mProjectManager->GetDeviceFromAddr(pAddr));
        }
    }
}

void ChatForm::InitDevice(DeviceTreeItem *pDevItem)
{
    m_DevItem = pDevItem;
    if(m_DevItem && m_DevItem->mInterfaceDevice)
    {
        NetDevice *tmpNetDev = VAR_CAST<NetDevice*>(m_DevItem->mInterfaceDevice);
        if(tmpNetDev)
        {
            connect(tmpNetDev, SIGNAL(sConnState(bool)), this, SLOT(slot_ConnStateChanged(bool)));
            connect(tmpNetDev->mNetManager, SIGNAL(sMessage(QString)), this, SLOT(processMessage(QString)));
            connect(tmpNetDev->mNetManager, SIGNAL(sCommand(QString)), this, SLOT(processCommand(QString)));
            connect(tmpNetDev->mNetManager, SIGNAL(sInform(QString)), this, SLOT(processInform(QString)));
        }
        slot_ConnStateChanged(true);
    }
    else
    {
        slot_ConnStateChanged(false);
    }
}

void ChatForm::slot_ConnStateChanged(bool pConn)
{
    setEnabled(pConn);
    if(pConn && m_DevItem && m_DevItem->mInterfaceDevice)
    {
        //>@更新页面名
        int index = GlobalPara.m_TabPageManager->indexOf(this);
        GlobalPara.m_TabPageManager->setTabText(index, m_DevItem->mInterfaceDevice->mDevAddr);
    }
}

QMenu* ChatForm::CreateSendMenu()
{
    QMenu *tmpSendMenu = new QMenu;
    QAction *tmpSendMessageAction = new QAction(QIcon(":/Menu/rc/Menu/message.png"), tr("Message"), 0);
    connect(tmpSendMessageAction, SIGNAL(triggered()), this, SLOT(slot_SendMethodChanged()));
    QAction *tmpSendInformAction = new QAction(QIcon(":/Menu/rc/Menu/inform.png"), tr("Inform"), 0);
    connect(tmpSendInformAction, SIGNAL(triggered()), this, SLOT(slot_SendMethodChanged()));
    QAction *tmpSendCommandAction = new QAction(QIcon(":/Menu/rc/Menu/app.png"), tr("Command"), 0);
    connect(tmpSendCommandAction, SIGNAL(triggered()), this, SLOT(tmpSendCommandAction()));
    tmpSendMenu->addAction(tmpSendMessageAction);
    tmpSendMenu->addAction(tmpSendInformAction);
    tmpSendMenu->addAction(tmpSendCommandAction);
    tmpSendMenu->setDefaultAction(tmpSendMessageAction);
    return tmpSendMenu;
}

//当编辑器的字体格式改变时，我们让部件状态也随之改变
void ChatForm::currentFormatChanged(const QTextCharFormat &format)
{
    ui->fontComboBox->setCurrentFont(format.font());
    if(format.fontPointSize()<9)  //如果字体大小出错，因为我们最小的字体为9
        ui->fontsizecomboBox->setCurrentIndex(3); //即显示12
    else
        ui->fontsizecomboBox->setCurrentIndex(ui->fontsizecomboBox->findText(QString::number(format.fontPointSize())));
    ui->textbold->setChecked(format.font().bold());
    ui->textitalic->setChecked(format.font().italic());
    ui->textUnderline->setChecked(format.font().underline());
    color = format.foreground().color();
}

void ChatForm::slot_SendMethodChanged()
{
    QAction *tmpAction = VAR_CAST<QAction *>(sender());
    if(tmpAction)
    {
        ui->BtnSend->setIcon(tmpAction->icon());
        ui->BtnSend->menu()->setDefaultAction(tmpAction);
    }
}

void ChatForm::slot_SendTriggered()
{
    if(!m_DevItem)
        return;
    NetDevice *tmpNetDev = VAR_CAST<NetDevice*>(m_DevItem->mInterfaceDevice);
    if(!(tmpNetDev && tmpNetDev->isSync()))
    {
        QMessageBox::warning(0,tr("警告"),
                             tr("当前连接中断，请检查网络！"),
                             QMessageBox::Ok);
        return;
    }
    QAction *tmpAction = ui->BtnSend->menu()->defaultAction();
    if(tmpAction)
    {
        QString tmpMsg = ui->SendTextEdit->toPlainText();
        ui->SendTextEdit->clear();
        ui->SendTextEdit->setFocus();
        if(tmpMsg.isEmpty())
        {
            QMessageBox::warning(0,tr("警告"),tr("发送内容不能为空"),QMessageBox::Ok);
            return;
        }
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        ui->TalkRecordBrowser->setTextColor(Qt::blue);
        ui->TalkRecordBrowser->setCurrentFont(QFont("Arial", 9,  QFont::DemiBold, false));
        ui->TalkRecordBrowser->append(">>[ " + tr("me") + " ] "+ time);

        ui->TalkRecordBrowser->setTextColor(color);
        ui->TalkRecordBrowser->setCurrentFont(ui->fontComboBox->currentFont());
        ui->TalkRecordBrowser->setFontPointSize(ui->fontsizecomboBox->currentText().toInt());
        ui->TalkRecordBrowser->append(tmpMsg);


        EVENT_TYPE tmpSendType = StrToEventType(tmpAction->text());
        switch(tmpSendType)
        {
            case Event_Message:
                tmpNetDev->mNetManager->SendMessage(tmpMsg);
                GlobalPara.showMessage("Send Message OK!");
                break;
            case Event_Command:
                tmpNetDev->mNetManager->SendCommand(tmpMsg);
                GlobalPara.showMessage("Send Command OK!");
                break;
            case Event_Inform:
                tmpNetDev->mNetManager->SendInform(tmpMsg);
                GlobalPara.showMessage("Send Inform OK!");
                break;
            default:
                GlobalPara.showMessage("Send Fail!");
                break;
        }
    }
}

void ChatForm::processMessage(QString pMsg)   //接收数据
{
    if(!m_DevItem)
        return;
    NetDevice *tmpNetDev = VAR_CAST<NetDevice*>(m_DevItem->mInterfaceDevice);
    if(!(tmpNetDev && tmpNetDev->isSync()))
    {
        QMessageBox::warning(0,tr("警告"),
                             tr("当前连接中断，请检查网络！"),
                             QMessageBox::Ok);
        return;
    }
    QString tmpSenderIP = tmpNetDev->mTcpClient->GetPeerIP().toString();
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    ui->TalkRecordBrowser->setTextColor(Qt::blue);
    ui->TalkRecordBrowser->setCurrentFont(QFont("Arial", 9,  QFont::DemiBold, false));
    ui->TalkRecordBrowser->append("<<[ " + tmpSenderIP + " ] "+ time);

    ui->TalkRecordBrowser->setTextColor(color);
    ui->TalkRecordBrowser->setCurrentFont(ui->fontComboBox->currentFont());
    ui->TalkRecordBrowser->setFontPointSize(ui->fontsizecomboBox->currentText().toInt());
    ui->TalkRecordBrowser->append(pMsg);
}

void ChatForm::processCommand(QString pMsg)
{
    processMessage(pMsg);
}

void ChatForm::processInform(QString pMsg)
{
    processMessage(pMsg);
}

void ChatForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool ChatForm::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->SendTextEdit)
    {
        if(event->type() == QEvent::KeyPress)//回车键
        {
             QKeyEvent *k = static_cast<QKeyEvent *>(event);
             if(k->key() == Qt::Key_Return)
             {
                 slot_SendTriggered();
                 return true;
             }
        }
    }
    return QWidget::eventFilter(target,event);
}

void ChatForm::on_fontComboBox_currentFontChanged(QFont f)//字体设置
{
    ui->SendTextEdit->setCurrentFont(f);
    ui->SendTextEdit->setFocus();
}

//字体大小设置
void ChatForm::on_fontsizecomboBox_currentIndexChanged(const QString &arg1)
{
   ui->SendTextEdit->setFontPointSize(arg1.toDouble());
   ui->SendTextEdit->setFocus();
}

void ChatForm::on_textbold_clicked(bool checked)
{
    if(checked)
        ui->SendTextEdit->setFontWeight(QFont::Bold);
    else
        ui->SendTextEdit->setFontWeight(QFont::Normal);
    ui->SendTextEdit->setFocus();
}

void ChatForm::on_textitalic_clicked(bool checked)
{
    ui->SendTextEdit->setFontItalic(checked);
    ui->SendTextEdit->setFocus();
}

void ChatForm::on_textUnderline_clicked(bool checked)
{
    ui->SendTextEdit->setFontUnderline(checked);
    ui->SendTextEdit->setFocus();
}

void ChatForm::on_textcolor_clicked()
{
    color = QColorDialog::getColor(color,this);
    if(color.isValid())
    {
        ui->SendTextEdit->setTextColor(color);
        ui->SendTextEdit->setFocus();
    }
}

void ChatForm::on_SendFile_clicked()
{
    if(!m_DevItem)
        return;
    NetDevice *tmpNetDev = VAR_CAST<NetDevice*>(m_DevItem->mInterfaceDevice);
    if(!(tmpNetDev && tmpNetDev->isSync()))
    {
        QMessageBox::warning(0,tr("警告"),
                             tr("当前连接中断，请检查网络！"),
                             QMessageBox::Ok);
        return;
    }
    static QString sCurrentPath;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select File"),
                                         sCurrentPath,
                                         QString("*.*"));
    if (fileName.isEmpty())
        return;
    sCurrentPath = getFileDirectory(fileName);

    tmpNetDev->mNetManager->SendFile("y", fileName, QString("/tmp/%1").arg(getFileName(fileName)), 60000);
}

void ChatForm::on_GetFile_clicked()
{
    if(!m_DevItem)
        return;
    NetDevice *tmpNetDev = VAR_CAST<NetDevice*>(m_DevItem->mInterfaceDevice);
    if(!(tmpNetDev && tmpNetDev->isSync()))
    {
        QMessageBox::warning(0,tr("警告"),
                             tr("当前连接中断，请检查网络！"),
                             QMessageBox::Ok);
        return;
    }
    static QString sCurrentPath;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Select Path"),
                                         sCurrentPath,
                                         QString("*.*"));
    if (fileName.isEmpty())
        return;
    sCurrentPath = getFileDirectory(fileName);

    tmpNetDev->mNetManager->GetFile("y", QString("/tmp/%1").arg(getFileName(fileName)), fileName, 600000);
}

//>@保存日志到当前设备缓冲的LOG目录下，以时间进行命名
void ChatForm::on_SaveTalkRecord_clicked()
{
    if(ui->TalkRecordBrowser->document()->isEmpty())
    {
        QMessageBox::warning(0,tr("warn"),
                             tr("Current window is empty！"),
                             QMessageBox::Ok);
    }
    else
    {
        if(!(m_DevItem && m_DevItem->mInterfaceDevice))
        {
            QMessageBox::warning(0,tr("warn"),
                                 tr("Current device is uninited,\n"
                                    "Please init device firstly!"),
                                 QMessageBox::Ok);
            return;
        }
        if(m_DevItem->mInterfaceDevice->mBufferPath.isEmpty())
        {
            QMessageBox::warning(0,tr("warn"),
                                 tr("Current device's buffer is not set,\n"
                                    "Please set firstly!"),
                                 QMessageBox::Ok);
            return;
        }
        QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd.chat");
        QFile file(m_DevItem->mInterfaceDevice->mBufferPath + D_LOGDIR + fileName);
        if(!file.open(QFile::Append | QFile::Text))
        {
            QMessageBox::critical(this,tr("Save Error"),
                                  tr("Can't save file %1:\n %2").arg(fileName)
                                                                .arg(file.errorString()),
                                  QMessageBox::Ok);
            return;
        }
        QTextStream out(&file);
        out << ui->TalkRecordBrowser->toPlainText() << '\n';
        ui->TalkRecordBrowser->clear();
    }
}

void ChatForm::CreatWebCamProcess()
{
    ReleaseWebCamProcess();
    m_WebCamSendProcess = new QProcess;
    m_WebCamSendProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_WebCamSendProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slot_WebCamSendProcessFinished(int, QProcess::ExitStatus)));
    m_WebCamRecvProcess = new QProcess;
    m_WebCamRecvProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_WebCamRecvProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slot_WebCamRecvProcessFinished(int, QProcess::ExitStatus)));
}

void ChatForm::ReleaseWebCamProcess()
{
    if(m_WebCamSendProcess !=0)
    {
        disconnect(m_WebCamSendProcess, SIGNAL(finished(int, QProcess::ExitStatus)),0,0);
        m_WebCamSendProcess->kill();
        static quint8 tmpState;
        while((tmpState = m_WebCamSendProcess->state()))
        {
            IDE_TRACE_INT(tmpState);
            QtSleep(20);
        }
        m_WebCamSendProcess->waitForFinished();
        delete m_WebCamSendProcess;
        m_WebCamSendProcess = 0;
    }
    if(m_WebCamRecvProcess !=0)
    {
        disconnect(m_WebCamRecvProcess, SIGNAL(finished(int, QProcess::ExitStatus)),0,0);
        m_WebCamRecvProcess->kill();
        static quint8 tmpState;
        while((tmpState = m_WebCamRecvProcess->state()))
        {
            IDE_TRACE_INT(tmpState);
            QtSleep(20);
        }
        m_WebCamRecvProcess->waitForFinished();
        delete m_WebCamRecvProcess;
        m_WebCamRecvProcess = 0;
    }
}

void ChatForm::slot_WebCamSendProcessFinished()
{
    if(m_WebCamSendProcess && m_WebCamSendCmd.isEmpty() == false)
        m_WebCamSendProcess->start(m_WebCamSendCmd);
}

void ChatForm::slot_WebCamRecvProcessFinished()
{
    if(m_WebCamRecvProcess && m_WebCamRecvCmd.isEmpty() == false)
        m_WebCamRecvProcess->start(m_WebCamRecvCmd);
}

void ChatForm::on_WebCamera_clicked(bool checked)
{
    if(!m_DevItem)
    {
        ui->WebCamera->setChecked(false);
        return;
    }
    NetDevice *tmpNetDev = VAR_CAST<NetDevice*>(m_DevItem->mInterfaceDevice);
    if(!(tmpNetDev && tmpNetDev->isSync()))
    {
        QMessageBox::warning(0,tr("警告"),
                             tr("当前连接中断，请检查网络！"),
                             QMessageBox::Ok);
        ui->WebCamera->setChecked(false);
        return;
    }

    if(checked)
    {
        ui->MediaSplitter->show();
        CreatWebCamProcess();
        //>@进行SIP会话建立：开启自己的流，并通知对方开启接收，然后开启对方流，成功后开启本地接收程序
        QHostAddress tmpOwnerIP = tmpNetDev->mTcpClient->GetLocalIP();
        QHostAddress tmpGuestIP = tmpNetDev->mTcpClient->GetPeerIP();
        //start /b
        m_WebCamSendCmd = QString("%1 -y -fflags +genpts -f dshow -vcodec mjpeg -s 640x480 -i audio=\"Conexant HD Audio input\":video=\"USB 视频设备\" -vcodec copy -an -f rtp rtp://%2:%3 -vn -acodec aac -strict -2 -f rtp rtp://%4:%5")
                                .arg(D_FFMPEG_PATH)
                                .arg(tmpGuestIP.toString())
                                .arg(D_VPVPORT)
                                .arg(tmpGuestIP.toString())
                                .arg(D_VPAPORT);
        IDE_TRACE_STR(m_WebCamSendCmd);
        m_WebCamSendProcess->startDetached(m_WebCamSendCmd);

        tmpNetDev->mNetManager->Set(QString("<Logic><Monitor>stop</Monitor></Logic>"));
        tmpNetDev->mNetManager->Set(QString("<Logic><VideoPhone>start</VideoPhone></Logic>"));

        //>@播放对方的流媒体
        m_WebCamRecvCmd = QString("%1 sdp://%2 -nocache -wid %3")
                                .arg(D_MPLAYER_PATH)
                                .arg(D_VideoPhonePath)
                                .arg((int)(ui->OppositeSideLabel->winId()));
        IDE_TRACE_STR(m_WebCamRecvCmd);
        m_WebCamRecvProcess->startDetached(m_WebCamRecvCmd);
    }
    else
    {
        ui->MediaSplitter->hide();
        ReleaseWebCamProcess();
        tmpNetDev->mNetManager->Set(QString("<Logic><VideoPhone>stop</VideoPhone></Logic>"));
    }
}
