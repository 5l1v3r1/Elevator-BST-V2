#include "propertytree.h"
#include "graphicsinc.h"
#include "diagramscene.h"

TreeComboBox::TreeComboBox(QTreeWidgetItem* pItem, QStringList pEnumValue, QString pValue, QWidget *parent):
    QComboBox(parent)
{
    mTreeItem = pItem;
    mValue = pValue;
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setMaximumHeight(25);
    addItems(pEnumValue);
    for(int i=0;i<pEnumValue.count();i++)
    {
        if(pValue.compare(pEnumValue.at(i)) == 0)
            setCurrentIndex(i);
    }
    connect(this, SIGNAL(currentIndexChanged(QString)), this, SLOT(slot_ValueChanged(QString)));
}

void TreeComboBox::slot_ValueChanged(QString pValue)
{
    emit sValueChanged(pValue);
}

TreeSpinBox::TreeSpinBox(QTreeWidgetItem* pItem, QString pValue, QWidget *parent):
    QSpinBox(parent)
{
    mTreeItem = pItem;
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setMaximumHeight(25);
    setRange(0, 65535);
    mValue = pValue.toInt();
    setValue(mValue);
    connect(this, SIGNAL(editingFinished()), this, SLOT(slot_ValueChanged()));
}

void TreeSpinBox::slot_ValueChanged()
{
    emit sValueChanged(QString("%1").arg(value()));
}

TreeButtons::TreeButtons(QTreeWidgetItem* pItem, QString pFilePath, QStringList pFileType, QWidget *parent):
    QWidget(parent)
{
    mTreeItem = pItem;
    mFileFormat = QString("Rc (");
    for(int i=0;i<pFileType.count();i++)
    {
        QString format = pFileType.at(i);
        mFileFormat.append(QString("*.%1 ").arg(format.toLower()));
    }
    mFileFormat.remove(mFileFormat.length()-1, 1);
    mFileFormat.append(QString(")"));
    mFilePath = pFilePath;

    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setFixedHeight(20);

    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->setSpacing(0);
    hLayout->setMargin(0);
    QSpacerItem* spacerItem = new QSpacerItem(10, 200, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hLayout->addSpacerItem(spacerItem);

    QPushButton* modifyBtn = new QPushButton(QIcon(":/Menu/rc/Menu/open.png"), QString(""), 0);
    modifyBtn->setFixedSize(20, 20);
    hLayout->addWidget(modifyBtn, 0);
    connect(modifyBtn, SIGNAL(clicked()), this, SLOT(slot_ExplorerFile()));

    QPushButton* clearBtn = new QPushButton(QIcon(":/Menu/rc/Menu/undo.png"), QString(""), 0);
    clearBtn->setFixedSize(20, 20);
    hLayout->addWidget(clearBtn, 1);
    connect(clearBtn, SIGNAL(clicked()), this, SLOT(slot_ClearFile()));
}

TreeButtons::~TreeButtons()
{
}

void TreeButtons::focusOutEvent(QFocusEvent *event)
{
    emit sValueChanged(mFilePath);
}

void TreeButtons::slot_ExplorerFile()
{
    static QString sSelectPath = QDir::currentPath();
    QString rcPath = QFileDialog::getOpenFileName(this, tr("Select File"),
                                                  sSelectPath,
                                                  mFileFormat);
    if(!rcPath.isEmpty())
    {
        sSelectPath = getParentDirectory(rcPath);
        emit sValueChanged(rcPath);
    }
}

void TreeButtons::slot_ClearFile()
{
    emit sValueChanged(QString());
}

PropertyTree::PropertyTree(QWidget *parent) :
    UpQTreeWidget(parent)
{
    mRcMenu = 0;
    mCurTreeItem = 0;
    mComponet = 0;

    setRootIsDecorated(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setStyleSheet("QTreeWidget::item{height:20px}");  //设置行宽
    setColumnCount(2);
    setColumnWidth(0, 120);
    setHeaderLabel(tr("Property List"));
    setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);
    setSelectionMode(QAbstractItemView::SingleSelection);
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
            this, SLOT(slot_ItemDoubleClicked(QTreeWidgetItem *, int)));
    connect(this, SIGNAL(itemPressed(QTreeWidgetItem *, int)),
            this, SLOT(slot_ItemPressed(QTreeWidgetItem *, int)));
}

