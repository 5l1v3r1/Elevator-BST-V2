#include "imagescene.h"
#include "mainwindow.h"

ImageScene::ImageScene(QObject *parent) : QGraphicsScene(parent)
{
    myOperateCode = CodeSelect;
    myItemType = ITEM_INVALD;
    textItem = 0;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
    m_LayList.clear();
    m_LastLayNum = 0;
    m_Frame = 0;

    GlobalPara.m_ActiveImageItemInfo = m_SelectItemInfo = 0;

    m_ItemContexMenu = 0;
    m_ItemContexMenu = new QMenu(0);
    CreateMenuAction();
}

ImageScene::~ImageScene()
{
    if(m_ItemContexMenu)//保证同时只存在一个menu，及时释放内存
    {
        DestoryMenuAction();
        delete m_ItemContexMenu;
        m_ItemContexMenu = 0;
    }
}

void ImageScene::UpdateScene(QPixmap pPic)
{
    DelBackground();
    AddBackground(pPic);
}

bool ImageScene::AddBackground(QPixmap pPic)
{
    if(m_LayList.isEmpty() == false)
    {
        IDE_TRACE();
        return false;
    }
    if(pPic.isNull())
    {
        IDE_TRACE();
        return false;
    }
    setSceneRect(QRectF(pPic.rect()));
    IMAGEITEMINFO *tmp = new IMAGEITEMINFO(tr("Background"), ITEM_BASE, QRectF(pPic.rect()));
    ImageItem *tmpItem = new ImageItem(ITEM_BASE, 0);
    addItem(tmpItem);
    tmp->mItem = (QGraphicsItem*)tmpItem;
    tmpItem->setGeometry(tmp->mRect);
    IDE_TRACE_INT(tmp->mRect.width());
    IDE_TRACE_INT(tmp->mRect.height());
    tmp->SetPixmap(pPic);
    m_LayList.append(tmp);
    RegularZValue();
    return true;
}

bool ImageScene::DelBackground()
{
    bool flag = false;
    int count = m_LayList.count();
    if(count<=0)
        return true;
    //>@首先删除子节点
    for(int i=count-1;i>0;i--)
    {
        IMAGEITEMINFO *tmp = m_LayList.at(i);
        if(tmp)
            ItemOperate(tmp, CodeDelete);
    }
    IMAGEITEMINFO *tmp = m_LayList.at(0);
    if(tmp && tmp->type() == ITEM_BASE)
    {
        ImageItem *tmpItem = /*qgraphicsitem_cast*/(ImageItem *)(tmp->mItem);
        if(tmpItem)
        {
            removeItem(tmpItem);
            delete tmpItem;
            m_LayList.removeOne(tmp);
            flag = true;
        }
    }
    RegularZValue();
    return flag;
}

void ImageScene::CreateMenuAction()
{
    m_ModifyCom = m_ItemContexMenu->addAction(QIcon(":/Menu/rc/Menu/toolView.png"), tr("Modify"));
    m_ItemContexMenu->addSeparator();
    m_DelCom = m_ItemContexMenu->addAction(QIcon(":/Menu/rc/Menu/minus.png"), tr("Delete"));
    m_ItemContexMenu->addSeparator();

    MainWindow* tmpMain = VAR_CAST<MainWindow*>(GlobalPara.m_MainWindow);
    //connect(m_ModifyCom, SIGNAL(triggered()), tmpMain, SLOT(EffecfSettingDialog()));
    connect(m_DelCom, SIGNAL(triggered()), this, SLOT(SLOT_ComDel()));
}

void ImageScene::DestoryMenuAction()
{
    disconnect(m_ModifyCom, SIGNAL(triggered(bool)), 0, 0);
    disconnect(m_DelCom, SIGNAL(triggered(bool)), 0, 0);
}

void ImageScene::setTextColor(const QColor &color)
{
    myTextColor = color;
    QList<QGraphicsItem *> tmpList = selectedItems();
    if (tmpList.isEmpty() == false && tmpList.first()->type() == ITEM_TEXT)
    {
        TextItem *item = /*qgraphicsitem_cast<*/(TextItem *)(tmpList.first());
        if(item)
            item->setDefaultTextColor(myTextColor);
    }
}

void ImageScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    QList<QGraphicsItem *> tmpList = selectedItems();
    if (tmpList.isEmpty() == false && (tmpList.first()->type() == ITEM_BASE || tmpList.first()->type() == ITEM_LAY))
    {
        ImageItem *item = /*qgraphicsitem_cast*/(ImageItem *)(tmpList.first());
        if (item)
        {
            QGraphicsColorizeEffect *uEffect = new QGraphicsColorizeEffect();
            uEffect->setColor(color);
            item->setGraphicsEffect(uEffect);
        }
    }
}

