#include "mapsform.h"
#include "ui_mapsform.h"

MapsForm::MapsForm(QWidget *parent) :
    TabPage(parent),
    ui(new Ui::MapsForm)
{
    ui->setupUi(this);

    ui->JsMapsView->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    ui->JsMapsView->settings()->setAttribute(QWebSettings::PluginsEnabled, true);   //живЊ
    ui->JsMapsView->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    ui->JsMapsView->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    ui->JsMapsView->settings()->setAttribute(QWebSettings::JavaEnabled, true);

    m_RadioBtnGroup = new QButtonGroup;
    m_RadioBtnGroup->addButton(ui->RadioBaiduMaps, 0);
    m_RadioBtnGroup->addButton(ui->RadioGoogleMaps, 1);
    m_RadioBtnGroup->addButton(ui->RadioGaodeMaps, 2);
    m_RadioBtnGroup->addButton(ui->RadiLightMaps, 3);
    connect(m_RadioBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(slot_ChangeMaps(int)));

    slot_ChangeMaps(3);

    QAction *osloAction = new QAction(tr("&Oslo"), this);
    QAction *berlinAction = new QAction(tr("&Berlin"), this);
    QAction *jakartaAction = new QAction(tr("&Jakarta"), this);
    QAction *nightModeAction = new QAction(tr("Night Mode"), this);
    nightModeAction->setCheckable(true);
    nightModeAction->setChecked(false);
    QAction *osmAction = new QAction(tr("About OpenStreetMap"), this);
    connect(osloAction, SIGNAL(triggered()), SLOT(chooseOslo()));
    connect(berlinAction, SIGNAL(triggered()), SLOT(chooseBerlin()));
    connect(jakartaAction, SIGNAL(triggered()), SLOT(chooseJakarta()));
    connect(nightModeAction, SIGNAL(triggered()), ui->MapView, SLOT(toggleNightMode()));
    connect(osmAction, SIGNAL(triggered()), SLOT(aboutOsm()));

    m_Menu = new QMenu;
    m_Menu->addAction(osloAction);
    m_Menu->addAction(berlinAction);
    m_Menu->addAction(jakartaAction);
    m_Menu->addSeparator();
    m_Menu->addAction(nightModeAction);
    m_Menu->addAction(osmAction);

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
    {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id =
            settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system
        // default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered)
        {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        networkSession->open();
    }
    else
    {
        networkSession = 0;
    }
}

MapsForm::~MapsForm()
{
    delete ui;
}

void MapsForm::slot_ChangeMaps(int index)
{
    if(index < 3)
    {
        QString tmpHtmlFile;
        switch(index)
        {
        case 0:
            tmpHtmlFile = QString(":/MapsRc/MapsRc/baidumaps.html");
            break;
        case 1:
            tmpHtmlFile = QString(":/MapsRc/MapsRc/googlemaps.html");
            break;
        case 2:
            tmpHtmlFile = QString(":/MapsRc/MapsRc/gaodemaps.html");
            break;
        default:
            return;
        }
        QFile file(tmpHtmlFile);
        if(!file.open(QIODevice::ReadOnly))
        {
            IDE_TRACE();
            return;
        }
        QString content;
        QTextStream stream(&file);
        content = stream.readAll();
        ui->JsMapsView->setHtml(content);
        ui->JsMapsView->show();
        ui->MapStackedView->setCurrentIndex(0);
        ui->JsMapsView->setFocus();
    }
    else if(index == 3)
    {
        ui->MapStackedView->setCurrentIndex(1);
        ui->MapView->setFocus();
    }
}

void MapsForm::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
    {
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    }
    else
    {
        id = config.identifier();
    }

    QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();
}

void MapsForm::chooseOslo()
{
    ui->MapView->setCenter(59.9138204, 10.7387413);
}

void MapsForm::chooseBerlin()
{
    ui->MapView->setCenter(52.52958999943302, 13.383053541183472);
}

void MapsForm::chooseJakarta()
{
    ui->MapView->setCenter(-6.211544, 106.845172);
}

void MapsForm::aboutOsm()
{
    QDesktopServices::openUrl(QUrl("http://www.openstreetmap.org"));
}

void MapsForm::on_BtnGo_clicked()
{
    ui->MapView->setCenter(ui->LatitudeSpinBox->value(), ui->LongitudeSpinBox->value());
}