PropertyTree::~PropertyTree()
{
    disconnect(this, SIGNAL(sUpdate(COM_INFO*)), 0, 0);
    ReleaseMenu();
}

void PropertyTree::CreateMenu()
{
    /*
        New：根据RC的排序在最近的位置添加一个新项
        Add：在RC列表的最后添加多个新项
        ---------
        Del：删除当前RC项，对于系统默认的RC项只能删除文件，不删除整个项
        Clear：清空所有资源，于系统默认的RC项只清空文件内容部分
    */
    mRcMenu = new QMenu;
    mRcMenu->addAction(QIcon(":/Menu/rc/Menu/new.png"), tr("&New"),
                               this, SLOT(slot_NewRC()));
    mRcMenu->addAction(QIcon(":/Menu/rc/Menu/add.png"), tr("&Add"),
                               this, SLOT(slot_AddRCs()));
    mRcMenu->addSeparator();
    mRcMenu->addAction(QIcon(":/Menu/rc/Menu/delete.png"), tr("&Del"),
                               this, SLOT(slot_DelRC()));
    mRcMenu->addAction(QIcon(":/Menu/rc/Menu/clear.png"), tr("&Clear"),
                               this, SLOT(slot_ClearRCs()));
}

void PropertyTree::ReleaseMenu()
{
    if(mRcMenu)
        mRcMenu->deleteLater();
}

void PropertyTree::Connect()
{
    if(!mComponet)
        return;
    connect(mComponet, SIGNAL(sSceneRectChanged(QRectF)), this, SLOT(slot_SceneRectChanged(QRectF)));
}

void PropertyTree::Disconnect()
{
    if(!mComponet)
        return;
    disconnect(mComponet, SIGNAL(sRectChanged(QRect)), 0, 0);
}

bool PropertyTree::RefreshTree(GraphicsOperate *pComponent)
{
    //>@信号连接
    Disconnect();
    mComponet = pComponent;
    if(!mComponet)
        return false;
    //>@清空已有的项目
    mCurTreeItem = 0;
    ClearAllItem();
    //>@建立信号连接
    Connect();
    //>@添加控件名
    QTreeWidgetItem *tmpComTreeItem = new QTreeWidgetItem((QTreeWidget*)0,
                                                          QStringList()<<tr("Component")<<mComponet->m_ComPath,
                                                          COMTREEITEM_TYPE);
    tmpComTreeItem->setIcon(1, QIcon(mComponet->m_ComCategory->Icon));
    tmpComTreeItem->setBackgroundColor(0, QColor(254,235,208));
    tmpComTreeItem->setBackgroundColor(1, QColor(254,235,208));
    addTopLevelItem(tmpComTreeItem);
    //>@添加坐标
    QTreeWidgetItem *tmpRectTreeItem = new QTreeWidgetItem((QTreeWidget*)0,
                                                           QStringList()<<tr("Geometry")<<((mComponet->m_UiScene->m_UiDirection)?tr("Horizontal"):tr("Vertical")),
                                                           RECTTREEITEM_TYPE);
    tmpRectTreeItem->setBackgroundColor(0, QColor(255,250,179));
    tmpRectTreeItem->setBackgroundColor(1, QColor(255,250,179));
    addTopLevelItem(tmpRectTreeItem);
    ChangeRect(tmpRectTreeItem, mComponet->geometry().toRect());
    //>@添加状态机
    DiagramScene *tmpScene = (DiagramScene *)(mComponet->m_UiScene);
    for(int i=0;i<mComponet->m_ComCategory->StateMachines.count();i++)
    {
        STATEMACHINE *tmpSM = mComponet->m_ComCategory->StateMachines.at(i);
        if(!tmpSM)
            continue;
        QTreeWidgetItem *tmpSMTreeItem = new QTreeWidgetItem((QTreeWidget*)0,
                                                             QStringList()<<tmpSM->StateName,
                                                             STATETREEITEM_TYPE);
        tmpSMTreeItem->setBackgroundColor(0, (i%2)?QColor(255,250,179):QColor(254,235,208));
        tmpSMTreeItem->setBackgroundColor(1, (i%2)?QColor(255,250,179):QColor(254,235,208));
        addTopLevelItem(tmpSMTreeItem);
        //>@初始化状态机
        QString tmpEffectName;
        QDomElement tmpStateElement = tmpScene->m_UiManager->GetComState(mComponet->m_ComElement, tmpSM->StateName);
        if(!tmpStateElement.isNull())
            tmpEffectName = tmpStateElement.attribute(D_EFFECTTYPE);
        ChangeEffect(tmpSMTreeItem, tmpEffectName);
    }
    expandAll();
}

