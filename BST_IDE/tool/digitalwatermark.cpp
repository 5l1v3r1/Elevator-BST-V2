#include "digitalwatermark.h"


TamperAssessDlg::TamperAssessDlg(QWidget *parent)
    : QDialog(parent)
{

}

TamperAssessDlg::TamperAssessDlg(QWidget *parent,int scl,double * taf,const char * path) : QDialog(parent)
{
    setWindowTitle(tr("Tamper Report"));
    QPixmap pixmap("./image/bg566x412.png");
    QPalette palette;
    palette.setBrush(backgroundRole(), QBrush(pixmap));
    setPalette(palette);
    char str[20];
    QGridLayout *top = new QGridLayout;
    for(int i=0;i<=scl;i++)
    {
        if(i==0)
        {
            Taf0Label = new QLabel(tr("taf0:"));
            Taf0Edit = new QLineEdit;
            memset(str,0,20);
            sprintf(str,"%f",*(taf+0));
            Taf0Edit->setText(str);
            Taf0Edit->setEnabled(false);
            top->addWidget(Taf0Label,0,0);
            top->addWidget(Taf0Edit,0,1);
        }
        else if(i==1)
        {
                Taf1Label = new QLabel(tr("taf1:"));
                Taf1Edit = new QLineEdit;
                memset(str,0,20);
                sprintf(str,"%f",*(taf+1));
                Taf1Edit->setText(str);
                Taf1Edit->setEnabled(false);
                top->addWidget(Taf1Label,1,0);
                top->addWidget(Taf1Edit,1,1);
        }
        else if(i==2)
        {
            Taf2Label = new QLabel(tr("taf2:"));
            Taf2Edit = new QLineEdit;
            memset(str,0,20);
            sprintf(str,"%f",*(taf+2));
            Taf2Edit->setText(str);
            Taf2Edit->setEnabled(false);
            top->addWidget(Taf2Label,2,0);
            top->addWidget(Taf2Edit,2,1);
        }
        else if(i==3)
        {
            Taf3Label = new QLabel(tr("taf3:"));
            Taf3Edit = new QLineEdit;
            memset(str,0,20);
            sprintf(str,"%f",*(taf+3));
            Taf3Edit->setText(str);
            Taf3Edit->setEnabled(false);
            top->addWidget(Taf3Label,3,0);
            top->addWidget(Taf3Edit,3,1);
        }
        else if(i==4)
        {
            Taf4Label = new QLabel(tr("taf4:"));
            Taf4Edit = new QLineEdit;
            memset(str,0,20);
            sprintf(str,"%f",*(taf+4));
            Taf4Edit->setText(str);
            Taf4Edit->setEnabled(false);
            top->addWidget(Taf4Label,4,0);
            top->addWidget(Taf4Edit,4,1);
        }
        else if(i==5)
        {
            Taf5Label = new QLabel(tr("taf5:"));
            Taf5Edit = new QLineEdit;
            memset(str,0,20);
            sprintf(str,"%f",*(taf+5));
            Taf5Edit->setText(str);
            Taf5Edit->setEnabled(false);
            top->addWidget(Taf5Label,5,0);
            top->addWidget(Taf5Edit,5,1);
        }
    }
    imageLabel = new QLabel(tr(" "));
    closeButton = new QPushButton(tr("Close"));
    connect(closeButton,SIGNAL(clicked()),this,SLOT(closeClicked()));
     QVBoxLayout *left = new QVBoxLayout;
    left->addLayout(top);
    left->addWidget(closeButton);
    QHBoxLayout *mainlayout = new QHBoxLayout(this);
    mainlayout->addLayout(left);
    mainlayout->addWidget(imageLabel);
    if(image.load(path));
    {
        imageLabel->setPixmap (QPixmap::fromImage(image));
    }
}

void TamperAssessDlg::closeClicked()
{
    close();
}

DigitalWaterMark::DigitalWaterMark(QObject *parent) :  QObject(parent)
{
}

void DigitalWaterMark::file_open(char * f1,char * mode)
{
    fp=fopen(f1,mode);
}

void DigitalWaterMark::file_SeekToBegin()
{
    fseek(fp,0,0);
}

void DigitalWaterMark::file_WriteData(unsigned char * image_head,int num)
{
    fseek(fp,62,0);
    fwrite(image_head,num,1,fp);
}

