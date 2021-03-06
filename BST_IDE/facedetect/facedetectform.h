#ifndef FACEDETECTFORM_H
#define FACEDETECTFORM_H

#include "tabpage.h"
#ifdef SURRPORT_OPENCV
#include "highgui.h"
#include "cv.h"
#endif

namespace Ui {
class FaceDetectForm;
}

class FaceDetectForm : public TabPage
{
    Q_OBJECT
public:
    explicit FaceDetectForm(QWidget *parent = 0);
    ~FaceDetectForm();

    void openCamara();
    void closeCamara();
    void takingPictures();
    bool addPictureItem(QImage pImage, QString pName);
#ifdef SURRPORT_OPENCV
    void detect_and_draw(IplImage *img);
    void DetectOnePixmap(QString pFile);
#endif
    
private slots:
    void readFarme();
    void on_PictureQueneList_itemDoubleClicked(QListWidgetItem *item);
    void on_BtnOpenCamera_clicked();
    void on_BtnTakePicture_clicked();

private:
    Ui::FaceDetectForm *ui;
#ifdef SURRPORT_OPENCV
    CvHaarClassifierCascade* cascade;
    CvMemStorage* storage;
#endif
    QString   cascade_name; //人脸检测要用到的分类器

    quint32   m_PicTakenCount;

    QTimer    *timer;
#ifdef SURRPORT_OPENCV
    CvCapture *cam;// 视频获取结构， 用来作为视频获取函数的一个参数
    IplImage  *frame;//申请IplImage类型指针，就是申请内存空间来存放每一帧图像
#endif
};

#endif // FACEDETECTFORM_H