QRect PropertyTree::GetCurRect(QTreeWidgetItem *pGeometryTree)
{
    QRect tmpRect;
    if(!pGeometryTree)
        return tmpRect;
    for(int i=0;i<pGeometryTree->childCount();i++)
    {
        QTreeWidgetItem *tmpSubItem = pGeometryTree->child(i);
        if(!tmpSubItem)
            continue;
        if(tmpSubItem->type() == X_TREEITEM_TYPE)
            tmpRect.setX(tmpSubItem->text(1).toInt());
        else if(tmpSubItem->type() == Y_TREEITEM_TYPE)
            tmpRect.setY(tmpSubItem->text(1).toInt());
        else if(tmpSubItem->type() == W_TREEITEM_TYPE)
            tmpRect.setWidth(tmpSubItem->text(1).toInt());
        else if(tmpSubItem->type() == H_TREEITEM_TYPE)
            tmpRect.setHeight(tmpSubItem->text(1).toInt());
    }
    return tmpRect;
}

bool PropertyTree::ChangeRect(QTreeWidgetItem *pGeometryTree, QRect pRect)
{
    if(mCurTreeItem)
    {
        if(itemWidget(mCurTreeItem, 1))
            removeItemWidget(mCurTreeItem, 1);
        mCurTreeItem = 0;
    }
    if(!pGeometryTree)
        return false;
    //>@查看当前值是否和待设置的值相同
    QRect tmpCurRect = GetCurRect(pGeometryTree);
    if(tmpCurRect == pRect)
        return true;
    ClearTreeItem(pGeometryTree);
    QTreeWidgetItem *tmpSubRectTreeItem = new QTreeWidgetItem(pGeometryTree,
                                                              QStringList()<<tr("X")<<QString("%1").arg(pRect.x()),
                                                              X_TREEITEM_TYPE);
    tmpSubRectTreeItem = new QTreeWidgetItem(pGeometryTree,
                                             QStringList()<<tr("Y")<<QString("%1").arg(pRect.y()),
                                             Y_TREEITEM_TYPE);
    tmpSubRectTreeItem = new QTreeWidgetItem(pGeometryTree,
                                             QStringList()<<tr("W")<<QString("%1").arg(pRect.width()),
                                             W_TREEITEM_TYPE);
    tmpSubRectTreeItem = new QTreeWidgetItem(pGeometryTree,
                                             QStringList()<<tr("H")<<QString("%1").arg(pRect.height()),
                                             H_TREEITEM_TYPE);
    return true;
}

QTreeWidgetItem* PropertyTree::AddEffectSubParaItem(QTreeWidgetItem* pParent, QString pParaName, QString pInitValue, QString pUiValue)
{
    if(!pParent)
        return (QTreeWidgetItem *)0;
    QTreeWidgetItem *tmpSubParaItem = new QTreeWidgetItem(pParent, SUBPARATREEITEM_TYPE);
    pParent->addChild(tmpSubParaItem);
    tmpSubParaItem->setText(0, pParaName);
    //>@Para只有两种类型：一种以/进行分割(1)，一种为数字(2)
    if(pInitValue.contains('/'))
    {
        tmpSubParaItem->setData(1, EDIT_TYPE, 1);
        QStringList tmpList = pInitValue.split('/');
        tmpSubParaItem->setData(1, EDIT_DATA, tmpList);
        if(tmpList.contains(pUiValue))  //>@验证UI中的值是否有效，否则使用默认值
            tmpSubParaItem->setText(1, pUiValue);
        else
            tmpSubParaItem->setText(1, tmpList.at(0));
    }
    else
    {
        tmpSubParaItem->setData(1, EDIT_TYPE, 2);
        tmpSubParaItem->setData(1, EDIT_DATA, pInitValue);
        if(pUiValue.isEmpty())
            tmpSubParaItem->setText(1, pInitValue);
        else
            tmpSubParaItem->setText(1, pUiValue);
    }
    return tmpSubParaItem;
}