void DigitalWaterMark::file_close()
{
    fclose(fp);
}

long DigitalWaterMark::GetWidth(char * bmpfile)
{
    FILE *fp1=fopen(bmpfile,"rb");
    fseek(fp1,18,0);
    fread(&biWidths,4,1,fp1);
    fclose(fp1);
    return biWidths;
}

long DigitalWaterMark::GetHeight(char *bmpfile)
{
    FILE *fp1=fopen(bmpfile,"rb");
    fseek(fp1,22,0);
    fread(&biHeights,4,1,fp1);
    fclose(fp1);
    return biHeights;
}

void DigitalWaterMark::readBmp(char *bmpName,int length)
{
    //二进制读方式打开指定的图像文件
    FILE *fp1=fopen(bmpName,"rb");
    //跳过位图文件头结构BITMAPFILEHEADER
    fseek(fp1,54,0);
    fread(pBmpBuf,1,length,fp1);
    //关闭文件
    fclose(fp1);
}

void DigitalWaterMark::file_ReadData(unsigned char *q,int length)
{
    fseek(fp,62,0);
    fread(q,length,1,fp);
}

void DigitalWaterMark::file_ReadHead62(unsigned char * image_head)
{
    fread(image_head,62,1,fp);
}

void DigitalWaterMark::file_ReadHead54()
{
    fseek(fp,0,0);
    bmp54=(unsigned char *)malloc(54);
    fread(bmp54,54,1,fp);
}

void DigitalWaterMark::file_save(char * newname)
{
    fp=fopen(newname,"wb");
    fwrite(bmp54,54,1,fp);
    fwrite(pBmpBuf,bmplen,1,fp);
    fclose(fp);
}

void DigitalWaterMark::Transform(int Num, double input[], double output[])
{
    int k;
    double * a0;
    a0=(double *)malloc(Num*sizeof(double));
    for(k=0;k<Num;k++)
        a0[k]=input[k];
    double * a1;
    a1=(double *)malloc(Num/2*sizeof(double));
    for(k=0;k<Num/2;k++)
        a1[k]=0;
    for(k=0;k<Num/2;k++)
    {
        a1[k]=a0[2*k]+a0[2*k+1];
    }
    double * d1;
    d1=(double *)malloc(Num/2*sizeof(double));
    for(k=0;k<Num/2;k++)
        d1[k]=0;
    for(k=0;k<Num/2;k++)
    {
        d1[k]=a0[2*k]-a0[2*k+1];
    }
    for(k=0;k<Num/2;k++)
        output[k]=a1[k];
    for(k=0;k<Num/2;k++)
        output[Num/2+k]=d1[k];
    free(a0);
    free(a1);
    free(d1);
}

void DigitalWaterMark::ImageDwt(int scale,double*wrey0,double*wrey)
{
   //**********************************
    int width=biWidths;
    int height=biHeights;

    //目前只处理方形图像，长等于宽
    //tran
    int k,n,i;
    int m;
    m=width;//图像大小:长等于宽
    double Rowdata[512],ColData[512],TranData[512];
    //目前图像大小不超过
    double *Row_data;
    Row_data=(double *)malloc(width*height*sizeof(double));

    for(k=0;k<width*height;k++)
    {
        wrey[k]=wrey0[k];
    }

    for(i=0;i<scale;i++)
    {
        for(k=0;k<m;k++)
        {
            for(n=0;n<m;n++)
                Rowdata[n]=wrey[k*width+n];
            Transform(m,Rowdata,TranData);//********************行变换
            for(n=0;n<m;n++)
            {
                Row_data[k*width+n]=TranData[n];/**///保存行变换数据为外部变量
            }
        }

        for(k=0;k<m;k++)
        {
            for(n=0;n<m;n++)
            {
                ColData[n]=Row_data[n*width+k];//读一列数据
            }
            Transform(m,ColData,TranData);//列变换
            for(n=0;n<m;n++)
            {
                wrey[n*width+k]=TranData[n];//保存列变换数据为外部变量
            }
        }
        m=m/2;//子图像大小
    }

    free(Row_data);
}

