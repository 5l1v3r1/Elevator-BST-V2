#include "global.h"
#include "loginform.h"
#include "ui_loginform.h"

SplashForm::SplashForm(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::SplashForm)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("BST_IDE"));

    setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette tmpPalette = this->palette();
    tmpPalette.setBrush(QPalette::Window,
                        QBrush(QImage(":/Page/rc/Page/loginframe.png").scaled(size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
    setPalette(tmpPalette); // widget加上了背景图.

    ui->DatabaseIP->setText(GetLocalIP());
    ui->DatabaseCombo->insertItem(0,QString("BST_IDE"));
    ui->UsernameEdit->setText(QString("sa"));
    ui->PasswdEdit->setText(QString("123456"));
}

SplashForm::~SplashForm()
{
    delete ui;
}

void SplashForm::accept()
{
    ENET_ADDR tmpIPAddr(ui->DatabaseIP->text());
    if(tmpIPAddr.isValid() == false)
    {
        QMessageBox::question(this, tr("Splash Question"),
                                      tr("IP is invalid, please check it!"),
                                      QMessageBox::Yes);
        return;
    }
    GlobalPara.m_SqlDatabase = QSqlDatabase::addDatabase("QODBC", QString("Manager")+ui->DatabaseCombo->currentText());
    GlobalPara.m_SqlDatabase.setConnectOptions("SQL_ATTR_PACKET_SIZE=1024;SQL_ATTR_CONNECTION_TIMEOUT=1,SQL_ATTR_LOGIN_TIMEOUT=1");
    GlobalPara.m_SqlDatabase.setHostName(ui->DatabaseIP->text());
    GlobalPara.m_SqlDatabase.setDatabaseName(ui->DatabaseCombo->currentText());      //即使dsn中已经设置了UID和PASSWD，仍然需要执行setUserName和setPassword的步骤
    //还要注意这里的用户名要有创建表的权限，不然创建下面的表student会不成功。
    GlobalPara.m_SqlDatabase.setUserName(ui->UsernameEdit->text());
    GlobalPara.m_SqlDatabase.setPassword(ui->PasswdEdit->text());
    if (!GlobalPara.m_SqlDatabase.open())
    {
        int ret = QMessageBox::question(this, tr("Splash Question"),
                              QString("%1.\n").arg(GlobalPara.m_SqlDatabase.lastError().databaseText())
                              +tr("Do you Want login the Limited-Version?"),
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::Yes);
        if(ret == QMessageBox::Yes)
        {
            QDialog::accept();
            return;
        }
        else
        {
            QDialog::reject();
            return;
        }
    }
    QDialog::accept();
}

void SplashForm::reject()
{
    int ret = QMessageBox::warning(this, tr("Splash Warning"),
                                   tr("Do you want to Quit?\nOtherwise,login the Limited-Version."),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);
    if(ret == QMessageBox::No)
    {
        QDialog::accept();
        return;
    }
    QDialog::reject();
}
