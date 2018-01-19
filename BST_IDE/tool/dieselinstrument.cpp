#include "dieselinstrument.h"

DieselInstrument::DieselInstrument(QWidget *parent) :
    QWidget(parent)
{
}

void DieselInstrument::init()
{
    mValue= 0;
    update();
}

void DieselInstrument::setValue(int newvalue)
{
    mValue= newvalue;
    update();
}

void DieselInstrument::paintEvent(QPaintEvent *event)
{
    //设置指针的关键点
    static const QPoint hourHand[8] =
    {
        QPoint(4, -20),
        QPoint(4, -60),
        QPoint(1, -70),
        QPoint(1, -80),
        QPoint(-1, -80),
        QPoint(-1, -70),
        QPoint(-4, -60),
        QPoint(-4,-20)
    };
    // 设置颜色
    QColor pointer(200,0,0);
    QColor hourColor(255, 255, 255);
    QColor minuteColor(255,122,29);
    QColor textColor(255, 255, 255);

    int side = qMin(width(), height());                //取最小值
    QPainter tmPainter(this);
    tmPainter.setRenderHint(QPainter::Antialiasing, true);//消除锯齿
    tmPainter.setPen(Qt::black);
    tmPainter.setBrush(Qt::black);

    tmPainter.translate(width() / 2, height() / 2);       //将二分之一宽和高设为原点

    QRectF rect(-(side/2-6), -(side/2-6), side-12, side-12);
    int startAngle = -54 * 16;
    int spanAngle = (180+110) * 16;
    tmPainter.drawChord(rect, startAngle, spanAngle);

//    tmPainter.fillRect(rect(), QBrush(Qt::black));       //设置背景颜色
    tmPainter.scale(side / 300.0, side / 300.0);          //伸缩图片大小
    //设置指针笔形与填充颜色
    tmPainter.setPen(Qt::NoPen);
    tmPainter.setBrush(pointer);
    //设置时刻度
    tmPainter.save();
    tmPainter.setRenderHint(QPainter::Antialiasing,true);
    tmPainter.setPen(hourColor);
    for (int i = 0; i < 9; ++i)
    {
        tmPainter.drawLine(-53, 73, -62, 85);
        tmPainter.rotate(36.0);
    }
    tmPainter.restore();
    //设置分刻度
    tmPainter.save();
    tmPainter.setRenderHint(QPainter::Antialiasing,true);
    tmPainter.setPen(minuteColor);
    for (int j = 0; j < 8; ++j)
    {
        tmPainter.drawLine(-73, 53, -85, 62);
        tmPainter.rotate(36.0);
    }
    tmPainter.restore();
    //设置刻度值
    tmPainter.setPen(textColor);
    tmPainter.drawText(-61, 57,30, 30,Qt::AlignHCenter | Qt::AlignTop,QString::number(0));//从（x,y）开始,高为30,宽为30
    tmPainter.drawText(-90, 15,30, 30,Qt::AlignHCenter | Qt::AlignTop,QString::number(10));
    tmPainter.drawText(-90, -35,30, 30,Qt::AlignHCenter | Qt::AlignTop,QString::number(20));
    tmPainter.drawText(-62, -71,30, 30,Qt::AlignHCenter | Qt::AlignTop,QString::number(30));
    tmPainter.drawText(-15, -90,30, 30,Qt::AlignHCenter | Qt::AlignTop,QString::number(40));
    tmPainter.drawText(32, -71,30, 30,Qt::AlignHCenter | Qt::AlignTop,QString::number(50));
    tmPainter.drawText(62, -31,30, 30,Qt::AlignHCenter | Qt::AlignTop,QString::number(60));
    tmPainter.drawText(60, 20,30, 30,Qt::AlignHCenter | Qt::AlignTop, QString::number(70));
    tmPainter.drawText(35, 55,30, 30,Qt::AlignHCenter | Qt::AlignTop, QString::number(80));
    //内圆
    tmPainter.save();
    tmPainter.setRenderHint(QPainter::Antialiasing,true);
    tmPainter.setPen(QPen(Qt::white,2,Qt::SolidLine));
    tmPainter.setBrush(QBrush(Qt::NoBrush));
    tmPainter.drawEllipse(-20,-20,40,40);
    tmPainter.restore();
    //画指针
    tmPainter.save();
    tmPainter.setPen(Qt::NoPen);
    tmPainter.setRenderHint(QPainter::Antialiasing,true);
    tmPainter.rotate(180+36+mValue*288/80);
    tmPainter.drawConvexPolygon(hourHand, 8);
    tmPainter.restore();
    //最外面圆弧
    tmPainter.save();
    tmPainter.setPen(QPen(Qt::red,3,Qt::SolidLine));
    tmPainter.setRenderHint(QPainter::Antialiasing,true);
    tmPainter.drawArc(-105,-105,210,210,-54*16,72*16);
    tmPainter.restore();
    tmPainter.save();
    tmPainter.setPen(QPen(Qt::yellow,3,Qt::SolidLine));
    tmPainter.setRenderHint(QPainter::Antialiasing,true);
    tmPainter.drawArc(-105,-105,210,210,18*16,144*16);
    tmPainter.restore();
    tmPainter.save();
    tmPainter.setPen(QPen(Qt::white,3,Qt::SolidLine));
    tmPainter.setRenderHint(QPainter::Antialiasing,true);
    tmPainter.drawArc(-105,-105,210,210,162*16,72*16);
    tmPainter.restore();
    //内圆内数字
    QFont font;
    font.setPointSize(12);
    setFont(font);

    tmPainter.drawText(-13, -11,30, 30,Qt::AlignHCenter | Qt::AlignTop,QString::number(mValue));
}