void DigitalWaterMark::Dwt(int scale)
{
    dwtscale=scale;
    int width=biWidths;
    int height=biHeights;
    int lineByte=width*3+width%4;
    int i,j;
    double *Wrey0/*,*Greu0,*Grev0*/;//变换前的亮度
    //Grey,*Greu,*Grev;//变换后的亮度
    Wrey0=(double *)malloc(width*height*sizeof(double));
    Wrey=(double *)malloc(width*height*sizeof(double));
    Wreu=(double *)malloc(width*height*sizeof(double));
    Wrev=(double *)malloc(width*height*sizeof(double));

    unsigned char r=0;
    unsigned char g=0;
    unsigned char b=0;
    int k;

    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
            for(k=0;k<3;k++)///肯袼豏GB三个分量分别置0才变成黑色
            {
                if(k==0)
                    r=*(pBmpBuf+i*lineByte+j*3+k);
                if(k==1)
                    g=*(pBmpBuf+i*lineByte+j*3+k);
                if(k==2)
                    b=*(pBmpBuf+i*lineByte+j*3+k);
            }
            Wrey0[i*width+j]=((float)r*0.3f+(float)g*0.59f+(float)b*0.11+0.5f);
            Wrey[i*width+j]=((float)r*0.3f+(float)g*0.59f+(float)b*0.11+0.5f);
            Wreu[i*width+j]=(float)b-Wrey0[i*width+j];
            Wrev[i*width+j]=(float)r-Wrey0[i*width+j];
         }
    }
    ImageDwt(scale,Wrey0,Wrey);   //灰度显示

    free(Wrey0);
}

void DigitalWaterMark::DwtNoMeaningAuthWM(char* dstPath)
{
    int width=biWidths;
    int height=biHeights;
    Dwt(scale);
    int k,n,i,j;
    unsigned char * wm_embed;
    wm_embed=(unsigned char *)malloc(height*width*sizeof(unsigned char));  //存储嵌入数据存放空间

    for(k=0;k<height;k++)
    {
        for(n=0;n<width;n++)
        {
            int dec;
            double quan1;
            quan1=quan;
            for(int sc=scale;sc>=1;sc--)
            {
                if((k<(height/(pow(2,sc)))) && (n<(width/(pow(2,sc)))))
                {
                    quan1=pow(4,sc)*quan;
                    break;
                }
            }
            dec=(int)(Wrey[k*width+n]/quan1);   //此时与原图数据有处理关系
            if((dec%2)==0)
            {
                wm_embed[width*k+n]=0;
            }
            else
                wm_embed[width*k+n]=1;
        }
    }

    memset(image_head,0x00,62);
    image_head[0]=66;
    image_head[1]=77;
    image_head[10]=62;
    image_head[14]=40;
    image_head[26]=1;
    image_head[28]=1;
    image_head[38]=196;
    image_head[39]=14;
    image_head[42]=196;
    image_head[43]=14;
    image_head[58]=255;
    image_head[59]=255;
    image_head[60]=255;

    int length=width*height/8;
    image_head[18]=width%256;
    image_head[19]=width/256;
    image_head[22]=height%256;
    image_head[23]=height/256;
    image_head[34]=length%256;
    image_head[35]=length/256;
    image_head[2]=(62+length)%256;
    image_head[3]=(62+length)/256;
    unsigned char * q;
    q=(unsigned char *)malloc(length*sizeof(unsigned char));
    memset(q,0x00,length);
    j=0;
    for(i=0;i<length;i++)
    {
        unsigned char temp0,temp1,temp2,temp3,temp4,temp5,temp6,temp7;
        if(wm_embed[j]==1){temp0=0x01;temp0=temp0<<7;j++;}
        else{temp0=0x00;temp0=temp0<<7;j++;}
        if(wm_embed[j]==1){temp1=0x01;temp1=temp1<<6;j++;}
        else{temp1=0x00;temp1=temp1<<6;j++;}
        if(wm_embed[j]==1){temp2=0x01;temp2=temp2<<5;j++;}
        else{temp2=0x00;temp2=temp2<<5;j++;}
        if(wm_embed[j]==1){temp3=0x01;temp3=temp3<<4;j++;}
        else{temp3=0x00;temp3=temp3<<4;j++;}
        if(wm_embed[j]==1){temp4=0x01;temp4=temp4<<3;j++;}
        else{temp4=0x00;temp4=temp4<<3;j++;}
        if(wm_embed[j]==1){temp5=0x01;temp5=temp5<<2;j++;}
        else{temp5=0x00;temp5=temp5<<2;j++;}
        if(wm_embed[j]==1){temp6=0x01;temp6=temp6<<1;j++;}
        else{temp6=0x00;temp6=temp6<<1;j++;}
        if(wm_embed[j]==1){temp7=0x01;j++;}
        else{temp7=0x00;j++;}
        q[i]=q[i]|temp0;q[i]=q[i]|temp1;q[i]=q[i]|temp2;
        q[i]=q[i]|temp3;q[i]=q[i]|temp4;q[i]=q[i]|temp5;
        q[i]=q[i]|temp6;q[i]=q[i]|temp7;
    }

    file_open(dstPath,"wb");
    file_SeekToBegin();
    fwrite(image_head,62,1,fp);
    file_WriteData(q,length);
    file_close();

    free(wm_embed);
    free(q);
}

