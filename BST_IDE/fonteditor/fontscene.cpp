#include "fontscene.h"

FontScene::FontScene(QObject *parent) : QGraphicsScene(parent)
{
    myOperateCode = CodeSelect;
    myItemType = ITEM_INVALD;
    myTextColor = Qt::black;
    m_LayList.clear();
    m_LastLayNum = 0;
    m_Frame = 0;

    m_SelectItemInfo = 0;

    m_ItemContexMenu = 0;
    m_ItemContexMenu = new QMenu(0);
    CreateMenuAction();
}

FontScene::~FontScene()
{
    if(m_ItemContexMenu)//保证同时只存在一个menu，及时释放内存
    {
        DestoryMenuAction();
        delete m_ItemContexMenu;
        m_ItemContexMenu = 0;
    }
}

void FontScene::CreateMenuAction()
{
    m_ModifyCom = m_ItemContexMenu->addAction(QIcon(":/Menu/rc/Menu/toolView.png"), tr("Modify"));
    m_ItemContexMenu->addSeparator();
    m_DelCom = m_ItemContexMenu->addAction(QIcon(":/Menu/rc/Menu/minus.png"), tr("Delete"));
    m_ItemContexMenu->addSeparator();

    connect(m_ModifyCom, SIGNAL(triggered(bool)), this, SLOT(SLOT_ComModify()));
    connect(m_DelCom, SIGNAL(triggered(bool)), this, SLOT(SLOT_ComDel()));
}

void FontScene::DestoryMenuAction()
{
    disconnect(m_ModifyCom, SIGNAL(triggered(bool)), 0, 0);
    disconnect(m_DelCom, SIGNAL(triggered(bool)), 0, 0);
}

void FontScene::setTextColor(const QColor &color)
{
    myTextColor = color;
    QList<QGraphicsItem *> tmpList = selectedItems();
    if (tmpList.isEmpty() == false && tmpList.first()->type() == ITEM_TEXT)
    {
        TextItem *item = (TextItem *)(tmpList.first());
        if(item)
            item->setDefaultTextColor(myTextColor);
    }
}

void FontScene::setFont(const QFont &font)
{
    myFont = font;
    QList<QGraphicsItem *> tmpList = selectedItems();
    if (tmpList.isEmpty() == false && tmpList.first()->type() == ITEM_TEXT)
    {
        TextItem *item = (TextItem *)(tmpList.first());
        //At this point the selection can change so the first selected item might not be a ImageTextItem
        if (item)
            item->setFont(myFont);
    }
}

void FontScene::editorLostFocus(TextItem *item)
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

void FontScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
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
                    TextItem *item = (TextItem *)(SelectItems.at(i));
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
    if(myOperateCode == CodeInsert)
    {
        if(SelectItems.count())  //>@只允许在空白地方创建
        {
            IDE_TRACE();
            return;
        }
        IDE_TRACE();
        TEXTINFO *tmpTextInfo = new TEXTINFO;
        tmpTextInfo->mItem = new TextItem(0);
        addItem(tmpTextInfo->mItem);
        tmpTextInfo->mItem->setFont(myFont);
        tmpTextInfo->mItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        connect(tmpTextInfo->mItem, SIGNAL(lostFocus(ImageItem*)), this, SLOT(editorLostFocus(ImageItem*)));
        connect(tmpTextInfo->mItem, SIGNAL(selectedChange(QGraphicsItem*)), this, SIGNAL(itemSelected(QGraphicsItem*)));
        tmpTextInfo->mItem->setDefaultTextColor(myTextColor);
        tmpTextInfo->mItem->setPos(mouseEvent->scenePos());
        tmpTextInfo->mName = tr("Text%1").arg(++m_LastLayNum);
        m_LayList.append(tmpTextInfo);
        RegularZValue();
        SetMode(CodeSelect);
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void FontScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void FontScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    SetMode(CodeSelect);
    emit mouseRelease();
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void FontScene::RegularZValue()
{
    for(int i=0;i<m_LayList.count();i++)
    {
        TEXTINFO *tmpInfo = m_LayList.at(i);
        if(tmpInfo->mItem)
        {
            tmpInfo->mItem->setZValue(ITEMZVALUEBASE+i);
        }
    }
}

void FontScene::SLOT_ComModify()
{

}

void FontScene::SLOT_ComDel()
{

}

