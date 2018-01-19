#ifndef DIGITALWATERMARK_H
#define DIGITALWATERMARK_H

#include "global.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <QtGui>

typedef enum{
    NOMEANINGWK,
    NOMEANINGWKAUTH,
    MEANINGWK,
    MEANINGWKAUTH
} DWT_TYPE;

class TamperAssessDlg : public QDialog
{
    Q_OBJECT
public:
    TamperAssessDlg(QWidget *parent=0);
    explicit TamperAssessDlg(QWidget *parent = 0,
                            int scl=0 ,
                            double * taf=0 ,
                            const char * path=0);

public slots:
    void closeClicked();

private:
        QLabel *Taf0Label;
    QLabel *Taf1Label;
    QLabel *Taf2Label;
    QLabel *Taf3Label;
    QLabel *Taf4Label;
    QLabel *Taf5Label;
    QLabel *imageLabel;

    QLineEdit *Taf0Edit;
    QLineEdit *Taf1Edit;
    QLineEdit *Taf2Edit;
    QLineEdit *Taf3Edit;
    QLineEdit *Taf4Edit;
    QLineEdit *Taf5Edit;

    QPushButton *closeButton;
    QImage image;
};

class DigitalWaterMark : public QObject
{
public:
    DigitalWaterMark(QObject *parent=0);

    int scale;
    double quan;
    char bmpfile[100];
    char auth_bmp_path[100];

    double chaos_kx;
    double chaos_ky;
    double chaos_kz;
    int  wk_length;

    unsigned char *pBmpBuf;//读入图像数据的指针
    double *Wrey;
    double *Wreu;
    double *Wrev;

    long biWidths;
    long biHeights;
    FILE * fp;
    int dwtscale;

    unsigned char image_head[62];
    unsigned char *bmp54; //读取数据头指针
    int bmplen;

    void file_open(char * f1,char * mode);
    void file_SeekToBegin();
    void file_WriteData(unsigned char * image_head,int num);
    void file_close();

    long GetWidth(char * bmpfile);
    long GetHeight(char *bmpfile);
    void readBmp(char *bmpName,int length);
    void file_ReadData(unsigned char *q,int length);
    void file_ReadHead62(unsigned char * image_head);
    void file_ReadHead54();
    void file_save(char * newname);

    void Transform(int Num, double input[], double output[]);
    void ImageDwt(int scale,double*wrey0,double*wrey);
    void Dwt(int scale);
    void DwtNoMeaningAuthWM(char *dstPath);
    void DwtNoMeaningAuth(char *path, char *dstPath);

    void Inv_Transform(int Num, double input[], double output[]);//行变换
    void ImageIdwt(int scale,double *wrey0,double *wrey);
    void Idwt();
    void DwtMeaningAuthWM(double kx, double ky, double kz, int length, char *dstPath);
    void DwtMeaningAuth(double kx, double ky, double kz, int length, char *dstPath);

    void setImage(char *srcPath)
    {
        memset(bmpfile,0,100);
        strncpy(bmpfile,srcPath,100);
    }

    void setNoMeaningWkPara(int pscale, double pquan);
    void setNoMeaningWkAuthPara(int pscale, double pquan, char *pauth_bmp_path);
    void setMeanWkPara(int pscale, double pquan, double pchaos_kx, double pchaos_ky, double pchaos_kz, int pwk_length);

    void ProcessWaterMark(DWT_TYPE pType);
};

#endif // DIGITALWATERMARK_H