void DigitalWaterMark::DwtNoMeaningAuth(char * path, char *dstPath)
{
    long width=biWidths;
    long height=biHeights;

    file_open(bmpfile,"rb");
    file_ReadHead54();
    file_close();

    file_open(path,"rb");
    file_SeekToBegin();
    file_ReadHead62(image_head);
    int length=image_head[34]+image_head[35]*256;

    unsigned char * q;
    q=(unsigned char *)malloc(length*sizeof(unsigned char));
    file_ReadData(q,length);
    file_close();

    int watermark_N=length*8;
    unsigned char * wm_embed;
    wm_embed=(unsigned char *)malloc(watermark_N*sizeof(unsigned char));
    int i,j;
    j=0;
    for(i=0;i<length;i++)
    {
        unsigned char temp0,temp1,temp2,temp3,temp4,temp5,temp6,temp7;
        temp0=q[i]>>7;temp1=q[i]>>6;temp1=temp1&0x01;
        temp2=q[i]>>5;temp2=temp2&0x01;temp3=q[i]>>4;temp3=temp3&0x01;
        temp4=q[i]>>3;temp4=temp4&0x01;temp5=q[i]>>2;temp5=temp5&0x01;
        temp6=q[i]>>1;temp6=temp6&0x01;temp7=q[i]&0x01;
        wm_embed[j]=temp0;j++;
        wm_embed[j]=temp1;j++;
        wm_embed[j]=temp2;j++;
        wm_embed[j]=temp3;j++;
        wm_embed[j]=temp4;j++;
        wm_embed[j]=temp5;j++;
        wm_embed[j]=temp6;j++;
        wm_embed[j]=temp7;j++;
    }
    //定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
    int lineByte=width*3+width%4;
    int length1=lineByte * height;
    bmplen=length1;
    //申请位图数据所需要的空间，读位图数据进内存
    pBmpBuf=(unsigned char *)malloc(length1);
    readBmp(bmpfile,length1);

    Dwt(scale);
    unsigned char * wm_extract;
    wm_extract=(unsigned char *)malloc(width*height*sizeof(unsigned char));  //提取存放空间
    int k,n;
    for(k=0;k<height;k++)
    {
        for(n=0;n<width;n++)
        {
            int dec;
            double quan1;
            quan1=quan;
            for(int sc=scale;sc>=1;sc--)
            {
                if((k<(height/(pow(2,sc)))) && (n<(width/(pow(2,sc)))))
                {
                    quan1=pow(4,sc)*quan;
                    break;
                }
            }

            dec=(int)(Wrey[k*width+n]/quan1);   //以原来图像数据参考
            if((dec%2)==0)
            {
                wm_extract[width*k+n]=0;
            }
            else
                wm_extract[width*k+n]=1;
        }
    }
    double *taf;
    taf=(double *)malloc((scale+1)*sizeof(double));
    int *numb;
    numb=(int *)malloc((scale+1)*sizeof(int));
    memset(numb,0,(scale+1)*sizeof(int));
    for(k=0;k<height;k++)
    {
        for(n=0;n<width;n++)
        {
            if(wm_extract[k*width+n]!=wm_embed[k*width+n])
            {
                numb[0]=numb[0]+1;
                if(!((k<(height/(pow(2,scale)))) && (n<(width/(pow(2,scale))))))
                {
                    for(int sc=scale-1;sc>=0;sc--)
                    {
                        if((k<(height/(pow(2,sc)))) && (n<(width/(pow(2,sc)))))
                        {
                            *(numb+sc+1)=*(numb+sc+1)+1;
                            break;
                        }
                    }
                }
            }
        }
    }

    taf[0]=((double)numb[0])/(double)(width*height);
    for(i=1;i<=scale;i++)
    {
        *(taf+i)=*(numb+i)/(double)(3*width/pow(2,i)*height/pow(2,i));
    }

    for(k=0;k<height;k++)
    {
        for(n=0;n<width;n++)
        {
            if(wm_extract[k*width+n]!=wm_embed[k*width+n])  //修改图像的区分
            {
                *(pBmpBuf+k*lineByte+n*3+0)=255;
                *(pBmpBuf+k*lineByte+n*3+1)=255;
                *(pBmpBuf+k*lineByte+n*3+2)=255;
            }
            else
            {
                *(pBmpBuf+k*lineByte+n*3+0)=0;
                *(pBmpBuf+k*lineByte+n*3+1)=0;
                *(pBmpBuf+k*lineByte+n*3+2)=0;
            }
        }
    }
    int sca;
    for(sca=1;sca<=scale;sca++)
    {
        k=height/pow(2,sca);
        for(n=0;n<width/pow(2,sca-1);n++)
        {
            *(pBmpBuf+k*lineByte+n*3+0)=255;
            *(pBmpBuf+k*lineByte+n*3+1)=255;
            *(pBmpBuf+k*lineByte+n*3+2)=255;
        }
    }
    for(sca=1;sca<=scale;sca++)
    {
        n=width/pow(2,sca);
        for(k=0;k<height/pow(2,sca-1);k++)
        {
            *(pBmpBuf+k*lineByte+n*3+0)=255;
            *(pBmpBuf+k*lineByte+n*3+1)=255;
            *(pBmpBuf+k*lineByte+n*3+2)=255;
        }
    }
    file_save(dstPath);

    TamperAssessDlg dlg(NULL,scale,taf,dstPath);
    dlg.exec();

    free(wm_embed);
    free(wm_extract);
    free(q);
}

