#ifndef EFFECTSWIPE_H
#define EFFECTSWIPE_H

#include "global.h"
#include <QStackedWidget>
#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QProcess>
#include <QObject>
#include <QTimer>
#include <QStringRef>
#include <QDebug>
#include <QDir>
#include <QBasicTimer>
#include <QCache>
#include <QKeyEvent>
#include <QPainter>
#include <QVector>
#include <QTime>
#include <QList>
#include <QPaintEvent>
#include <QtGui>
#include <QXmlStreamReader>
#include <QStringRef>

#define DEFAULT_INPUT_TIMEOUT 10000
#define SIZING_FACTOR_HEIGHT 6/10
#define SIZING_FACTOR_WIDTH 6/10

/*******************************************************************/
                      /*** DemoApplication ***/
/*******************************************************************/
class DemoApplication : public QObject
{
  Q_OBJECT
public:
    DemoApplication(QString executableName,QString imageName);
    QImage* getImage(QString imagePath);

private:
    QString imagePath;
    QString appCaption;  //Ó?Ó?³???±ê?â
    QString executablePath;  //Ó??§Â·Ÿ¶
    QStringList arguments;
    QProcess process;
};

/*******************************************************************/
                      /*** PictureFlow ***/
/*******************************************************************/

class PictureFlowPrivate;

class PictureFlow : public QWidget
{
    Q_OBJECT

      Q_PROPERTY(int slideCount READ slideCount WRITE setSlideCount)
      Q_PROPERTY(int currentSlide READ currentSlide WRITE setCurrentSlide)
      Q_PROPERTY(QSize slideSize READ slideSize WRITE setSlideSize)
      Q_PROPERTY(int zoomFactor READ zoomFactor WRITE setZoomFactor)

    public:
      PictureFlow(QWidget* parent = 0);
      ~PictureFlow();
      int slideCount() const;
      void setSlideCount(int count);
      QSize slideSize() const;  //·µ»Ø»?µÆÆ¬?æ»?
      void setSlideSize(QSize size);
      void setZoomFactor(int zoom);
      int zoomFactor() const;
      void clearCaches();
      virtual QImage slide(int index) const;
      virtual void setSlide(int index, const QImage& image);

      virtual void setSlideCaption(int index, QString caption);
      virtual void setSlide(int index, const QPixmap& pixmap);
      int currentSlide() const;

    public slots:
      void setCurrentSlide(int index);
      void clear();
      void render();
      void showPrevious();
      void showNext();
      void showSlide(int index);

    signals:
      void itemActivated(int index);
      void inputReceived();

    protected:
      void paintEvent(QPaintEvent *event);
      void keyPressEvent(QKeyEvent* event);
      void mouseMoveEvent(QMouseEvent* event);
      void mousePressEvent(QMouseEvent* event);
      void mouseReleaseEvent(QMouseEvent* event);
      void resizeEvent(QResizeEvent* event);
      void timerEvent(QTimerEvent* event);

    private:
      PictureFlowPrivate* d;
};

/*******************************************************************/
                      /*** SlideShow ***/
/*******************************************************************/

class SlideShowPrivate;

class SlideShow : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int slideInterval READ slideInterval WRITE setSlideInterval)

public:
    SlideShow(QWidget* parent = 0);
    ~SlideShow();
    void addImage(QString filename);
    void addImageDir(QString dirName);
    void clearImages();
    void startShow();
    void stopShow();

    int slideInterval();
    void setSlideInterval(int val);

protected:
    void paintEvent(QPaintEvent *event);

private:
    SlideShowPrivate* d;
};

class EffectSwipe : public QStackedWidget
{
    Q_OBJECT
public:
    EffectSwipe(QWidget *parent, QRect pRect);
    void InitEffectRc(EFFECT_INFO pEffect);
    QList<QString> m_PathList;
    QRect TmppRect;
public slots:
    void geometryChanged(QRect TmpRect);

public:
    PictureFlow* pictureFlowWidget;
    SlideShow* slideShowWidget;
    QTimer* inputTimer;
    QList<DemoApplication*> demoList;

    void populatePictureFlow();
};
#endif // EFFECTSWIPE_H