void PropertyTree::InitEffectPara(QTreeWidgetItem *pItem, EFFECT *pDefaultEffect, QDomElement pStateElement)
{
    QString pEffectPara;
    if(!pStateElement.isNull())
        pEffectPara = pStateElement.attribute(D_EFFECTPARA);
    QStringList tmpParaList = pEffectPara.split(';');
    QHash<QString, QString> tmpParas;
    //>@制作hash
    for(int j=0;j<tmpParaList.count();j++)
    {
        QStringList tmpSubParas = tmpParaList.at(j).split(':');
        if(tmpSubParas.count()!=2)
            continue;
        tmpParas.insert(tmpSubParas.at(0), tmpSubParas.at(1));
    }
    for(int i=0;i<pDefaultEffect->EffectParas.count();i++)
    {
        EFFECTPARA *tmpDefaultPara = pDefaultEffect->EffectParas.at(i);
        if(!tmpDefaultPara)
            continue;
        //>@创建Item
        //>@在UI中查找是否有此参数项
        QString tmpUiPara = tmpParas.take(tmpDefaultPara->ParaName);
        AddEffectSubParaItem(pItem, tmpDefaultPara->ParaName, tmpDefaultPara->ParaValue, tmpUiPara);
    }
    //>@针对于一些默认项中没有的特效，也列出来
    QHash<QString, QString>::const_iterator i = tmpParas.constBegin();
    while (i != tmpParas.constEnd())
    {
        AddEffectSubParaItem(pItem, i.key(), "0", i.value());
        ++i;
    }
}

QTreeWidgetItem* PropertyTree::AddEffectSubRcItem(QTreeWidgetItem* pParent, QStringList pRcNames,
                                                  QStringList pRcTypes, int pRcLabel, QString pRcFile)
{
    if(!pParent)
        return (QTreeWidgetItem *)0;
    QTreeWidgetItem *tmpTreeItem = new QTreeWidgetItem(pParent, SUBRCTREEITEM_TYPE);
    pParent->addChild(tmpTreeItem);
    tmpTreeItem->setData(0, RC_NICKNAME, pRcNames);
    //>@规整别名序号
    if(pRcLabel >= pRcNames.count())
        pRcLabel = 0;
    tmpTreeItem->setText(0, pRcNames.at(pRcLabel));
    tmpTreeItem->setData(1, RC_FILETYPE, pRcTypes);
    DiagramScene *tmpScene = (DiagramScene *)(mComponet->m_UiScene);
    tmpScene->m_UiManager->RegulatRcFile(pRcFile);
    tmpTreeItem->setData(1, RC_FILEPATH, pRcFile);
    tmpTreeItem->setIcon(1, QIcon(pRcFile));
    tmpTreeItem->setText(1, getFileName(pRcFile));
    return tmpTreeItem;
}

QTreeWidgetItem* PropertyTree::AddEffectSubRcItem(QTreeWidgetItem* pParent, QString pRcName, QString pRcFile)
{
    QStringList tmpFileTypeList;
    tmpFileTypeList << "svg" << "png" << "jpg" << "bmp" << "gif";
    return AddEffectSubRcItem(pParent, QStringList()<<pRcName, tmpFileTypeList, 0, pRcFile);
}

void PropertyTree::InitSubRcList(QTreeWidgetItem *pItem, EFFECT *pDefaultEffect, QDomElement pStateElement)
{
    QHash<QString, QString> tmpRcHash;
    //>@在之前的操作中已经保证当前pStateElement的特效类型与待设置的特效类型相同，因此此处不再判断
    if(!pStateElement.isNull())
    {
        QDomNodeList tmpList = pStateElement.childNodes();
        for(int i=0;i<tmpList.count();i++)
        {
            QDomElement tmpRcElement = tmpList.at(i).toElement();
            if(tmpRcElement.isNull())
                continue;
            tmpRcHash.insert(tmpRcElement.tagName(), tmpRcElement.text());
        }
    }
    DiagramScene *tmpScene = (DiagramScene *)(mComponet->m_UiScene);
    for(int i=0;i<pDefaultEffect->Resources.count();i++)
    {
        RESOURCE *tmpRc = pDefaultEffect->Resources.at(i);
        if(!tmpRc)
            continue;
        AddEffectSubRcItem(pItem, tmpRc->NickNames, tmpRc->RcTypes, tmpScene->m_UiRcNickLabel, tmpRcHash.take(tmpRc->NickNames.first()));
    }
    //>@针对于一些默认项中没有的特效，也列出来
    QHash<QString, QString>::const_iterator i = tmpRcHash.constBegin();
    QStringList tmpFileTypeList;
    tmpFileTypeList << "svg" << "png" << "jpg" << "bmp" << "gif";
    while (i != tmpRcHash.constEnd())
    {
        AddEffectSubRcItem(pItem, QStringList()<<i.key(), tmpFileTypeList, tmpScene->m_UiRcNickLabel, i.value());
        ++i;
    }
}