void DigitalWaterMark::Inv_Transform(int Num, double input[], double output[])//行变换
{
    int k;
    double * rc1;//输入的尺度系数
    rc1=(double *)malloc(Num/2*sizeof(double));
    for(k=0;k<Num/2;k++)
        rc1[k]=input[k];
    double * d1;//输入的小波系数
    d1=(double *)malloc(Num/2*sizeof(double));
    for(k=0;k<Num/2;k++)
        d1[k]=input[Num/2+k];
    double * rc0;//重构系数，即output[Num]
    rc0=(double *)malloc(Num*sizeof(double));
    for(k=0;k<Num;k++)
        rc0[k]=0;
    for(k=0;k<Num/2;k++)
    {
        rc0[2*k]=(rc1[k]+d1[k]);
        rc0[2*k+1]=(rc1[k]-d1[k]);
    }
    for(k=0;k<Num;k++)
        output[k]=rc0[k];
    free(rc0);
    free(rc1);
    free(d1);
}

void DigitalWaterMark::ImageIdwt(int scale,double *wrey0,double *wrey)
{
    int width=biWidths;
    int height=biHeights;
    //目前只处理方形图像，长等于宽
    //tran
    int k,n,i,j;
    int m;
    double ColData[512],TranData[512];
    //目前图像大小不超过512
    double *Row_data;
    Row_data=(double *)malloc(width*height*sizeof(double));
    for(i=0;i<width;i++)
    {
        for(j=0;j<height;j++)
        {
            wrey[height*i+j]=wrey0[height*i+j];
        }
    }
    for(i=0;i<scale;i++)
    {
        m=width/pow(2,scale-i-1);
        ////   列重构  ///////
        for(k=0;k<m;k++)
        {
            for(n=0;n<m;n++)
            {
                ColData[n]=wrey[n*width+k];//读一列数据
            }
            Inv_Transform(m,ColData,TranData);//列变换
            for(n=0;n<m;n++)
            {
                Row_data[n*width+k]=TranData[n]/2;//保存为外部变量
            }
        }
        /////  end of 列重构  ////////
        ////    行重构    ///////
        for(k=0;k<m;k++)
        {
            Inv_Transform(m,&Row_data[k*width],TranData);//行变换
            for(n=0;n<m;n++)
            {
                wrey[k*width+n]=TranData[n]/2;//保存为外部数据,更新低频区
            }
        }
    }
    free(Row_data);
}

