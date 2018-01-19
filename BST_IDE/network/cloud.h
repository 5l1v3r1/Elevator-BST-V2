#ifndef CLOUD_H
#define CLOUD_H

#include <QNetworkConfiguration>
#include <QNetworkSession>

#include <QGraphicsItem>
#include <QGraphicsSvgItem>

class Cloud : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    explicit Cloud(const QNetworkConfiguration &config, QGraphicsItem *parent = 0);
    ~Cloud();

    enum { Type = UserType + 1 };
    int type() const { return Type; }

    void setFinalScale(qreal factor);
    void setDeleteAfterAnimation(bool deleteAfter);

    void calculateForces();

    bool advance();
    QRectF boundingRect() const;
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    static qreal getRadiusForState(QNetworkConfiguration::StateFlags state);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private Q_SLOTS:
    void stateChanged(QNetworkSession::State state);
    void newConfigurationActivated();

private:
    QNetworkConfiguration configuration;
    QNetworkSession *session;

    QGraphicsTextItem *text;
    QGraphicsSvgItem *icon;

    qreal finalOpacity;
    qreal finalScale;
    qreal currentScale;

    QPointF newPos;

    bool deleteAfterAnimation;
};

#endif