//>@根据pEffectName初始化树/控件属性（包括UI文件）
bool PropertyTree::ChangeEffect(QTreeWidgetItem *pItem, QString pEffectName)
{
    if(mCurTreeItem)
    {
        if(itemWidget(mCurTreeItem, 1))
            removeItemWidget(mCurTreeItem, 1);
        mCurTreeItem = 0;
    }
    if(!pItem)
        return false;
    QString tmpStateName = pItem->text(0);
    QString tmpEffectName = pItem->text(1);
    //>@查看当前Item上的特效是否跟要设置的相同，如果相同则退出
    if(!pEffectName.isEmpty() && tmpEffectName.compare(pEffectName) == 0)
        return false;
    //>@设置待配置的特效名，并隐藏填写所有特效类型
    QList<EFFECT*> tmpDefaultEffects = mComponet->GetDefaultEffects(tmpStateName);
    QStringList tmpDefaultEffectNames;
    EFFECT* tmpDefaultEffect = 0;
    for(int i=0;i<tmpDefaultEffects.count();i++)
    {
        EFFECT *tmpEffect = tmpDefaultEffects.at(i);
        if(!tmpEffect)
            continue;
        if(tmpEffect->EffectName.compare(pEffectName) == 0)
            tmpDefaultEffect = tmpEffect;
        tmpDefaultEffectNames.append(tmpEffect->EffectName);
    }
    if(!tmpDefaultEffectNames.contains(D_RCINVALID))
        tmpDefaultEffectNames.push_front(D_RCINVALID);
    //>@查看待配置的特效名是否在系统默认中存在，如果不存在则使用Invalid
    if(!tmpDefaultEffect)
    {
        pEffectName = D_RCINVALID;
    }
    //>@
    pItem->setData(1, EFFECT_NAME, tmpDefaultEffectNames);
    pItem->setText(1, pEffectName);
    //>@查看当前要设置的Effect是否和UI中的是否一样，如果一样，则表示是用于初始化的，使用UI中的数据初始化树
    //>@如果不一样，则删除UI中的，使用默认的数据初始化树。
    mComponet->ModifyStateEffect(tmpStateName, pEffectName);
    //>@删除已有Item
    ClearTreeItem(pItem);
    //>@只有不为Invalid时才需要初始化资源与参数
    if(pEffectName.compare(D_RCINVALID))
    {
        DiagramScene *tmpScene = (DiagramScene *)(mComponet->m_UiScene);
        QDomElement tmpStateElement = tmpScene->m_UiManager->GetComState(mComponet->m_ComElement, tmpStateName);
        //>@先初始化Parameter
        QTreeWidgetItem *tmpParaTreeItem = new QTreeWidgetItem(pItem,
                                                               QStringList()<<tr("Parameter"),
                                                               PARATREEITEM_TYPE);
        InitEffectPara(tmpParaTreeItem, tmpDefaultEffect, tmpStateElement);
        //>@在初始化RC
        QTreeWidgetItem *tmpRcTreeItem = new QTreeWidgetItem(pItem,
                                                             QStringList()<<tr("Resource"),
                                                             RCTREEITEM_TYPE);
        InitSubRcList(tmpRcTreeItem, tmpDefaultEffect, tmpStateElement);
    }
    return true;
}