void DigitalWaterMark::Idwt()
{
    int width=biWidths;
    int height=biHeights;
    int lineByte=width*3+width%4;
    int i,j;
    double *Wrey0/*,*Greu0,*Grev0*/;//变换前的亮度
    //Grey,*Greu,*Grev;//变换后的亮度
    Wrey0=(double *)malloc(width*height*sizeof(double));
    for(i=0;i<height*width;i++)
    {
        Wrey0[i]=Wrey[i];
    }
    ImageIdwt(dwtscale,Wrey0,Wrey);//显示灰度图

    //以下几行显示彩色图
    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
            int r,g,b;
            b=(int)(Wrey[i*width+j]+Wreu[i*width+j]);
            r=(int)(Wrey[i*width+j]+Wrev[i*width+j]);
            g=(int)((Wrey[i*width+j]-0.3f*r-0.11f*b)/0.59);
            r=r>255?255:r;r=r<0?0:r;
            g=g>255?255:g;g=g<0?0:g;
            b=b>255?255:b;b=b<0?0:b;

            *(pBmpBuf+i*lineByte+j*3+0)=(unsigned char)r;
            *(pBmpBuf+i*lineByte+j*3+1)=(unsigned char)g;
            *(pBmpBuf+i*lineByte+j*3+2)=(unsigned char)b;
        }
    }
    //*** 以上几行显示彩色图
    free(Wrey0);
    free(Wrev);
    free(Wrey);
    free(Wreu);
}

void DigitalWaterMark::DwtMeaningAuthWM(double kx, double ky, double kz, int length, char *dstPath)
{
    int width=GetWidth(bmpfile);
    int height=GetHeight(bmpfile);

    file_open(bmpfile,"rb");
    file_ReadHead54();
    file_close();

    if(length>width*height)
    {
        printf("水印比特太多，最多为width*height！");
    }

    //定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
    int lineByte=width*3+width%4;
    int length1=lineByte * height;
    bmplen=length1;
    //申请位图数据所需要的空间，读位图数据进内存
    pBmpBuf=(unsigned char *)malloc(length1);
    readBmp(bmpfile,length1);

    Dwt(scale);

    int i,k,n;
    //水印嵌入到最低频的32x32空间
    BOOL * T;//图像最大为1024*1024
    T=(BOOL *)malloc(width*height*sizeof(BOOL));
    for(k=0;k<height;k++)
        for(n=0;n<width;n++)
            T[k*width+n]=false;
    for(n=0;n<100;n++)
    {
        kx=4*kx*(1-kx);
    }
    for(n=0;n<200;n++)
    {
        ky=4*ky*(1-ky);
    }
    if(kx==ky)ky=4*ky*(1-ky);//要保证kx不等于ky
    for(n=0;n<200;n++)
    {
        kz=4*kz*(1-kz);
    }
    int * wm_embed;
    wm_embed=(int *)malloc(length*sizeof(int));  //嵌入数据存放空间
    for(i=0;i<length;i++)
    {
        for(n=0;n<100;n++)
        {
            kz=4*kz*(1-kz);
        }
        if(kz>0.5)
            wm_embed[i]=1;
        else
            wm_embed[i]=0;
    }
    int number=0;
    double quan1;
    while(number<length)
    {
        for(i=0;i<100;i++)kx=4*kx*(1-kx);
        for(i=0;i<100;i++)ky=4*ky*(1-ky);
        k=(int)(height*kx);//此处是嵌入区域：px=int(32*kx)+64...
        n=(int)(width*ky);//
        quan1=quan;
        for(int sc=scale;sc>=1;sc--)
        {
            if((k<(height/(pow(2,sc)))) && (n<(width/(pow(2,sc)))))
            {
                quan1=pow(4,sc)*quan;
                break;
            }
        }

        if(T[k*width+n]==false)
        {
            T[k*width+n]=true;
            int dec;//注意：int(-13.6)=-13,和整数取整不一样
            if(Wrey[k*width+n]>=0){
                dec=(int)(Wrey[k*width+n]/quan1);
                if((dec%2)==wm_embed[number])
                {
                    Wrey[k*width+n]=dec*quan1+quan1/2.0;
                }
                else
                {
                    if((Wrey[k*width+n]-quan1*dec)>(quan1/2.0))
                        Wrey[k*width+n]=quan1*dec+3.0*quan1/2.0;  //此时有对图片嵌入处理
                    else
                        Wrey[k*width+n]=quan1*dec-quan1/2.0;
                }
            }
            else
            {
                dec=(int)(Wrey[k*width+n]/quan1)-1;
                int dec1=abs(dec%2);
                if(dec1==wm_embed[number])
                {
                    Wrey[k*width+n]=dec*quan1+quan1/2.0;
                }
                else
                {
                    if((Wrey[k*width+n]-quan1*dec)>(quan1/2.0))
                        Wrey[k*width+n]=quan1*dec+3.0*quan1/2.0;
                    else
                        Wrey[k*width+n]=quan1*dec-quan1/2.0;
                }
            }
            number++;
        }
    }

    Idwt();
    file_save(dstPath);

    free(wm_embed);
    free(T);
}

