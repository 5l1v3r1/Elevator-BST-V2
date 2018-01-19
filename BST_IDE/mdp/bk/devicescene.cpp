#include "devicescene.h"

DeviceScene::DeviceScene(QString pMdpPath, QRectF pRect, QObject *parent):
    QGraphicsScene(parent),
    mItemAnim(0),
    mDeviceParaForm(0)
{
    mItemFont = QFont("Arial",9,QFont::Black,true);
    mItemFont.setPixelSize(9);
    mItemFont.setCapitalization(QFont::SmallCaps);
    mItemFont.setLetterSpacing(QFont::AbsoluteSpacing,1);
    setSceneRect(pRect);
    setItemIndexMethod(QGraphicsScene::NoIndex);
    mItemAnim = new ItemAnim(this);
    connect(mItemAnim, SIGNAL(sDelNodes(QStringList)), this, SIGNAL(sDelNodes(QStringList)));
    mDeviceParaForm = new DeviceParaForm(pMdpPath);
    InitNodeList(mDeviceParaForm->mNodeList);
}

DeviceScene::~DeviceScene()
{
    mDeviceParaForm->deleteLater();
    disconnect(mItemAnim, SIGNAL(sDelNodes(QStringList)), 0, 0);
    mItemAnim->deleteLater();
}

void DeviceScene::InitNodeList(QStringList pNodeList)
{
    QStringList keyList = GlobalPara.m_ConfigInfoGroup.keys();
    for(int i=0;i<keyList.count();i++)
    {
        QString key = keyList.at(i);
        MDPConfigInfo *configInfo = GlobalPara.m_ConfigInfoGroup.value(key);
        DeviceItem* BgItem;
        if((!key.compare("BottomImg", Qt::CaseInsensitive))||
           (!key.compare("BoardImg", Qt::CaseInsensitive))||
           (!key.compare("Cpu", Qt::CaseInsensitive))||
           (!key.compare("Mem", Qt::CaseInsensitive))||
           (!key.compare("Net", Qt::CaseInsensitive)))
        {
            BgItem = new DeviceItem(this);
            BgItem->setImg(QImage(configInfo->mRc));
            BgItem->setGeometry(configInfo->mRectF);
            BgItem->setFlag(QGraphicsWidget::ItemIsMovable, configInfo->mDrag);
            BgItem->setFont(mItemFont);
            BgItem->SetColor(D_ModifyColor);
            BgItem->setData(0, QVariant(configInfo->mName));
            if(configInfo->mDrag)
                BgItem->setData(1, QVariant(QString("DragEnable")));
            else
                BgItem->setData(1, QVariant(QString("DragDisable")));
            BgItem->setZValue(configInfo->mZValue);
            addItem(BgItem);
            if((!key.compare("Cpu", Qt::CaseInsensitive))||
               (!key.compare("Mem", Qt::CaseInsensitive))||
               (!key.compare("Net", Qt::CaseInsensitive)))
                mPHNodeMap.insert(configInfo->mName, BgItem);
        }
        for(int j=0;j<pNodeList.count();j++)
        {
            if((!key.compare("Cpu", Qt::CaseInsensitive))||
               (!key.compare("Mem", Qt::CaseInsensitive))||
               (!key.compare("Net", Qt::CaseInsensitive)))
                continue;
            QString itemName = pNodeList.at(j);
            if(itemName.compare(configInfo->mName, Qt::CaseInsensitive))continue;
            DeviceItem* item;
            item = new DeviceItem(this);
            item->setImg(QImage(configInfo->mRc));
            item->setGeometry(configInfo->mRectF);
            item->setFlag(QGraphicsWidget::ItemIsMovable, configInfo->mDrag);
            item->setFont(mItemFont);
            item->SetColor(D_ModifyColor);
            item->setData(0, QVariant(configInfo->mName));
            if(configInfo->mDrag)
                item->setData(1, QVariant(QString("DragEnable")));
            else
                item->setData(1, QVariant(QString("DragDisable")));
            item->setZValue(configInfo->mZValue);
            SetShadow(item, D_ShadowColor, 0.3, QPointF(10,10));
            addItem(item);
            mPHNodeMap.insert(configInfo->mName, item);
        }
        mDeviceParaForm->slot_AddNode(QString("Cpu"));
        mDeviceParaForm->slot_AddNode(QString("Mem"));
        mDeviceParaForm->slot_AddNode(QString("Net"));
    }
    mItemAnim->AddNode();
}

void DeviceScene::OpenNodeParaForm(QGraphicsItem* pItem)
{
    if(!pItem)return;
    QString itemName = pItem->data(0).toString();
    PH_Type PHType = GetPHType(itemName);
    if(PHType == P_INVALID)return;
    mItemAnim->StopTurn();
    mDeviceParaForm->SetCurPage(PHType);
    mDeviceParaForm->show();
    int ret = mDeviceParaForm->exec();
    if(ret == QDialog::Accepted)
    {
        mDeviceParaForm->Save();
        DeviceItem* devItem = static_cast<DeviceItem*>(pItem);
        if(devItem)devItem->SetColor(D_ModifyColor);
        mItemAnim->TurnNode(mItemAnim->mTurnIndex+2);
    }
    else
    {
        mDeviceParaForm->Cancer();
        mItemAnim->TurnNode(mItemAnim->mTurnIndex-2);
    }
    ClearColor(pItem);
    SetShadow(pItem, D_ShadowColor, 0.3, QPointF(10,10));
}

bool DeviceScene::FindSelectItem(QGraphicsItem*& pItem)
{
    QStringList keyList = mPHNodeMap.keys();
    for(int i=0;i<keyList.count();i++)
    {
        QString key = keyList.at(i);
        pItem = mPHNodeMap.value(key);
        if(!pItem)continue;
        QGraphicsColorizeEffect* effect = VAR_CAST<QGraphicsColorizeEffect*>(pItem->graphicsEffect());
        if((effect)&&(effect->color() == D_SelectColor))
        return true;
    }
    return false;
}

void DeviceScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
//    IDE_DEBUG("dragMoveEvent");
    event->accept();
}

void DeviceScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
//    IDE_DEBUG("dropEvent");
    if(event->mimeData()->hasFormat("PHNode"))
    {
        QByteArray itemData = event->mimeData()->data("PHNode");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        QString itemName;
        dataStream>>itemName;
        MDPConfigInfo *configInfo;
        QStringList keyList = GlobalPara.m_ConfigInfoGroup.keys();
        for(int i=0;i<keyList.count();i++)
        {
            QString key = keyList.at(i);
            configInfo = GlobalPara.m_ConfigInfoGroup.value(key);
            if(!key.compare(itemName, Qt::CaseInsensitive))
                goto gt_FindItem;
        }
        event->ignore();
        return;
gt_FindItem:
        DeviceItem* item = new DeviceItem(this);
        item->setImg(QImage(configInfo->mRc));
        item->setFont(mItemFont);
        item->SetColor(D_InitColor);
        item->setFlag(QGraphicsWidget::ItemIsMovable, configInfo->mDrag);
        item->setData(0, QVariant(configInfo->mName));
        if(configInfo->mDrag)
            item->setData(1, QVariant(QString("DragEnable")));
        else
            item->setData(1, QVariant(QString("DragDisable")));
        item->setGeometry(event->scenePos().rx(), event->scenePos().ry(), 0, 0);
        item->setZValue(configInfo->mZValue);
        addItem(item);
        SetShadow(item, D_ShadowColor, 0.3, QPointF(10,10));
        mPHNodeMap.insert(itemName, item);
        mItemAnim->AddNode();
        event->accept();
    }
    else
        event->ignore();
}

void DeviceScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    IDE_DEBUG("mousePressEvent");
    IDE_TRACE_INT(event->scenePos().rx());
    IDE_TRACE_INT(event->scenePos().ry());
    QGraphicsScene::mousePressEvent(event);
    qreal rx = event->scenePos().rx();
    qreal ry = event->scenePos().ry();
    QGraphicsItem* item = itemAt(rx, ry);
    if(item)
    {
        QString itemName = item->data(0).toString();
        PH_Type PHType = GetPHType(itemName);
        if(PHType != P_INVALID)
        {
            QStringList keyList = mPHNodeMap.keys();
            for(int i=0;i<keyList.count();i++)
            {
                QString key = keyList.at(i);
                DeviceItem* item = mPHNodeMap.value(key);
                if(!item)continue;
                ClearColor(item);
                SetShadow(item, D_ShadowColor, 0.3, QPointF(10,10));
            }
            SetColor(item, D_SelectColor);
            return;
        }
    }
    if(mItemAnim->AddAnimIsRunning()||mItemAnim->DelAnimIsRunning())
        return;
    qreal r = sqrt(rx*rx + ry*ry);
    if(r > D_Radius+50)
    {
        mItemAnim->StopTurn();
    }
    else if(r < D_Radius-50)
    {
        mItemAnim->mAutoTurn = true;
        mItemAnim->TurnNode(mItemAnim->mTurnIndex+1);
    }
}

void DeviceScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseDoubleClickEvent(event);
    if(mItemAnim->AddAnimIsRunning()||mItemAnim->DelAnimIsRunning())
        return;
    OpenNodeParaForm(itemAt(event->scenePos().rx(), event->scenePos().ry()));
}

void DeviceScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if(mItemAnim->AddAnimIsRunning())
        return;
    qreal rx = event->scenePos().rx();
    qreal ry = event->scenePos().ry();
    QGraphicsItem* item = itemAt(rx, ry);
    if(!item)return;
    QString itemName = item->data(0).toString();
    PH_Type PHType = GetPHType(itemName);
    if((PHType == P_INVALID)||(PHType == P_CPU)||(PHType == P_MEM)||(PHType == P_NET))return;
    qreal r = sqrt(rx*rx + ry*ry);
    if(r > D_Radius+30)
    {
        mItemAnim->DelNode(itemName);
    }
    else if(r < D_Radius-30)
    {
        mItemAnim->AddNode();
    }
}

void DeviceScene::keyPressEvent(QKeyEvent *event)
{
    QGraphicsScene::keyPressEvent(event);
    switch(event->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        if(mItemAnim->AddAnimIsRunning()||mItemAnim->DelAnimIsRunning())
            return;
        QGraphicsItem* item;
        if(FindSelectItem(item))OpenNodeParaForm(item);
        break;
    }
    case Qt::Key_Delete:
    {
        if(mItemAnim->AddAnimIsRunning())
            return;
        QGraphicsItem* item;
        if(FindSelectItem(item))mItemAnim->DelNode(item->data(0).toString());
        break;
    }
    case Qt::Key_Left:
        if(mItemAnim->AddAnimIsRunning()||mItemAnim->DelAnimIsRunning())
            return;
        mItemAnim->mAutoTurn = false;
        if(mItemAnim->TurnAnimIsRunning())
            return;
        mItemAnim->TurnNode(mItemAnim->mTurnIndex+1);
        break;
    case Qt::Key_Right:
        if(mItemAnim->AddAnimIsRunning()||mItemAnim->DelAnimIsRunning())
            return;
        mItemAnim->mAutoTurn = false;
        if(mItemAnim->TurnAnimIsRunning())
            return;
        mItemAnim->TurnNode(mItemAnim->mTurnIndex-1);
        break;
    }
}
/**
//>@***********************************************************
*/