void PropertyTree::slot_ItemDoubleClicked(QTreeWidgetItem *pItem, int column)
{
    if(!(pItem && column == 1))
        return;
    if(mCurTreeItem)
    {
        if(itemWidget(mCurTreeItem, 1))
            removeItemWidget(mCurTreeItem, 1);
    }
    mCurTreeItem = pItem;
    int tmpTreeType = pItem->type();
    if(tmpTreeType == X_TREEITEM_TYPE ||
       tmpTreeType == Y_TREEITEM_TYPE ||
       tmpTreeType == W_TREEITEM_TYPE ||
       tmpTreeType == H_TREEITEM_TYPE)        //>@双击修改坐标
    {
        if(mComponet->mDragValid)  //>@对于桌面是不可以修改坐标的。
            EditNumValue(pItem, pItem->text(1));
    }
    else if(tmpTreeType == STATETREEITEM_TYPE)    //>@双击修改特效
    {
        EditEnumValue(pItem, pItem->data(1, EFFECT_NAME).toStringList(), pItem->text(1));
    }
    else if(tmpTreeType == SUBPARATREEITEM_TYPE)    //>@双击修改特效参数项
    {
        int type = pItem->data(1, EDIT_TYPE).toInt();
        if(type == 1)
        {
            EditEnumValue(pItem, pItem->data(1, EDIT_DATA).toStringList(), pItem->text(1));
        }
        else if(type == 2)
        {
            EditNumValue(pItem, pItem->text(1));
        }
    }
    else if(tmpTreeType == SUBRCTREEITEM_TYPE)    //>@双击修改特效资源项
    {
        EditFileValue(pItem, pItem->data(1, RC_FILEPATH).toString(), pItem->data(1, RC_FILETYPE).toStringList());
    }
}

void PropertyTree::slot_ItemPressed(QTreeWidgetItem *item, int column)
{
    if(!item)
        return;
    if(mCurTreeItem)
    {
        if(itemWidget(mCurTreeItem, 1))
            removeItemWidget(mCurTreeItem, 1);
    }
    mCurTreeItem = item;
    if(QApplication::mouseButtons() == Qt::RightButton)
    {
        int tmpTreeType = item->type();
        if(tmpTreeType == SUBRCTREEITEM_TYPE)   //>@资源项有右建菜单功能
        {
            mRcMenu->exec(QCursor::pos());
        }
    }
}

void PropertyTree::EditNumValue(QTreeWidgetItem *pItem, QString pValue)
{
    TreeSpinBox* spinBox = new TreeSpinBox(pItem, pValue);
    setItemWidget(pItem, 1, spinBox);
    connect(spinBox, SIGNAL(sValueChanged(QString)), this, SLOT(slot_NumValueChanged(QString)));
}

void PropertyTree::EditFileValue(QTreeWidgetItem *pItem, QString pFilePath, QStringList pFileType)
{
    TreeButtons* buttons = new TreeButtons(pItem, pFilePath, pFileType);
    setItemWidget(pItem, 1, buttons);
    connect(buttons, SIGNAL(sValueChanged(QString)), this, SLOT(slot_FileValueChanged(QString)));
}

void PropertyTree::EditEnumValue(QTreeWidgetItem *pItem, QStringList pEnumValue, QString pValue)
{
    TreeComboBox* comboBox = new TreeComboBox(pItem, pEnumValue, pValue);
    setItemWidget(pItem, 1, comboBox);
    connect(comboBox, SIGNAL(sValueChanged(QString)), this, SLOT(slot_EnumValueChanged(QString)));
}