void DigitalWaterMark::DwtMeaningAuth(double kx, double ky, double kz, int length, char *dstPath)
{
    int width=GetWidth(bmpfile);
    int height=GetHeight(bmpfile);

    file_open(bmpfile,"rb");
    file_ReadHead54();
    file_close();

    if(length>width*height)
    {
        printf("水印比特太多，最多为width*height！");
    }
    //定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
    int lineByte=width*3+width%4;
    int length1=lineByte * height;
    bmplen=length1;
    //申请位图数据所需要的空间，读位图数据进内存
    pBmpBuf=(unsigned char *)malloc(length1);
    readBmp(bmpfile,length1);

    Dwt(scale);

    int i,k,n;
    //水印嵌入到最低频的32x32空间
    BOOL * T;//图像最大为1024*1024
    T=(BOOL *)malloc(width*height*sizeof(BOOL));
    for(k=0;k<height;k++)
       for(n=0;n<width;n++)
         T[k*width+n]=false;
    for(n=0;n<100;n++)
    {
        kx=4*kx*(1-kx);
    }
    for(n=0;n<200;n++)
    {
        ky=4*ky*(1-ky);
    }
    if(kx==ky)ky=4*ky*(1-ky);//要保证kx不等于ky
    for(n=0;n<200;n++)
    {
        kz=4*kz*(1-kz);
    }
    int * wm_embed;
    wm_embed=(int *)malloc(length*sizeof(int));
    unsigned char * wm_extract;
    wm_extract=(unsigned char *)malloc(height*width*sizeof(unsigned char));   //提取数据存放空间
    for(k=0;k<height;k++)
    {
        for(n=0;n<width;n++)
        {
            wm_extract[k*width+n]=0;
        }
    }
    for(i=0;i<length;i++)
    {
        for(n=0;n<100;n++)
        {
            kz=4*kz*(1-kz);
        }
        if(kz>0.5)
            wm_embed[i]=1;
        else
            wm_embed[i]=0;
    }
    int number=0;
    double quan1;
    double *taf;
    taf=(double *)malloc((scale+1)*sizeof(double));
    int *n_num;
    n_num=(int *)malloc((scale+1)*sizeof(int));
    memset(n_num,0,(scale+1)*sizeof(int));
    int *m_num;
    m_num=(int *)malloc((scale+1)*sizeof(int));
    memset(m_num,0,(scale+1)*sizeof(int));
    while(number<length)//
    {
        for(i=0;i<100;i++)kx=4*kx*(1-kx);
        for(i=0;i<100;i++)ky=4*ky*(1-ky);
        k=(int)(height*kx);//此处是嵌入区域：px=int(32*kx)+64...
        n=(int)(width*ky);//
        if(T[k*width+n]==false)
        {
            quan1=quan;
            for(int sc=scale;sc>=0;sc--)
            {
                if((k<(height/(pow(2,sc)))) && (n<(width/(pow(2,sc)))))
                {
                    quan1=pow(4,sc)*quan;
                    *(n_num+sc)=*(n_num+sc)+1;
                    break;
                }
            }

            T[k*width+n]=true;
            int dec;
            if(Wrey[k*width+n]>=0){
                dec=(int)(Wrey[k*width+n]/quan1);
                if(dec%2!=wm_embed[number])
                {
                    wm_extract[k*width+n]=255;
                    for(int sc=scale;sc>=0;sc--)
                    {
                        if((k<(height/(pow(2,sc)))) && (n<(width/(pow(2,sc)))))
                        {
                            *(m_num+sc)=*(m_num+sc)+1;
                            break;
                        }
                    }
                }
            }
            else
            {
                dec=(int)(Wrey[k*width+n]/quan1)-1;
                int dec1=abs(dec%2);
                if(dec1!=wm_embed[number])
                {
                    wm_extract[k*width+n]=255;
                    for(int sc=scale;sc>=0;sc--)
                    {
                        if((k<(height/(pow(2,sc)))) && (n<(width/(pow(2,sc)))))
                        {
                            *(m_num+sc)=*(m_num+sc)+1;
                            break;
                        }
                    }

                }
            }
            number++;
        }
    }
    int cor=0;
    for(k=0;k<height;k++)
    {
        for(n=0;n<width;n++)
        if(wm_extract[k*width+n]==255)cor=cor+1;
    }
    double watermark_cor;
    watermark_cor=((double)cor)/length;

    printf("TAF=%f\n",watermark_cor);

    for(i=0;i<=scale;i++)
    {
        *(taf+i)=*(m_num+i)/(double)(*(n_num+i));
    }

    for(k=0;k<height;k++)
    {
        for(n=0;n<width;n++)
        {
            if(wm_extract[k*width+n]==255)
            {
                *(pBmpBuf+k*lineByte+n*3+0)=255;
                *(pBmpBuf+k*lineByte+n*3+1)=255;
                *(pBmpBuf+k*lineByte+n*3+2)=255;
            }
            else
            {
                *(pBmpBuf+k*lineByte+n*3+0)=0;
                *(pBmpBuf+k*lineByte+n*3+1)=0;
                *(pBmpBuf+k*lineByte+n*3+2)=0;
            }
        }
    }
    int sca;
    for(sca=1;sca<=scale;sca++)
    {
        k=height/pow(2,sca);
        for(n=0;n<width/pow(2,sca-1);n++)
        {
            *(pBmpBuf+k*lineByte+n*3+0)=255;
            *(pBmpBuf+k*lineByte+n*3+1)=255;
            *(pBmpBuf+k*lineByte+n*3+2)=255;
        }
    }
    for(sca=1;sca<=scale;sca++)
    {
        n=width/pow(2,sca);
        for(k=0;k<height/pow(2,sca-1);k++)
        {
            *(pBmpBuf+k*lineByte+n*3+0)=255;
            *(pBmpBuf+k*lineByte+n*3+1)=255;
            *(pBmpBuf+k*lineByte+n*3+2)=255;
        }
    }

    file_save(dstPath);
    TamperAssessDlg dlg(NULL,scale,taf,dstPath);
    dlg.exec();

    free(wm_embed);
    free(wm_extract);
    free(T);
}

