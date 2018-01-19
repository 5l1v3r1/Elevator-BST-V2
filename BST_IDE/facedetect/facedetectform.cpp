#include "facedetectform.h"
#include "ui_facedetectform.h"

FaceDetectForm::FaceDetectForm(QWidget *parent) :
    TabPage(parent),
    ui(new Ui::FaceDetectForm)
{
    ui->setupUi(this);

    m_PicTakenCount = 0;
#ifdef SURRPORT_OPENCV
    cam     = 0;
    cascade = 0;
    storage = 0;
#endif

    cascade_name = D_FSRCPATH + D_BOOTDIR + QString("haarcascade_frontalface_alt.xml"); //人脸检测要用到的分类器

    timer   = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(readFarme()));  // 时间到，读取当前摄像头信息

    ui->PictureQueneList->setIconSize(QSize(60, 60));
    ui->PictureQueneList->setViewMode(QListView::ListMode);
    ui->PictureQueneList->setMovement(QListView::Static);
    ui->PictureQueneList->setFlow(QListView::LeftToRight);
    ui->PictureQueneList->setWrapping(true);
}

FaceDetectForm::~FaceDetectForm()
{
    closeCamara();
    delete timer;
    delete ui;
}

/******************************
********* 打开摄像头 ***********
*******************************/
void FaceDetectForm::openCamara()
{
#ifdef SURRPORT_OPENCV
    cam = cvCreateCameraCapture(0);//打开摄像头，从摄像头中获取视频
    timer->start(33);              // 开始计时，超时则发出timeout()信号
#endif
}

/*********************************
********* 读取摄像头信息 ***********
**********************************/
void FaceDetectForm::readFarme()
{
#ifdef SURRPORT_OPENCV
    frame = cvQueryFrame(cam);      // 从摄像头中抓取并返回每一帧
    // 将抓取到的帧，转换为QImage格式。QImage::Format_RGB888不同的摄像头用不同的格式。
    QImage image = QImage((const uchar*)frame->imageData, frame->width, frame->height, QImage::Format_RGB888).rgbSwapped();
    ui->PreviewLabel->setPixmap(QPixmap::fromImage(image));  // 将图片显示到label上
#endif
}

/*******************************
***关闭摄像头，释放资源，必须释放***
********************************/
void FaceDetectForm::closeCamara()
{
#ifdef SURRPORT_OPENCV
    timer->stop();         // 停止读取数据。
    cvReleaseCapture(&cam);//释放内存；
#endif
}

/*************************
********* 拍照 ***********
**************************/
void FaceDetectForm::takingPictures()
{
#ifdef SURRPORT_OPENCV
    frame = cvQueryFrame(cam);// 从摄像头中抓取并返回每一帧
    //将抓取到的帧，转换为QImage格式。QImage::Format_RGB888不同的摄像头用不同的格式。
    QImage image = QImage((const uchar*)frame->imageData, frame->width, frame->height, QImage::Format_RGB888).rgbSwapped();
    if(addPictureItem(image, D_GrabScreenPath + QString("%1.bmp").arg(m_PicTakenCount)))
        m_PicTakenCount++;
#endif
}

bool FaceDetectForm::addPictureItem(QImage pImage, QString pName)
{
    pImage.save(pName);
    IDE_TRACE_STR(pName);
    QListWidgetItem *tmpItem = new QListWidgetItem(QIcon(QPixmap::fromImage(pImage)), getFileName(pName));
    tmpItem->setStatusTip(pName);
    ui->PictureQueneList->addItem(tmpItem);
    return true;
}

void FaceDetectForm::on_PictureQueneList_itemDoubleClicked(QListWidgetItem *item)
{
    QString tmpPixName = item->statusTip();
#ifdef SURRPORT_OPENCV
    DetectOnePixmap(tmpPixName);
#endif
}

#ifdef SURRPORT_OPENCV
void FaceDetectForm::detect_and_draw(IplImage *img)
{
    static CvScalar colors[] =
    {
        {{0,0,255}},
        {{0,128,255}},
        {{0,255,255}},
        {{0,255,0}},
        {{255,128,0}},
        {{255,255,0}},
        {{255,0,0}},
        {{255,0,255}}
    };

    double scale = 1.3;
    IplImage* gray = cvCreateImage(cvSize(img->width,img->height), 8, 1);
    IplImage* small_img = cvCreateImage(cvSize( cvRound (img->width/scale), cvRound(img->height/scale)), 8, 1);

    cvCvtColor( img, gray, CV_BGR2GRAY );
    cvResize( gray, small_img, CV_INTER_LINEAR );
    cvEqualizeHist( small_img, small_img );
    cvClearMemStorage( storage );

    if( cascade )
    {
        /*函数cvHaarDetectObjects检测图像中的目标，由OpenCV提供。*/
        CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage, 1.1, 2, 0 ,
                                            cvSize(30, 30) );
        for( int i = 0; i < (faces ? faces->total : 0); i++ )
        {
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
            CvPoint center;
            int radius;
            center.x = cvRound((r->x + r->width*0.5)*scale);
            center.y = cvRound((r->y + r->height*0.5)*scale);
            radius = cvRound((r->width + r->height)*0.25*scale);
            cvCircle( img, center, radius, colors[i%8], 3, 8, 0 );
        }
    }

    cvShowImage( "result", img );
    //cvResizeWindow( "result", 800, 600 );
    cvReleaseImage( &gray );
    cvReleaseImage( &small_img );
}

void FaceDetectForm::DetectOnePixmap(QString pFile)
{
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name.toLatin1().data(), 0, 0, 0 );   //加载人脸检测所用的分类器
    if( !cascade )
    {
       IDE_DEBUG(QString("ERROR: Could not load classifier cascade"));
       return ;
    }
    storage = cvCreateMemStorage(0);                                       //动态存储结构，用来存储人脸在图像中的位置
    cvNamedWindow( "result", 1 );
    const char* filename = pFile.toLatin1().data();//待检测图像(包含绝对路径)
    IDE_TRACE_STR(pFile);
    IplImage* image = cvLoadImage( filename, 1 );                          //加载图像
    if(image)
    {
        detect_and_draw( image );                                              //对加载的图像进行检测
        cvWaitKey(0);
        cvReleaseImage( &image );
        cvDestroyWindow("result");
    }
}
#endif

void FaceDetectForm::on_BtnOpenCamera_clicked()
{
    QString tmpString = ui->BtnOpenCamera->text();
    if(tmpString.compare("OpenCamera",Qt::CaseInsensitive) == 0)
    {
        ui->BtnOpenCamera->setText("CloseCamera");
        ui->BtnOpenCamera->setIcon(QIcon(":/Menu/rc/Menu/pass.png"));
        openCamara();
    }
    else if(tmpString.compare("CloseCamera",Qt::CaseInsensitive) == 0)
    {
        ui->BtnOpenCamera->setText("OpenCamera");
        ui->BtnOpenCamera->setIcon(QIcon(":/Menu/rc/Menu/notpass.png"));
        closeCamara();
    }
}

void FaceDetectForm::on_BtnTakePicture_clicked()
{
    takingPictures();
}
