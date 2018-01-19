#include "graphicskeyboard.h"
#include "graphicsregion.h"

GraphicsKey::GraphicsKey(QGraphicsItem *parent, QRectF pRect) :
    GraphicsBase(parent, pRect)
{
    m_KeyState = PIC_DARK;
    m_CurTypeIndex = -1;
}

bool GraphicsKey::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    else if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        RESOURCE_INFO tmpRc;
        int index;
        int count = pEffect->Effect.RcFiles.count();
        for(int i=0;i<count;i++)
        {
            tmpRc = pEffect->Effect.RcFiles.at(i);
            index = GetKey(tmpRc.RcName);
            IDE_TRACE_INT(index);
            if(LoadPath(index, tmpRc.RcFile) == false)
            {
                IDE_TRACE_STR(tmpRc.RcFile);
            }
        }
    }
    else if(tmpString.compare(QString("Press"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Spread"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeSpread;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Rotate"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRotate;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Wheel"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeWheel;
        }
        else if(pEffect->Effect.EffectType.compare(QString("SlipCycle"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeSlipCycle;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_KEYPRESS, tmpAreaAnimate);
    }
    else if(tmpString.compare(QString("Release"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Spread"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeSpread;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Rotate"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRotate;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Wheel"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeWheel;
        }
        else if(pEffect->Effect.EffectType.compare(QString("SlipCycle"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeSlipCycle;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_KEYRELEASE, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool GraphicsKey::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return GraphicsBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool GraphicsKey::Start()
{
    if(GraphicsBase::Start() == false)
        return false;
    UpdateCom(0);
    return true;
}

bool GraphicsKey::PaintEffect(QPainter *p)
{
    if(m_EffectValid == false)
        return false;
    if(m_AreaAnimate == 0)
    {
        //>@首先绘制按钮底图
        DrawPixmap(p, GetPixmap((m_KeyState == PIC_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
        //>@最终绘制文字
        DrawPixmap(p, GetPixmap((m_KeyState == PIC_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
        return false;
    }
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeSpread:
        {
            //>@首先绘制按钮底图
            DrawPixmap(p, GetPixmap((m_KeyState == PIC_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
            //>@其次特效绘图
            m_PixmapPointer.mLastPointer = GetPixmap(PIC_EFFECT);
            m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer;
            Paint2D_Spread(p, rect());
            //>@最终绘制文字
            DrawPixmap(p, GetPixmap((m_KeyState == PIC_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
            break;
        }
        case EffectTypeWheel:
        {
            //>@首先绘制按钮底图
            DrawPixmap(p, GetPixmap((m_KeyState == PIC_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
            //>@其次特效绘图
            m_PixmapPointer.mLastPointer = GetPixmap(PIC_EFFECT);
            m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer;
            Paint2D_Wheel(p, rect());
            //>@最终绘制文字
            DrawPixmap(p, GetPixmap((m_KeyState == PIC_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
            break;
        }
        case EffectTypeRotate:
        {
            QImage tmpImage(size().toSize(), QImage::Format_ARGB32_Premultiplied);
            if(tmpImage.isNull())
                return false;
            tmpImage.fill(Qt::transparent);
            QPainter painter(&tmpImage);
            //>@首先绘制按钮底图
            DrawPixmap(p, GetPixmap((m_KeyState == PIC_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
            //>@绘制文字
            DrawPixmap(p, GetPixmap((m_KeyState == PIC_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
            painter.end();
            //>@最终特效绘图
            m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer = QPixmap::fromImage(tmpImage);
            Paint2D_Rotate(p, rect());
            break;
        }
        case EffectTypeSlipCycle:
        {
            //>@首先绘制按钮底图
            DrawPixmap(p, GetPixmap((m_KeyState == PIC_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
            //>@其次特效绘图
            m_PixmapPointer.mLastPointer = GetPixmap(PIC_EFFECT);
            m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer;
            Paint2D_Slipcycle(p, rect());
            //>@最终绘制文字
            DrawPixmap(p, GetPixmap((m_KeyState == PIC_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
            break;
        }
        default:
            Paint2D_None(p, rect());
            break;
    }
    return true;
}

void GraphicsKey::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_KEYPRESS:
        {
            if(m_KeyState == PIC_LIGHT)
                return;
            m_KeyState = PIC_LIGHT;
            break;
        }
        case OPERATE_KEYRELEASE:
        {
            if(m_KeyState == PIC_DARK)
                return;
            m_KeyState = PIC_DARK;
            break;
        }
        default:
            return;
    }
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    m_OperateInfo[STEP0].mValid = true;
    m_OperateInfo[STEP0].mOperate = pOperate;
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = OPERATE_NONE;
    //>@执行STEP0中的内容
    OperateStep0();

    if(m_KeyState == PIC_LIGHT)
    {
        emit reportEffect(OPERATE_KEYPRESS, QVariant(GetCurType()));
    }
    else if(m_KeyState = PIC_DARK)
    {
        emit reportEffect(OPERATE_KEYRELEASE, QVariant(GetCurType()));
    }
}

//#################################################################################
//#################################################################################

GraphicsLCD::GraphicsLCD(QGraphicsItem *parent, QRectF pRect) :
    GraphicsBase(parent, pRect)
{
    m_FloorAsciiList = 0;
    m_FloorAsciiMax = 0;
    m_FloorAsciiIndex = 0;
    m_DialNumIndex = 0;
    m_IconRectValid = false;
}

bool GraphicsLCD::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    else if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        RESOURCE_INFO tmpRc;
        int index;
        int count = pEffect->Effect.RcFiles.count();
        for(int i=0;i<count;i++)
        {
            tmpRc = pEffect->Effect.RcFiles.at(i);
            index = GetKey(tmpRc.RcName);
            IDE_TRACE_INT(index);
            if(LoadPath(index, tmpRc.RcFile) == false)
            {
                IDE_TRACE_STR(tmpRc.RcFile);
            }
        }
    }
    else
    {
        return false;
    }
    return true;
}

bool GraphicsLCD::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return GraphicsBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool GraphicsLCD::Start()
{
    m_EffectValid = true;
    qreal width = size().width();
    qreal height = size().height();
    switch(m_Pattern)
    {
        case PTN_KB_YUNGTAY:
        {
            //>@计算楼层显示区域
            m_FloorRect = QRectF(0, height/10.0, width, height*3.0/4.0);
            int tmpFontSize = CalcFontSizeFromSize("Arial", QSize(m_FloorRect.width(), m_FloorRect.height()), PRIOR_HEIGHT);
            m_FloorFont = QFont(QString("Arial"), tmpFontSize);
            //>@修改参数
            height = height - m_FloorRect.bottom();
            //>@计算按键回显区域
            m_DialNumRect = QRectF(width/3.0, m_FloorRect.bottom(), width/3.0, height*3.0/5.0);
            //>@计算特殊图标显示区域
            m_IconRect = QRectF(width/9.0, m_FloorRect.bottom(), width/9.0, height*3.0/5.0);
            break;
        }
        default:
        {
            //>@计算按键回显区域
            int tmpFontSize = CalcFontSizeFromSize("Arial", QSize(size().width(), size().height()*3.0/5.0), PRIOR_HEIGHT);
            m_DialNumFont = QFont(QString("Arial"), tmpFontSize);
            QFontMetricsF fm(m_DialNumFont);
            QSizeF tmpSize = fm.size(Qt::TextSingleLine, QString("AAA"));
            m_DialNumRect = QRectF(width*9.0/10.0-tmpSize.width(), height/5.0, tmpSize.width(), height*3.0/5.0);
            //>@计算楼层显示区域
            m_FloorRect = QRectF(width/10.0, height/10.0, m_DialNumRect.left()-width/5.0, height*4.0/5.0);
            tmpFontSize = CalcFontSizeFromSize("Arial", QSize(m_FloorRect.width(), m_FloorRect.height()/2.0), PRIOR_HEIGHT);
            m_FloorFont = QFont(QString("Arial"), tmpFontSize);
            break;
        }
    }
    return true;
}

QPixmap GraphicsLCD::GetFloorPixmap()
{
    QPixmap tmpPixmap;
    switch(m_Pattern)
    {
        case PTN_KB_YUNGTAY:
        {
            if(m_FloorRect.isEmpty())
                return QPixmap();
            QImage tmpImage(m_FloorRect.size().toSize(), QImage::Format_ARGB32_Premultiplied);
            if(tmpImage.isNull())
                return QPixmap();
            tmpImage.fill(Qt::transparent); //>@创建透明图层
            if(m_FloorAsciiIndex > 0)
            {
                qreal w = m_FloorRect.width();
                qreal h = m_FloorRect.height();
                QRectF tmpRect = m_FloorRect.adjusted(w/5.0, h/5.0, -w/5.0, -h/5.0);
                QPainter p(&tmpImage);
                int len = strlen(m_FloorAsciiList);
                qreal asciih, asciiw;
                int row = 1, column = 0;
                qreal totalw, totalh;
                while(1)
                {
                    asciih = tmpRect.height()/row;
                    asciiw = asciih * 3.0 / 4.0;
                    column = len / row;
                    totalw = column * asciiw;
                    if(totalw <= tmpRect.width())
                    {
                        break;
                    }
                    else if(len == 1)
                    {
                        asciiw = tmpRect.width();
                        asciih = asciiw * 4.0 / 3.0;
                        break;
                    }
                    row ++;
                }
                totalh = row * asciih;
                p.translate(m_FloorRect.width()/2.0-totalw/2.0, m_FloorRect.height()/2.0-totalh/2.0);
                for(int i=0;i<row;i++)
                {
                    for(int j=0;j<column;j++)
                    {
                        QRectF tmpRect = QRectF(j*asciiw, i*asciih, asciiw, asciih);
                        p.drawPixmap(tmpRect.toRect(), GetPixmap((int)(m_FloorAsciiList[i*column+j])));
                    }
                }
                p.end();
            }
            tmpPixmap = QPixmap::fromImage(tmpImage);
            break;
        }
        default:
        {
            if(m_FloorRect.isEmpty())
                return QPixmap();
            QImage tmpImage(m_FloorRect.size().toSize(), QImage::Format_ARGB32_Premultiplied);
            if(tmpImage.isNull())
                return QPixmap();
            tmpImage.fill(Qt::transparent); //>@创建透明图层
            if(m_FloorAsciiIndex > 0)
            {
                QPainter p(&tmpImage);
                QLatin1String tmpNum(m_FloorAsciiList);
                p.setFont(m_FloorFont);
                QFontMetrics fm(m_FloorFont);
                QRectF textRect =  fm.boundingRect(tmpNum);
                p.translate(m_FloorRect.width()/2.0, m_FloorRect.height()/2.0);
                p.drawText(QRect(-textRect.width()/2.0, -textRect.height()/2.0, textRect.width(), textRect.height()), tmpNum);
                p.end();
            }
            tmpPixmap = QPixmap::fromImage(tmpImage);
            break;
        }
    }
    return tmpPixmap;
}

QPixmap GraphicsLCD::GetDialPixmap()
{
    QPixmap tmpPixmap;
    switch(m_Pattern)
    {
        case PTN_KB_YUNGTAY:
        {
            if(m_DialNumRect.isEmpty())
                return QPixmap();
            QImage tmpImage(m_DialNumRect.size().toSize(), QImage::Format_ARGB32_Premultiplied);
            if(tmpImage.isNull())
                return QPixmap();
            tmpImage.fill(Qt::transparent); //>@创建透明图层
            if(m_DialNumIndex > 0)
            {
                int diffw = m_DialNumRect.width()/3.0;
                int diffh = m_DialNumRect.height();
                QPainter p(&tmpImage);
                if(m_DialNumIndex == 1)
                {
                    p.drawPixmap(QRect(diffw*2, 0, diffw, diffh), GetPixmap((int)(m_DialNumList[0])));
                }
                else if(m_DialNumIndex == 2)
                {
                    p.drawPixmap(QRect(diffw, 0, diffw, diffh), GetPixmap((int)(m_DialNumList[0])));
                    p.drawPixmap(QRect(diffw*2, 0, diffw, diffh), GetPixmap((int)(m_DialNumList[1])));
                }
                else if(m_DialNumIndex == 3)
                {
                    p.drawPixmap(QRect(0, 0, diffw, diffh), GetPixmap((int)(m_DialNumList[0])));
                    p.drawPixmap(QRect(diffw, 0, diffw, diffh), GetPixmap((int)(m_DialNumList[1])));
                    p.drawPixmap(QRect(diffw*2, 0, diffw, diffh), GetPixmap((int)(m_DialNumList[2])));
                }
                p.end();
            }
            tmpPixmap = QPixmap::fromImage(tmpImage);
            break;
        }
        default:   //>@使用默认外观，居中显示字体
        {
            if(m_DialNumRect.isEmpty())
                return QPixmap();
            QImage tmpImage(m_DialNumRect.size().toSize(), QImage::Format_ARGB32_Premultiplied);
            if(tmpImage.isNull())
                return QPixmap();
            tmpImage.fill(Qt::transparent); //>@创建透明图层
            if(m_DialNumIndex > 0)
            {
                QPainter p(&tmpImage);
                QLatin1String tmpNum(m_DialNumList);
                p.setFont(m_DialNumFont);
                QFontMetrics fm(m_DialNumFont);
                QRectF textRect =  fm.boundingRect(tmpNum);
                p.translate(m_DialNumRect.width()/2.0, m_DialNumRect.height()/2.0);
                p.drawText(QRect(-textRect.width()/2.0, -textRect.height()/2.0, textRect.width(), textRect.height()), tmpNum);
                p.end();
            }
            tmpPixmap = QPixmap::fromImage(tmpImage);
            break;
        }
    }
    return tmpPixmap;
}

QPixmap GraphicsLCD::GetIconPixmap()
{
    return GetPixmap(RCDISABILITY); //Disability图片
}

QPixmap GraphicsLCD::GetBgImage()
{
    QPixmap tmpPixmap = GetPixmap(BGRC);
    if(tmpPixmap.isNull() == false)
        return tmpPixmap;
    switch(m_Pattern)
    {
        case PTN_KB_YUNGTAY:
        {
            break;
        }
        default:   //>@使用默认外观
        {
            QImage tmpImage(size().width(), size().height(), QImage::Format_ARGB32_Premultiplied);
            tmpImage.fill(Qt::transparent); //>@创建透明图层
            QPainter p(&tmpImage);
            p.setRenderHint(QPainter::SmoothPixmapTransform, true);
            p.setRenderHint(QPainter::Antialiasing);
            QRectF tmpRect = rect();
            //>@绘制进度条外框
            p.save();
            p.setPen(Qt::NoPen);
            p.setBrush(QBrush(QColor(55,70,79)));
            p.drawRoundedRect(tmpRect, 25, 25);
            p.restore();
            //>@绘制进度条底图及顶图
            p.save();
            QLinearGradient pgBaseGradient(QPointF(0,1), QPointF(0,size().height()-2));
            pgBaseGradient.setColorAt(0, QColor(241,241,241,255));
            pgBaseGradient.setColorAt(0.5, QColor(202,202,202,255));
            pgBaseGradient.setColorAt(1, QColor(241,241,241,255));//设置渐变的颜色和路径比例
            p.setPen(Qt::NoPen);
            p.setBrush(QBrush(pgBaseGradient));
            tmpRect.adjust(2, 2, -2, -2);
            p.drawRoundedRect(tmpRect, 25, 25);
            p.restore();
            p.end();
            tmpPixmap = QPixmap::fromImage(tmpImage);
            m_EffectImage.insert(BGRC, tmpPixmap);
            break;
        }
    }
    return tmpPixmap;
}

quint32 GraphicsLCD::GetMappedFloor(quint32 pFloor)
{
    //>@根据映射表映射为另一个楼层
    quint32 tmpFloor = pFloor;

    return tmpFloor;
}

void GraphicsLCD::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_KEYPRESS:
        {
            quint32 tmpAscii = (quint32)(pPara.toInt());
            int disabilitykey = 0x18;
            int enterkey = (QTASCII)?(Qt::Key_Enter):(0x0d);
            int delkey = (QTASCII)?(Qt::Key_Escape):(0x1b);
            if(tmpAscii == disabilitykey)     //disability
            {
                m_IconRectValid = true;
            }
            else if(tmpAscii == enterkey)     //Enter
            {
                m_IconRectValid = false;
                //>@表示确认楼层
                quint32 tmpFloor;
                if(m_DialNumIndex == 1)
                {
                    tmpFloor = m_DialNumList[0];
                }
                else if(m_DialNumIndex == 2)
                {
                    tmpFloor = ((quint32)m_DialNumList[0])<<8 + m_DialNumList[1];
                }
                else if(m_DialNumIndex == 3)
                {
                    tmpFloor = ((quint32)m_DialNumList[0])<<16 + ((quint32)m_DialNumList[1])<<8 + m_DialNumList[2];
                }
                else
                {
                    update();
                    return;
                }
                emit reportEffect(OPERATE_CHANGE, QVariant(tmpFloor));
                m_DialNumIndex = 0;
                update();
            }
            else if(tmpAscii >= 0x20 && tmpAscii <= 0x7e)
            {
                if(m_DialNumIndex >= MAX_DIALCOUNT)
                    return;
                m_DialNumList[m_DialNumIndex++] = tmpAscii;
                m_DialNumList[m_DialNumIndex] = 0;
                emit reportEffect(OPERATE_CHANGE, QVariant(INVALIDRC));
            }
            else if(tmpAscii == delkey)  //DEL
            {
                if(m_DialNumIndex == 0)
                    return;
                m_DialNumIndex --;
                m_DialNumList[m_DialNumIndex] = '\0';
            }
            m_PixmapPointer.mCurPointer = GetDialPixmap();
            break;
        }
        case OPERATE_CHANGE:  //>@已按的楼层数改变
        {
            QList<QVariant> tmpList = pPara.toList();
            m_FloorAsciiMax = tmpList.count() * 4;
            if(m_FloorAsciiList)
                free(m_FloorAsciiList);
            m_FloorAsciiIndex = 0;
            if(m_FloorAsciiMax <= 0)
            {
                m_FloorAsciiList[m_FloorAsciiIndex] = '\0';
                return;
            }
            m_FloorAsciiList = (char*)malloc(m_FloorAsciiMax+1);
            for(int i=0;i<tmpList.count();i++)
            {
                quint32 tmpNewFloor = GetMappedFloor(tmpList.at(i).toInt());
                m_FloorAsciiList[m_FloorAsciiIndex++] = (tmpNewFloor>>16)&0xff;
                m_FloorAsciiList[m_FloorAsciiIndex++] = (tmpNewFloor>>8)&0xff;
                m_FloorAsciiList[m_FloorAsciiIndex++] = (tmpNewFloor)&0xff;
                m_FloorAsciiList[m_FloorAsciiIndex++] = ';';
            }
            m_FloorAsciiList[m_FloorAsciiIndex-1] = '\0';
            m_PixmapPointer.mLastPointer = GetFloorPixmap();
            break;
        }
        case OPERATE_ADD:  //>@添加一个已按的楼层
        {
            m_FloorAsciiMax = 1024;
            if(m_FloorAsciiList == 0)
            {
                m_FloorAsciiIndex = 0;
                m_FloorAsciiList = (char*)malloc(m_FloorAsciiMax+1);
            }
            char tmpFloorAsciiList[4];
            quint32 tmpNewFloor = GetMappedFloor(pPara.toInt());
            tmpFloorAsciiList[0] = (tmpNewFloor>>16)&0xff;
            tmpFloorAsciiList[1] = (tmpNewFloor>>8)&0xff;
            tmpFloorAsciiList[2] = (tmpNewFloor)&0xff;
            tmpFloorAsciiList[3] = '\0';
            //>@查找当前已有字符串中是否含有待添加的字符串
            char* tmpRet = strstr(m_FloorAsciiList, tmpFloorAsciiList);
            if(tmpRet == NULL)  //>@找不到
            {
                m_FloorAsciiList[m_FloorAsciiIndex++] = ';';
                m_FloorAsciiList[m_FloorAsciiIndex++] = tmpFloorAsciiList[0];
                m_FloorAsciiList[m_FloorAsciiIndex++] = tmpFloorAsciiList[1];
                m_FloorAsciiList[m_FloorAsciiIndex++] = tmpFloorAsciiList[2];
            }
            else
            {
                return;
            }
            m_FloorAsciiList[m_FloorAsciiIndex-1] = '\0';
            m_PixmapPointer.mLastPointer = GetFloorPixmap();
            break;
        }
        case OPERATE_DEL:  //>@删除一个已按的楼层
        {
            if(m_FloorAsciiIndex <= 0)
                return;
            char tmpFloorAsciiList[4];
            quint32 tmpNewFloor = GetMappedFloor(pPara.toInt());
            tmpFloorAsciiList[0] = (tmpNewFloor>>16)&0xff;
            tmpFloorAsciiList[1] = (tmpNewFloor>>8)&0xff;
            tmpFloorAsciiList[2] = (tmpNewFloor)&0xff;
            tmpFloorAsciiList[3] = '\0';
            //>@查找当前已有字符串中是否含有待添加的字符串
            char* tmpRet = strstr(m_FloorAsciiList, tmpFloorAsciiList);
            if(tmpRet)  //>@找到了
            {
                strcpy(tmpRet, tmpRet+4);
            }
            else
            {
                return;
            }
            m_PixmapPointer.mLastPointer = GetFloorPixmap();
            break;
        }
        case OPERATE_SHOW:   //>@单独显示某一个字符串
        {
            //>@保护现场
            char *oldFloorAsciiList = m_FloorAsciiList;
            int oldIndex = m_FloorAsciiIndex;
            //>@获取楼层图像
            QByteArray tmpArray = pPara.toString().toLatin1();
            m_FloorAsciiList = (char*)(tmpArray.data());
            m_FloorAsciiIndex = strlen(m_FloorAsciiList);
            m_PixmapPointer.mLastPointer = GetFloorPixmap();
            //>@恢复现场
            m_FloorAsciiList = oldFloorAsciiList;
            m_FloorAsciiIndex = oldIndex;
            break;
        }
        default:
            return;
    }
    update();
}


bool GraphicsLCD::PaintEffect(QPainter *p)
{
    DrawPixmap(p, GetBgImage(), rect());
    //>@绘制特殊图标显示
    if(m_IconRectValid)
        DrawPixmap(p, GetIconPixmap(), m_IconRect);
    //>@绘制楼层显示
    DrawPixmap(p, m_PixmapPointer.mLastPointer, m_FloorRect);
    //>@绘制按键显示
    DrawPixmap(p, m_PixmapPointer.mCurPointer, m_DialNumRect);
    return true;
}

//#################################################################################
//#################################################################################

GraphicsKeyboard::GraphicsKeyboard(QGraphicsItem *parent, QRectF pRect) :
    GraphicsBase(parent, pRect)
{
    m_LCD = 0;
}

bool GraphicsKeyboard::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        if(pEffect->Effect.EffectType.compare(QString("Image"), Qt::CaseInsensitive) == 0)
        {
            m_RcType = RCIMAGE;
            analysisEffectPara((AREA_ANIMATE *)0, pEffect->Effect.EffectPara);
            RESOURCE_INFO tmpRc;
            int index;
            int count = pEffect->Effect.RcFiles.count();
            for(int i=0;i<count;i++)
            {
                tmpRc = pEffect->Effect.RcFiles.at(i);
                index = GetKey(tmpRc.RcName);
                IDE_TRACE_INT(index);
                if(LoadPath(index, tmpRc.RcFile) == false)
                {
                    IDE_TRACE_STR(tmpRc.RcFile);
                }
            }
        }
    }
    else if(tmpString.compare(QString("Press"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Spread"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeSpread;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Rotate"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRotate;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Wheel"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeWheel;
        }
        else if(pEffect->Effect.EffectType.compare(QString("SlipCycle"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeSlipCycle;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_KEYPRESS, tmpAreaAnimate);
    }
    else if(tmpString.compare(QString("Release"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Spread"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeSpread;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Rotate"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRotate;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Wheel"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeWheel;
        }
        else if(pEffect->Effect.EffectType.compare(QString("SlipCycle"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeSlipCycle;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_KEYRELEASE, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool GraphicsKeyboard::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    GraphicsBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
    if(pLabel.compare(PARA_PATTERN, Qt::CaseInsensitive) == 0)
    {
        if(pContent == QString("QWERTY"))
            m_Pattern = PTN_KB_QWERTY;
        else if(pContent == QString("GRID_H"))
            m_Pattern = PTN_KB_GRID_H;
        else if(pContent == QString("GRID_V"))
            m_Pattern = PTN_KB_GRID_V;
        else if(pContent == QString("DISEN"))
            m_Pattern = PTN_KB_DISEN;
        else if(pContent == QString("YUNGTAY"))
        {
            m_Pattern = PTN_KB_YUNGTAY;
            m_LCDHideTimer.setSingleShot(true);
            connect(&m_LCDHideTimer, SIGNAL(timeout()), this, SLOT(HideLCD()));
            m_LCDHideTimer.setInterval(3000);
        }
    }
    else if(pLabel.compare(PARA_RSPACE, Qt::CaseInsensitive) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            mRSpace = dec;
    }
    else if(pLabel.compare(PARA_CSPACE, Qt::CaseInsensitive) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            mCSpace = dec;
    }
    return true;
}

void GraphicsKeyboard::ReleaseKeyboardBtnInfoList()
{
    for(int i=m_KeyboardBtnInfoList.count()-1;i>=0;i--)
    {
        KeyboardBtnInfo *tmpKeyboardBtnInfo = m_KeyboardBtnInfoList.takeAt(i);
        if(tmpKeyboardBtnInfo == 0)
            continue;
        delete tmpKeyboardBtnInfo;
    }
}

bool GraphicsKeyboard::AddLCD(QRectF pRect)
{
    if(m_LCD)
    {
        disconnect(m_LCD, SIGNAL(reportEffect(AREA_OPERATE,QVariant)), 0, 0);
        m_LCD->deleteLater();
    }
    m_LCD = new GraphicsLCD(this, pRect);
    m_LCD->SetPattern(m_Pattern);
    QHash<int, QPixmap> tmpRcPixmaps;
    tmpRcPixmaps.insert(0, GetPixmap(200));
    tmpRcPixmaps.insert(24, GetPixmap(224));
    tmpRcPixmaps.insert(45, GetPixmap(245));
    tmpRcPixmaps.insert(48, GetPixmap(248));
    tmpRcPixmaps.insert(49, GetPixmap(249));
    tmpRcPixmaps.insert(50, GetPixmap(250));
    tmpRcPixmaps.insert(51, GetPixmap(251));
    tmpRcPixmaps.insert(52, GetPixmap(252));
    tmpRcPixmaps.insert(53, GetPixmap(253));
    tmpRcPixmaps.insert(54, GetPixmap(254));
    tmpRcPixmaps.insert(55, GetPixmap(255));
    tmpRcPixmaps.insert(56, GetPixmap(256));
    tmpRcPixmaps.insert(57, GetPixmap(257));
    tmpRcPixmaps.insert(65, GetPixmap(265));
    tmpRcPixmaps.insert(66, GetPixmap(266));
    tmpRcPixmaps.insert(67, GetPixmap(267));
    tmpRcPixmaps.insert(68, GetPixmap(268));
    tmpRcPixmaps.insert(69, GetPixmap(269));
    tmpRcPixmaps.insert(70, GetPixmap(270));
    tmpRcPixmaps.insert(71, GetPixmap(271));
    tmpRcPixmaps.insert(72, GetPixmap(272));
    m_LCD->InitEffect(tmpRcPixmaps, m_EffectGroup);
    connect(m_LCD, SIGNAL(reportEffect(AREA_OPERATE,QVariant)), this, SLOT(collectReport(AREA_OPERATE,QVariant)));
    return m_LCD->Start();
}

bool GraphicsKeyboard::AddButton(QString pKeys, GraphicsKey *&pButton, QRectF pRect)
{
    if(pButton)
        return true;
    pButton = new GraphicsKey(this, pRect);
    QHash<int, QPixmap> tmpRcPixmaps;
    tmpRcPixmaps.insert(PIC_DARK, GetPixmap(PIC_DARK));
    tmpRcPixmaps.insert(PIC_LIGHT, GetPixmap(PIC_LIGHT));
    tmpRcPixmaps.insert(PIC_EFFECT, GetPixmap(PIC_EFFECT));
    QStringList tmpList = pKeys.split(' ');
    QList<quint32> tmpTypeList;
    for(int i=0; i<tmpList.count(); i++)
    {
        quint32 tmpType = GetKeyType(tmpList.at(i));
        tmpTypeList.append(tmpType);
        tmpRcPixmaps.insert(TEXT_DARK+i*100, GetPixmap(tmpType));
        tmpRcPixmaps.insert(TEXT_LIGHT+i*100, GetPixmap(tmpType+100));
    }
    pButton->SetTypeList(tmpTypeList);
    pButton->InitEffect(tmpRcPixmaps, m_EffectGroup);
    //>@建立信号连接
    connect(this, SIGNAL(sAlternateType()), pButton, SLOT(SetAlternateType()));
    connect(pButton, SIGNAL(reportEffect(AREA_OPERATE,QVariant)), this, SLOT(collectReport(AREA_OPERATE,QVariant)));
    return true;
}

bool GraphicsKeyboard::InitKeyboardBtnInfoList()
{
    ReleaseKeyboardBtnInfoList();
    switch(m_Pattern)
    {
//        case PTN_KB_QWERTY:
//        {
//            KEYINFO tmpKeyLetters[]=
//            {
//                {"Esc", 1},    {"`", 1},  {"1 !", 1},{"2 @", 1},  {"3 #", 1},{"4 $", 1}, {"5 %", 1},  {"6 ^", 1},  {"7 &", 1},  {"8 *", 1},  {"9 (", 1},  {"0 )", 1},  {"- _", 1},    {"= +", 1},    {"Backspace", 2},   //>@15
//                {"Tab", 1.5},  {"q", 1},  {"w", 1},  {"e", 1},    {"r", 1},  {"t", 1},   {"y", 1},    {"u", 1},    {"i", 1},    {"o", 1},    {"p", 1},    {"[ {", 1},  {"] }", 1},    {"\\ }", 1},   {"Del", 1.5},  //>@15
//                {"Caps", 1.7}, {"a", 1},  {"s", 1},  {"d", 1},    {"f", 1},  {"g", 1},   {"h", 1},    {"j", 1},    {"k", 1},    {"l", 1},    {";  :", 1}, {"\'", 1},   {"Enter", 2.3},                              //>@13
//                {"Shift", 3},  {"z", 1},  {"x", 1},  {"c", 1},    {"v", 1},  {"b", 1},   {"n", 1},    {"m", 1},    {", <", 1},  {". >", 1},  {"/ ?", 1},  {"Up", 1},   {"Home", 1.5}, {"End", 1.5},                 //>@14
//                {"Ctrl", 1},   {"Win", 1},{"Alt", 1},{"Space", 8},{"EN", 1}, {"Left", 1},{"Down", 1}, {"Right", 1},{"Hide", 1}                                                                                      //>@9
//            };
//            qreal width = size().width();
//            qreal height = size().height();
//            int count = sizeof(tmpKeyLetters)/sizeof(KEYINFO);
//            int ratioTotalForRow = 16;  //>@1行共有16
//            int row = 5;
//            qreal tmpWidthDiff = width / (qreal)ratioTotalForRow;
//            int h = height / (qreal)row;
//            int x=0, y=0, w=0;
//            for(int i=0; i<count; i++)
//            {
//                if(i == 0)             //>@Row1
//                {
//                    x=0; y=0; w=0;
//                }
//                else if(i == 15)       //>@Row2
//                {
//                    x=0; y=h; w=0;
//                }
//                else if(i == 30)       //>@Row3
//                {
//                    x=0; y=2*h; w=0;
//                }
//                else if(i == 43)       //>@Row4
//                {
//                    x=0; y=3*h; w=0;
//                }
//                else if(i == 57)       //>@Row5
//                {
//                    x=0; y=4*h; w=0;
//                }
//                KeyboardBtnInfo *tmpKeyboardBtnInfo = new KeyboardBtnInfo;
//                w = tmpKeyLetters[i].mLenRatio * tmpWidthDiff;
//                tmpKeyboardBtnInfo->mRect = QRectF(x+mCSpace, y+mRSpace, w-2*mCSpace, h-2*mRSpace);
//                x += w;
//                if(tmpKeyLetters[i].mLabel.compare(QString("LCD")) == 0)
//                {
//                    AddLCD(tmpKeyboardBtnInfo->mRect);
//                    delete tmpKeyboardBtnInfo;
//                }
//                else
//                {
//                    AddButton(tmpKeyLetters[i].mLabel, tmpKeyboardBtnInfo->mButtonItem, tmpKeyboardBtnInfo->mRect);
//                    m_KeyboardBtnInfoList.append(tmpKeyboardBtnInfo);
//                }
//            }
//            break;
//        }
//        case PTN_KB_GRID_H:
//        {
//            KEYINFO tmpKeyLetters[]=
//            {
//                {"7", 1},  {"8", 1},   {"9", 1},  //0
//                {"4", 1},  {"5", 1},   {"6", 1},  //3
//                {"1", 1},  {"2", 1},   {"3", 1},  //6
//                {"*", 1},  {"0", 1},   {"-", 1},  //9
//                {"Enter", 1.5},    {"Esc", 1.5},  //12
//            };
//            qreal width = size().width();
//            qreal height = size().height();
//            int tmpSize1 = sizeof(KEYINFO);
//            int tmpSize2 = sizeof(tmpKeyLetters);
//            int count = tmpSize2/tmpSize1;
//            int ratioTotalForRow = 3;  //>@1列共有3
//            int row = 5;
//            qreal tmpWidthDiff = width / (qreal)ratioTotalForRow;
//            int h = height / (qreal)row;
//            int x=0, y=0, w=0;
//            for(int i=0; i<count; i++)
//            {
//                if(i == 0)             //>@Row1
//                {
//                    x=0; y=0; w=0;
//                }
//                else if(i == 3)       //>@Row2
//                {
//                    x=0; y=h; w=0;
//                }
//                else if(i == 6)       //>@Row3
//                {
//                    x=0; y=2*h; w=0;
//                }
//                else if(i == 9)       //>@Row4
//                {
//                    x=0; y=3*h; w=0;
//                }
//                else if(i == 12)       //>@Row5
//                {
//                    x=0; y=4*h; w=0;
//                }
//                KeyboardBtnInfo *tmpKeyboardBtnInfo = new KeyboardBtnInfo;
//                w = tmpKeyLetters[i].mLenRatio * tmpWidthDiff;
//                tmpKeyboardBtnInfo->mRect = QRectF(x+mCSpace, y+mRSpace, w-2*mCSpace, h-2*mRSpace);
//                x += w;
//                if(tmpKeyLetters[i].mLabel.compare(QString("LCD")) == 0)
//                {
//                    AddLCD(tmpKeyboardBtnInfo->mRect);
//                    delete tmpKeyboardBtnInfo;
//                }
//                else
//                {
//                    AddButton(tmpKeyLetters[i].mLabel, tmpKeyboardBtnInfo->mButtonItem, tmpKeyboardBtnInfo->mRect);
//                    m_KeyboardBtnInfoList.append(tmpKeyboardBtnInfo);
//                }
//            }
//            break;
//        }
//        case PTN_KB_GRID_V:
//        {
//            KEYINFO tmpKeyLetters[]=
//            {
//                {"7", 1, 1},  {"8", 1, 1},  {"9", 1, 1},  {"Esc", 1, 1},        //0
//                {"4", 1, 1},  {"5", 1, 1},  {"6", 1, 1},  {"Enter", 1, 2},      //4
//                {"1", 1, 1},  {"2", 1, 1},  {"3", 1, 1},                     //8
//                {"*", 1, 1},  {"0", 1, 1},  {"-", 1, 1},  {"A B C D G H", 1, 1},//11
//            };
//            qreal width = size().width();
//            qreal height = size().height();
//            int count = sizeof(tmpKeyLetters)/sizeof(KEYINFO);
//            int ratioTotalForColumn = 4;  //>@1列共有4
//            int column = 4;
//            qreal tmpHeightDiff = height / (qreal)ratioTotalForColumn;
//            qreal tmpWidthDiff = width / (qreal)column;
//            int x=0, y=0, h=0;
//            for(int i=0; i<count; i++)
//            {
//                if(i == 0)                      //>@Row1
//                {
//                    x=0; y=0; h=0;
//                }
//                else if(i == 4)                 //>@Row2
//                {
//                    x=4; y=tmpHeightDiff; h=0;
//                }
//                else if(i == 8)                 //>@Row3
//                {
//                    x=8; y=2*tmpHeightDiff; h=0;
//                }
//                else if(i == 11)                //>@Row4
//                {
//                    x=11; y=3*tmpHeightDiff; h=0;
//                }
//                KeyboardBtnInfo *tmpKeyboardBtnInfo = new KeyboardBtnInfo;
//                h = tmpKeyLetters[i].mHeightRatio * tmpHeightDiff;
//                w = (i-x)*tmpWidthDiff;
//                tmpKeyboardBtnInfo->mRect = QRectF(w, y, w, h);
//                if(tmpKeyLetters[i].mLabel.compare(QString("LCD")) == 0)
//                {
//                    AddLCD(tmpKeyboardBtnInfo->mRect);
//                    delete tmpKeyboardBtnInfo;
//                }
//                else
//                {
//                    AddButton(tmpKeyLetters[i].mLabel, tmpKeyboardBtnInfo->mButtonItem, tmpKeyboardBtnInfo->mRect);
//                    m_KeyboardBtnInfoList.append(tmpKeyboardBtnInfo);
//                }
//            }
//            break;
//        }
        case PTN_KB_DISEN:
        {
            KEYINFO tmpKeyLetters[]=
            {
                {"LCD", 3, 1},
                {"7", 1, 1},  {"8", 1, 1},  {"9", 1, 1}, //1
                {"4", 1, 1},  {"5", 1, 1},  {"6", 1, 1}, //4
                {"1", 1, 1},  {"2", 1, 1},  {"3", 1, 1}, //7
                {"-", 1, 1},  {"0", 1, 1},  {"*", 1, 1}, //10
                {"Esc", 1.5, 1},   {"Enter", 1.5, 1}, //13
            };
            qreal width = size().width();
            qreal height = size().height();
            int tmpSize1 = sizeof(KEYINFO);
            int tmpSize2 = sizeof(tmpKeyLetters);
            int count = tmpSize2/tmpSize1;
            int ratioTotalForRow = 3;  //>@1列共有3
            int row = 6;
            qreal tmpWidthDiff = width / (qreal)ratioTotalForRow;
            qreal tmpHeightDiff = height / (qreal)row;
            int x=0, y=0, w=0, h=0;
            for(int i=0; i<count; i++)
            {
                if(i == 0)             //>@Row1
                {
                    x=0; y=0; w=0;
                }
                else if(i == 1)       //>@Row2
                {
                    x=0; y=tmpHeightDiff; w=0;
                }
                else if(i == 4)       //>@Row3
                {
                    x=0; y=2*tmpHeightDiff; w=0;
                }
                else if(i == 7)       //>@Row4
                {
                    x=0; y=3*tmpHeightDiff; w=0;
                }
                else if(i == 10)       //>@Row5
                {
                    x=0; y=4*tmpHeightDiff; w=0;
                }
                else if(i == 13)       //>@Row6
                {
                    x=0; y=5*tmpHeightDiff; w=0;
                }
                KeyboardBtnInfo *tmpKeyboardBtnInfo = new KeyboardBtnInfo;
                w = tmpKeyLetters[i].mWidthRatio * tmpWidthDiff;
                h = tmpKeyLetters[i].mHeightRatio * tmpHeightDiff;
                tmpKeyboardBtnInfo->mRect = QRectF(x+mCSpace, y+mRSpace, w-2*mCSpace, h-2*mRSpace);
                x += w;
                if(tmpKeyLetters[i].mLabel.compare(QString("LCD")) == 0)
                {
                    AddLCD(tmpKeyboardBtnInfo->mRect);
                    delete tmpKeyboardBtnInfo;
                }
                else
                {
                    AddButton(tmpKeyLetters[i].mLabel, tmpKeyboardBtnInfo->mButtonItem, tmpKeyboardBtnInfo->mRect);
                    m_KeyboardBtnInfoList.append(tmpKeyboardBtnInfo);
                }
            }
            break;
        }
        case PTN_KB_YUNGTAY:
        {
            KEYINFO tmpKeyLetters[]=
            {
                {"LCD", 4, 5},
                {"A", 1, 1},           {"7", 1, 1},  {"8", 1, 1},  {"9", 1, 1}, //1
                {"B", 1, 1},           {"4", 1, 1},  {"5", 1, 1},  {"6", 1, 1}, //5
                {"Esc", 1, 1},         {"1", 1, 1},  {"2", 1, 1},  {"3", 1, 1}, //9
                {"Disablity", 1, 1},   {"-", 1, 1},  {"0", 1, 1},  {"Enter", 1, 1}  //13
            };
            qreal width = size().width();
            qreal height = size().height();
            int tmpSize1 = sizeof(KEYINFO);
            int tmpSize2 = sizeof(tmpKeyLetters);
            int count = tmpSize2/tmpSize1;
            int ratioTotalForRow = 4;  //>@1行共有3
            int row = 9;
            qreal tmpWidthDiff = width / (qreal)ratioTotalForRow;
            qreal tmpHeightDiff = height / (qreal)row;
            int x=0, y=0, w=0, h=0;
            for(int i=0; i<count; i++)
            {
                if(i == 0)             //>@Row1
                {
                    x=0; y=0; w=0;
                }
                else if(i == 1)       //>@Row2
                {
                    x=0; y=5*tmpHeightDiff; w=0;
                }
                else if(i == 5)       //>@Row3
                {
                    x=0; y=6*tmpHeightDiff; w=0;
                }
                else if(i == 9)       //>@Row4
                {
                    x=0; y=7*tmpHeightDiff; w=0;
                }
                else if(i == 13)       //>@Row5
                {
                    x=0; y=8*tmpHeightDiff; w=0;
                }
                KeyboardBtnInfo *tmpKeyboardBtnInfo = new KeyboardBtnInfo;
                w = tmpKeyLetters[i].mWidthRatio * tmpWidthDiff;
                h = tmpKeyLetters[i].mHeightRatio * tmpHeightDiff;
                tmpKeyboardBtnInfo->mRect = QRectF(x+mCSpace, y+mRSpace, w-2*mCSpace, h-2*mRSpace);
                x += w;
                if(tmpKeyLetters[i].mLabel.compare(QString("LCD")) == 0)
                {
                    AddLCD(tmpKeyboardBtnInfo->mRect);
                    delete tmpKeyboardBtnInfo;
                }
                else
                {
                    AddButton(tmpKeyLetters[i].mLabel, tmpKeyboardBtnInfo->mButtonItem, tmpKeyboardBtnInfo->mRect);
                    m_KeyboardBtnInfoList.append(tmpKeyboardBtnInfo);
                }
            }
            break;
        }
        default:
            return false;
    }
    return true;
}

bool GraphicsKeyboard::Start()
{
    if(GraphicsBase::Start() == false)
        return false;
    InitKeyboardBtnInfoList();
    return true;
}

void GraphicsKeyboard::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_CHANGE:
        {
            if(m_LCD)
                m_LCD->updateEffect(OPERATE_CHANGE, pPara);
            break;
        }
        case OPERATE_ADD:
        {
            if(m_LCD)
                m_LCD->updateEffect(OPERATE_ADD, pPara);
            break;
        }
        case OPERATE_DEL:
        {
            if(m_LCD)
                m_LCD->updateEffect(OPERATE_DEL, pPara);
            break;
        }
        default:
            break;
    }
}

void GraphicsKeyboard::collectReport(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_KEYPRESS:
        {
            if(m_LCD)
                m_LCD->updateEffect(OPERATE_KEYPRESS, pPara);
            quint32 tmpAscii = pPara.toUInt();
            if(tmpAscii >= 0x20 && tmpAscii <= 0x7e)
            {
                if(m_Pattern == PTN_KB_YUNGTAY)
                    m_LCDHideTimer.stop();
            }
            //emit reportEffect(OPERATE_CHANGE, QVariant(0));
            break;
        }
        case OPERATE_CHANGE:
        {
            switch(m_Pattern)
            {
                case PTN_KB_YUNGTAY:
                {
                    GraphicsButtonOperate *tmpParent = qgraphicsitem_cast<GraphicsButtonOperate *>(parentItem());
                    if(tmpParent)
                    {
                        quint32 tmpFloor = pPara.toInt();
                        if(tmpFloor ==  INVALIDRC)
                        {
                            if(tmpParent->m_RegionState)
                                tmpParent->updateEffect(OPERATE_CHANGE, QVariant(0));
                            if(m_LCD)
                                m_LCD->updateEffect(OPERATE_SHOW, QVariant(QString(" ")));
                        }
                        else
                        {
                            char tmpChar = 'A' + qrand() % 7;
                            QLatin1Char tmpLatinChar(tmpChar);
                            if(m_LCD)
                                m_LCD->updateEffect(OPERATE_SHOW, QVariant(QString(tmpLatinChar)));
                            tmpParent->updateEffect(OPERATE_CHANGE, QVariant(1));
                            if(m_Pattern == PTN_KB_YUNGTAY)
                                m_LCDHideTimer.start();
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}

void GraphicsKeyboard::HideLCD()
{
    GraphicsButtonOperate *tmpParent = qgraphicsitem_cast<GraphicsButtonOperate *>(parentItem());
    if(tmpParent)
    {
        if(tmpParent->m_RegionState)
            tmpParent->updateEffect(OPERATE_CHANGE, QVariant(0));
        if(m_LCD)
            m_LCD->updateEffect(OPERATE_SHOW, QVariant(QString(" ")));
    }
}

DesignKeyboard::DesignKeyboard(QGraphicsItem *parent, QRectF pRect) :
    GraphicsKeyboard(parent, pRect)
{
}

QSize DesignKeyboard::GetRcSize(QString pRcName)
{
    return size().toSize();
}