void DigitalWaterMark::setNoMeaningWkPara(int pscale, double pquan)
{
    scale=pscale;
    quan=pquan;
}

void DigitalWaterMark::setNoMeaningWkAuthPara(int pscale, double pquan, char* pauth_bmp_path)
{
    scale=pscale;
    quan=pquan;
    memset(auth_bmp_path,0,100);
    strncpy(auth_bmp_path, pauth_bmp_path, 100);
}

void DigitalWaterMark::setMeanWkPara(int pscale, double pquan, double pchaos_kx, double pchaos_ky, double pchaos_kz, int pwk_length)
{
    scale=pscale;
    quan=pquan;
    chaos_kx=pchaos_kx/100.0;
    chaos_ky=pchaos_ky/100.0;
    chaos_kz=pchaos_kz/100.0;
    wk_length=pwk_length;
}

void DigitalWaterMark::ProcessWaterMark(DWT_TYPE pType)
{
    switch(pType)
    {
        case NOMEANINGWK:
            DwtNoMeaningAuthWM("jin.bmp");
            break;
        case NOMEANINGWKAUTH:
            DwtNoMeaningAuth(auth_bmp_path, "wen.bmp");
            break;
        case MEANINGWK:
            DwtMeaningAuthWM(chaos_kx,chaos_ky,chaos_kz,wk_length, "jie.bmp");
            //新图片保存在 new_mean.bmp
            break;
        case MEANINGWKAUTH:
            DwtMeaningAuth(chaos_kx,chaos_ky,chaos_kz,wk_length, "wu.bmp");
            break;
        default:
            break;
    }
}
