#ifndef BEARERCLOUD_H
#define BEARERCLOUD_H

#include <QNetworkConfigurationManager>

#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMap>
#include <QHash>
#include "cloud.h"

class BearerCloud : public QGraphicsScene
{
    Q_OBJECT

public:
    BearerCloud(QObject *parent = 0);
    ~BearerCloud();

    void cloudMoved();

    void timerEvent(QTimerEvent *event);

private Q_SLOTS:
    void configurationAdded(const QNetworkConfiguration &config);
    void configurationRemoved(const QNetworkConfiguration &config);
    void configurationChanged(const QNetworkConfiguration &config);
    void updateConfigurations();

private:
    QNetworkConfigurationManager manager;

    QGraphicsTextItem *thisDevice;
    QHash<QString, Cloud *> configurations;

    QMap<QNetworkConfiguration::StateFlags, qreal> offset;
    QMultiMap<QNetworkConfiguration::StateFlags, QString> configStates;

    int timerId;
};

class CloudView : public QGraphicsView
{
    Q_OBJECT

public:
    CloudView(QWidget *parent = 0);
    ~CloudView() { }

protected:
    void resizeEvent(QResizeEvent *) {
        fitInView(sceneRect(), Qt::KeepAspectRatio);
    }
#ifdef Q_OS_WINCE
    void hideEvent(QHideEvent *) {
        qApp->quit();
    }
#endif
};

#endif // DIGITALWATERMARK_H