void PropertyTree::slot_NumValueChanged(QString pValue)
{
    TreeSpinBox* spinBox = VAR_CAST<TreeSpinBox*>(sender());
    if(!spinBox)
        return;
    QString tmpCurValue = spinBox->mTreeItem->text(1);
    if(tmpCurValue.compare(pValue))
    {
        DiagramScene *tmpScene = (DiagramScene *)(mComponet->m_UiScene);
        //>@只有修改的值确切的同步到ui文件以及组件中，才可以更新树项
        int tmpTreeType = spinBox->mTreeItem->type();
        int tmpTreeValue = spinBox->value();
        switch(tmpTreeType)
        {
            case X_TREEITEM_TYPE:  //>@对于坐标，通过先对mComponet进行修改，然后接受其发来的信号来改变Tree上的值
            {
                removeItemWidget(spinBox->mTreeItem, 1);
                spinBox->deleteLater();
                if(mComponet->mParComponet)
                {
                    QRectF tmpSceneRect = mComponet->mParComponet->mapRectToScene(0,0,10,10);
                    mComponet->ModifyComLeft((qreal)(tmpTreeValue+tmpSceneRect.x()), false);
                    tmpScene->StackMoveComponent(mComponet, mComponet->GetVirtualSceneRect());
                }
                break;
            }
            case Y_TREEITEM_TYPE:
            {
                removeItemWidget(spinBox->mTreeItem, 1);
                spinBox->deleteLater();
                if(mComponet->mParComponet)
                {
                    QRectF tmpSceneRect = mComponet->mParComponet->mapRectToScene(0,0,10,10);
                    mComponet->ModifyComTop((qreal)(tmpTreeValue+tmpSceneRect.y()), false);
                    tmpScene->StackMoveComponent(mComponet, mComponet->GetVirtualSceneRect());
                }
                break;
            }
            case W_TREEITEM_TYPE:
            {
                removeItemWidget(spinBox->mTreeItem, 1);
                spinBox->deleteLater();
                mComponet->ModifyComWidth((qreal)tmpTreeValue, false);
                tmpScene->StackMoveComponent(mComponet, mComponet->GetVirtualSceneRect());
                break;
            }
            case H_TREEITEM_TYPE:
            {
                removeItemWidget(spinBox->mTreeItem, 1);
                spinBox->deleteLater();
                mComponet->ModifyComHeight((qreal)tmpTreeValue, false);
                tmpScene->StackMoveComponent(mComponet, mComponet->GetVirtualSceneRect());
                break;
            }
            case SUBPARATREEITEM_TYPE:
            {
                if(tmpScene->StackSetComEffectPara(mComponet, spinBox->mTreeItem->parent()->parent()->text(0),
                                                            spinBox->mTreeItem->text(0), pValue))
                    spinBox->mTreeItem->setText(1, pValue);
                removeItemWidget(spinBox->mTreeItem, 1);
                spinBox->deleteLater();
                break;
            }
            default:
                break;
        }
    }
}

void PropertyTree::slot_EnumValueChanged(QString pValue)
{
    TreeComboBox* comboBox = VAR_CAST<TreeComboBox*>(sender());
    if(!comboBox)
        return;
    QString tmpCurValue = comboBox->mTreeItem->text(1);
    if(tmpCurValue.compare(pValue))
    {
        DiagramScene *tmpScene = (DiagramScene *)(mComponet->m_UiScene);
        //>@只有修改的值确切的同步到ui文件以及组件中，才可以更新树项
        int tmpTreeType = comboBox->mTreeItem->type();
        switch(tmpTreeType)
        {
            case STATETREEITEM_TYPE:
            {
                if(tmpScene->StackChangeComEffect(mComponet, comboBox->mTreeItem->text(0), pValue))
                    ChangeEffect(comboBox->mTreeItem, pValue);
                break;
            }
            case SUBPARATREEITEM_TYPE:
            {
                if(tmpScene->StackSetComEffectPara(mComponet, comboBox->mTreeItem->parent()->parent()->text(0),
                                                            comboBox->mTreeItem->text(0), pValue))
                    comboBox->mTreeItem->setText(1, pValue);
                break;
            }
            default:
                break;
        }
    }
    removeItemWidget(comboBox->mTreeItem, 1);
    comboBox->deleteLater();
}

void PropertyTree::AddRcItem(QTreeWidgetItem *tmpParItem, QString pRcName, QString pRcFile)
{
    DiagramScene *tmpScene = (DiagramScene *)(mComponet->m_UiScene);
    if(tmpScene->StackModifyComRc(mComponet, tmpParItem->parent()->text(0),pRcName, pRcFile))
    {
        AddEffectSubRcItem(tmpParItem, pRcName, pRcFile);
    }
}

void PropertyTree::ModifyRcItem(QTreeWidgetItem *pItem, QString pValue)
{
    DiagramScene *tmpScene = (DiagramScene *)(mComponet->m_UiScene);
    if(tmpScene->StackModifyComRc(mComponet, pItem->parent()->parent()->text(0),
                                              pItem->data(0,RC_NICKNAME).toStringList().first(), pValue))
    {
        pItem->setData(1, RC_FILEPATH, pValue);
        pItem->setIcon(1, QIcon(pValue));
        pItem->setText(1, getFileName(pValue));
    }
}