void ImageScene::setFont(const QFont &font)
{
    myFont = font;
    QList<QGraphicsItem *> tmpList = selectedItems();
    if (tmpList.isEmpty() == false && tmpList.first()->type() == ITEM_TEXT)
    {
        TextItem *item = /*qgraphicsitem_cast*/(TextItem *)(tmpList.first());
        //At this point the selection can change so the first selected item might not be a ImageTextItem
        if (item)
            item->setFont(myFont);
    }
}

void ImageScene::editorLostFocus(TextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty())
    {
        removeItem(item);
        item->deleteLater();
    }
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::RightButton)
    {
        if(m_ItemContexMenu)
        {
            QList<QGraphicsItem *> SelectItems = items(mouseEvent->scenePos());
            if(SelectItems.count()>0)
            {
                bool flag = false;
                for(int i=0;i<SelectItems.count();i++)
                {
                    ImageItem *item = (ImageItem *)(SelectItems.at(i));
                    if (item)
                    {
                        flag = true;
                        break;
                    }
                }
                if(flag == false)
                {
                    IDE_TRACE();
                    return;
                }
                m_ItemContexMenu->exec(QCursor::pos());//在当前鼠标位置显示
            }
        }
        return;
    }
    else if(mouseEvent->button() != Qt::LeftButton)
    {
        IDE_TRACE();
        return;
    }

    QList<QGraphicsItem *> SelectItems = items(mouseEvent->scenePos());
    if(SelectItems.count()>0)
    {
        if(myOperateCode == CodeInsert)
        {
            QGraphicsItem *tmpParentItem;
            bool flag = false;
            for(int i=0;i<SelectItems.count();i++)
            {
                tmpParentItem = SelectItems.at(i);
                if(tmpParentItem->type() != ITEM_BASE) //>@只允许在BASE上创建
                    continue;
                flag = true;
                break;
            }
            if(flag == false)
            {
                IDE_TRACE();
                return;
            }
            IMAGEITEMINFO *tmpImageInfo = new IMAGEITEMINFO;
            tmpImageInfo->mParItem = tmpParentItem;
            tmpImageInfo->mType = myItemType;
            tmpImageInfo->mRect = QRectF(mouseEvent->scenePos(), QSizeF(100,100));
            ItemOperate(tmpImageInfo, CodeInsert);
            SetMode(CodeSelect);
        }
        else
        {
            QGraphicsItem *tmp = SelectItems.first();
            int type = tmp->type();
            if(type == ITEM_LAY)  //>@是否为QGraphicsWidget的type
            {
                bool flag = false;
                IMAGEITEMINFO* tmpImageInfo = 0;
                for(int i=0; i<m_LayList.count(); i++)
                {
                    tmpImageInfo = m_LayList.at(i);
                    if(tmpImageInfo)
                    {
                        QGraphicsItem *tmpItem = tmpImageInfo->mItem;
                        if(tmpItem == tmp)
                        {
                            flag = true;
                            break;
                        }
                    }
                }
                if(flag)
                {
                    IDE_TRACE_INT(tmpImageInfo->type());
                    ItemOperate(tmpImageInfo, CodeSelect);
                }
            }
        }
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void ImageScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void ImageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    SetMode(CodeSelect);
    emit mouseRelease();
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void ImageScene::RegularZValue()
{
    for(int i=0;i<m_LayList.count();i++)
    {
        IMAGEITEMINFO *tmpInfo = m_LayList.at(i);
        if(tmpInfo->mItem)
        {
            tmpInfo->mItem->setZValue(ITEMZVALUEBASE+i);
        }
    }
}

bool ImageScene::ItemOperate(IMAGEITEMINFO *&ItemInfo, ITEM_OPERATECODE Code)
{
    switch(ItemInfo->mType)
    {
        case ITEM_LAY:
        {
            switch(Code)
            {
                case CodeInsert:
                {
                    ImageEffectForm tmpEffectForm(ItemInfo);
                    int ret = tmpEffectForm.exec();
                    if(ret == QDialog::Rejected)
                    {
                        return false;
                    }
                    ImageItem *tmpItem = new ImageItem(tmpEffectForm.m_EffectPic, ItemInfo->mType, ItemInfo->mParItem);
                    ItemInfo->mItem = (QGraphicsItem  *)tmpItem;
                    tmpItem->setGeometry(ItemInfo->mRect);
                    ItemInfo->mName = tr("Lay%1").arg(++m_LastLayNum);
                    m_LayList.append(ItemInfo);
                    RegularZValue();
                    break;
                }
                case CodeDelete:
                {
                    ImageItem *tmpItem =  /*qgraphicsitem_cast*/(ImageItem *)(ItemInfo->mItem);
                    if(tmpItem)
                    {
                        removeItem(tmpItem);
                        delete tmpItem;
                        m_LayList.removeOne(ItemInfo);
                        RegularZValue();
                    }
                    break;
                }
                case CodeMove:
                {
                    ImageItem *tmpItem =  /*qgraphicsitem_cast*/(ImageItem *)(ItemInfo->mItem);
                    if(tmpItem)
                    {
                        tmpItem->setGeometry(ItemInfo->mRect);
                        updateFrame(ItemInfo);
                    }
                    break;
                }
                case CodeSelect:
                {
                    ImageItem *tmpItem = /*qgraphicsitem_cast*/(ImageItem *)(ItemInfo->mItem);
                    if(tmpItem)
                    {
                        createFrame();
                        connect(tmpItem, SIGNAL(ReleaseSingnal(QPointF)), m_Frame, SLOT(DragComPos(QPointF)));
                        if(m_SelectItemInfo != ItemInfo)
                        {
                            //>@保存当前选中的控件
                            GlobalPara.m_ActiveImageItemInfo = m_SelectItemInfo = ItemInfo;
                            updateFrame(ItemInfo);
                            m_Frame->show();
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case ITEM_TEXT:
        {
            switch(Code)
            {
                case CodeInsert:
                {
                    IDE_TRACE_INT(ItemInfo->mParItem);
                    TextItem *tmpItem = new TextItem(ItemInfo->mParItem);
                    ItemInfo->mItem = (QGraphicsItem *)tmpItem;
                    tmpItem->setFont(myFont);
                    tmpItem->setTextInteractionFlags(Qt::TextEditorInteraction);
                    connect(tmpItem, SIGNAL(lostFocus(ImageItem*)), this, SLOT(editorLostFocus(ImageItem*)));
                    connect(tmpItem, SIGNAL(selectedChange(QGraphicsItem*)), this, SIGNAL(itemSelected(QGraphicsItem*)));
                    tmpItem->setDefaultTextColor(myTextColor);
                    tmpItem->setPos(ItemInfo->mRect.topLeft());
                    ItemInfo->mName = tr("Text%1").arg(++m_LastLayNum);
                    m_LayList.append(ItemInfo);
                    RegularZValue();
                    break;
                }
                case CodeDelete:
                {
                    TextItem *tmpItem =  /*qgraphicsitem_cast*/(TextItem *)(ItemInfo->mItem);
                    if(tmpItem)
                    {
                        removeItem(tmpItem);
                        delete tmpItem;
                        m_LayList.removeOne(ItemInfo);
                        RegularZValue();
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
    return true;
}

void ImageScene::deleteFrame()
{
    if(m_Frame == 0)
        return;
    disconnect(m_Frame, SIGNAL(updateControl(DiagramFrame *)), 0, 0);
    removeItem(m_Frame);
    m_Frame->deleteLater();
    m_Frame = 0;
}

void ImageScene::createFrame()
{
    if(m_Frame == 0)
    {
        m_Frame = new ImageFrame(0, this);
        addItem(m_Frame);
        connect(m_Frame, SIGNAL(updateControl(ImageFrame *)), this, SLOT(updateControl(ImageFrame *)));
    }
}

void ImageScene::updateFrame(IMAGEITEMINFO* ComInfo)
{
    if(ComInfo == 0)
    {
        IDE_TRACE();
        return;
    }
    if(ComInfo->mItem == 0)
    {
        IDE_TRACE();
        return;
    }
    createFrame();
    m_Frame->setParentItem(ComInfo->mItem);
    m_Frame->InitFrame(ComInfo);
}

void ImageScene::updateControl(ImageFrame *pFrame)
{
    if(pFrame == 0)
    {
        IDE_TRACE();
        return;
    }
    if(m_SelectItemInfo == 0 || m_SelectItemInfo->mType == ITEM_BASE)
    {
        IDE_TRACE();
        return;
    }
    if(pFrame->m_DragPosChangeFlag)
    {
        pFrame->m_DragPosChangeFlag = false;
        IDE_TRACE();
        return;
    }
    if(pFrame->m_FramePolygon.count() != 4)
    {
        IDE_TRACE_INT(pFrame->m_FramePolygon.count());
        return;
    }
    QPointF tmpPoint1 = pFrame->m_FramePolygon.at(0);
    QPointF tmpPoint2 = pFrame->m_FramePolygon.at(2);
    QRectF rect = QRectF(tmpPoint1,tmpPoint2).normalized();
    m_SelectItemInfo->mRect = QRectF(pFrame->m_ChildParentDiff + rect.topLeft(), QSizeF(rect.right() - rect.left(), rect.bottom() - rect.top()));
    ItemOperate(m_SelectItemInfo, CodeMove);
    IDE_TRACE();
}