void PropertyTree::DelRcItem(QTreeWidgetItem *pItem)
{
    DiagramScene *tmpScene = (DiagramScene *)(mComponet->m_UiScene);
    if(tmpScene->StackDelComRc(mComponet, pItem->parent()->parent()->text(0),
                                           pItem->data(0,RC_NICKNAME).toStringList().first()))
    {
        pItem->setData(1, RC_FILEPATH, QString());
        pItem->setIcon(1, QIcon());
        pItem->setText(1, QString());
    }
}

void PropertyTree::slot_FileValueChanged(QString pValue)
{
    TreeButtons* buttons = VAR_CAST<TreeButtons*>(sender());
    if(!buttons)
        return;
    QString tmpCurValue = buttons->mTreeItem->text(1);
    if(tmpCurValue.compare(pValue))
    {
        ModifyRcItem(buttons->mTreeItem, pValue);
    }
    removeItemWidget(buttons->mTreeItem, 1);
    buttons->deleteLater();
}

void PropertyTree::slot_SceneRectChanged(QRectF pRect)
{
    for(int i=0;i<topLevelItemCount();i++)
    {
        QTreeWidgetItem *tmpTreeItem = topLevelItem(i);
        if(!tmpTreeItem)
            continue;
        if(tmpTreeItem->text(0).compare(tr("Geometry")) == 0)
        {
            QRect tmpRect = mComponet->mParComponet->mapRectFromScene(pRect).toRect();
            ChangeRect(tmpTreeItem, tmpRect);
            return;
        }
    }
}

void PropertyTree::slot_NewRC()
{
    QTreeWidgetItem *tmpParItem = mCurTreeItem->parent();
    if(!tmpParItem)
        return;
    QTreeWidgetItem *tmpPreItem = tmpParItem->child(tmpParItem->childCount()-1);
    if(!tmpPreItem)
        return;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Select File"),
                                                    mSelectPath,
                                                    tr("files (*.svg *.png *.jpg *.bmp *.gif)"));
    if (fileName.isEmpty())
        return;
    mSelectPath = getFileDirectory(fileName);

    QString tmpName = tmpPreItem->data(0,RC_NICKNAME).toStringList().first();
    quint32 tmpNum = GetRcNum(tmpName)+1;
    IDE_TRACE_INT(tmpNum);
    QString tmpNewName = QString("RC%1").arg(tmpNum);
    AddRcItem(tmpParItem, tmpNewName, fileName);
}

void PropertyTree::slot_AddRCs()
{
    QTreeWidgetItem *tmpParItem = mCurTreeItem->parent();
    if(!tmpParItem)
        return;

    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select File"),
                                                      mSelectPath,
                                                      tr("files (*.svg *.png *.jpg *.bmp *.gif)"));
    if (files.isEmpty())
        return;
    mSelectPath = getFileDirectory(files.first());

    int i = 0, j = 0;
    QTreeWidgetItem *tmpItem = 0;
    for(;j<tmpParItem->childCount();j++)
    {
        tmpItem = tmpParItem->child(j);
        if(!tmpItem)
            continue;
        if(tmpItem->text(1).isEmpty())
            ModifyRcItem(tmpItem, files.at(i++));
    }
    //>@最后一个item
    if(i < files.count())
    {
        QString tmpName = tmpItem->data(0,RC_NICKNAME).toStringList().first();
        quint32 tmpNum = GetRcNum(tmpName);
        for(;i<files.count();i++)
        {
            tmpNum += 1;
            QString tmpNewName = QString("RC%1").arg(tmpNum);
            AddRcItem(tmpParItem, tmpNewName, files.at(i));
        }
    }
}

void PropertyTree::slot_DelRC()
{
    DelRcItem(mCurTreeItem);
}

void PropertyTree::slot_ClearRCs()
{
    if(!mCurTreeItem)
        return;
    QTreeWidgetItem *tmpItem = 0;
    for(int j=0;j<mCurTreeItem->childCount();j++)
    {
        tmpItem = mCurTreeItem->child(j);
        if(!tmpItem)
            continue;
        if(tmpItem->text(1).isEmpty())
            continue;
        DelRcItem(tmpItem);
    }
}
