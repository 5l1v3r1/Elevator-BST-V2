#include "uipropertywidget.h"
#include "diagramsetting.h"
#include "ui_uipropertywidget.h"

UiPropertyWidget::UiPropertyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UiPropertyWidget)
{
    ui->setupUi(this); 

    mPreTreeItem = 0;
    mComInfo_A = 0;
    mComInfo_B = 0;
}

UiPropertyWidget::~UiPropertyWidget()
{
    disconnect(this, SIGNAL(sUpdate(COM_INFO*)), 0, 0);
    delete ui;
}

void UiPropertyWidget::slot_PropertyTreeDragEnter(QDragEnterEvent* event)
{
    event->accept();
}

void UiPropertyWidget::slot_PropertyTreeDrop(QDropEvent* event)
{
    ui->PropertyTree->UpDropEvent(event);
}

void UiPropertyWidget::slot_PropertyTreeStartDrag(Qt::DropActions actions)
{
    ui->PropertyTree->UpStartDrag(actions);
}

bool UiPropertyWidget::RefreshTree(COM_INFO *pComInfo)
{
    if((!pComInfo)||(pComInfo->ComPath.isEmpty())||(!pComInfo->ComponentPtr))
        return false;
    mComInfo_B = pComInfo;
    QString OBName = pComInfo->ComPath;
    mComInfo_A = GlobalPara.m_ComCategoryGroup.value(OBName);
    if(!mComInfo_A)
        return false;
    mPreTreeItem = 0;
    ui->PropertyTree->ClearAllItem();
//>@Object
    QTreeWidgetItem* OBItem = new QTreeWidgetItem;
    OBItem->setText(0, "Object");
    OBItem->setFont(0, QFont("Arial", 9, QFont::Bold));
    OBItem->setToolTip(0,"/Object:Invalid");
    OBItem->setBackgroundColor(0, QColor(254,235,208));
    OBItem->setBackgroundColor(1, QColor(254,235,208));
    ui->PropertyTree->addTopLevelItem(OBItem);
    QTreeWidgetItem* OBNameItem = new QTreeWidgetItem;
    OBNameItem->setText(0, "ObjectName");
    OBNameItem->setFont(0, QFont("Arial", 9, QFont::Bold));
    OBNameItem->setText(1, OBName);
    OBItem->setToolTip(0,"/Object/ObjectName:Invalid");
    OBItem->addChild(OBNameItem);
//>@Geometry
    QMap<QString, qreal> pointMap;
    pointMap.insert("0Left", pComInfo->ComRect.left());
    pointMap.insert("1Top", pComInfo->ComRect.top());
    pointMap.insert("2Width", pComInfo->ComRect.width());
    pointMap.insert("3Height", pComInfo->ComRect.height());
    QTreeWidgetItem* geometryItem = new QTreeWidgetItem;
    geometryItem->setBackgroundColor(0, QColor(255,250,179));
    geometryItem->setBackgroundColor(1, QColor(255,250,179));
    geometryItem->setText(0, "Geometry");
    geometryItem->setFont(0, QFont("Arial", 9, QFont::Bold));
    ui->PropertyTree->addTopLevelItem(geometryItem);
    QStringList pointList = pointMap.keys();
    for(int i=0;i<pointList.count();i++)
    {
        QString pointName = pointList.at(i);
        qreal pointVaule = pointMap.value(pointName);
        pointName.remove(0, 1);
        QTreeWidgetItem* geometryCldItem = new QTreeWidgetItem;
        geometryCldItem->setText(0, pointName);
        geometryCldItem->setFont(0, QFont("Arial", 9, QFont::Bold));
        geometryCldItem->setText(1, QString("%1").arg(pointVaule));
        geometryCldItem->setData(11, 0, QVariant((quint32)pComInfo));
        geometryCldItem->setToolTip(0, QString("/Geometry/%1:Num").arg(pointName));
        geometryItem->addChild(geometryCldItem);
    }
//>@State
    InitStateTree();
    ui->PropertyTree->expandAll();
}

bool UiPropertyWidget::InitStateTree()
{
    mPreTreeItem = 0;
    for(int i=0;i<mComInfo_A->StateMachines.count();i++)
    {
        STATEMACHINE* stateMachine_A = mComInfo_A->StateMachines.at(i);
        QTreeWidgetItem* stateItem = new QTreeWidgetItem;
        QString stateName_A = stateMachine_A->StateName;
        stateItem->setText(0, stateName_A);
        stateItem->setFont(0, QFont("Arial", 9, QFont::Bold));
        stateItem->setData(10, 1, QVariant((quint32)stateMachine_A));
        stateItem->setData(11, 0, QVariant((quint32)mComInfo_B));
        stateItem->setToolTip(0, QString("/%1:Enum").arg(stateName_A));
        stateItem->setBackgroundColor(0, (i%2)?QColor(255,250,179):QColor(254,235,208));
        stateItem->setBackgroundColor(1, (i%2)?QColor(255,250,179):QColor(254,235,208));
        ui->PropertyTree->addTopLevelItem(stateItem);
        for(int j=0;j<mComInfo_B->StateMachine.count();j++)
        {
            STATE_INFO *stateMachine_B = mComInfo_B->StateMachine.at(j);
            QString stateName_B = stateMachine_B->StateName;
            if(!stateName_B.compare(stateName_A, Qt::CaseInsensitive))
                slot_UpdateState(stateItem, stateMachine_B);
        }
    }
    return true;
}

////>@/Geometry  /StateMachine/Effect/Rc/
////>@/StateName -- EffectName
////>@          EffectPara
////>@           Rcs
////>@           |-           RcName
////>@           |-           RcFile
//void COM_INFO::slot_ModifyValue(QString pPath, QString pValue)
//{
//    if(pPath.isEmpty())
//        return;
//    int index0 = pPath.indexOf('/');
//    if(index0 < 0)
//        return;
//    int index1 = pPath.indexOf('/', index0);
//    if(index1 < 0)
//    {
//        QString tmpString = pPath.mid(index0+1);
//        if(tmpString.compare("Geometry") == 0)
//        {
//            MdyGeometry(pValue);
//        }
//        else if(tmpString.compare("Common") == 0)  //>@修改某个状态的特效类型
//        {
//            MdyGeometry(pValue);
//        }
//    }

//}

bool UiPropertyWidget::slot_UpdateState(QTreeWidgetItem *pItem, STATE_INFO* pStateInfo_B)
{
    pItem->setData(11, 1, QVariant((quint32)pStateInfo_B));
    QString stateName_A = pItem->text(0);
    QString stateName_B = pStateInfo_B->StateName;
    if(stateName_A.compare(stateName_B, Qt::CaseInsensitive))
        return false;
    STATEMACHINE *stateMachine_A = (STATEMACHINE*)(pItem->data(10, 1).toInt());
    if(!stateMachine_A)
        return false;
    pItem->takeChildren();
    EFFECT_INFO *effect_B = &(pStateInfo_B->Effect);
    QString effectName_B = effect_B->EffectType;
    QString effectNames_A;
    for(int i=0;i<stateMachine_A->Effects.count();i++)
    {
        const EFFECT *effect_A = &(stateMachine_A->Effects.at(i));
        QString effectName_A = effect_A->EffectName;
        effectNames_A.append(effectName_A);
        effectNames_A.append("/");
        if(!effectName_A.compare(effectName_B, Qt::CaseInsensitive))
        {
            pItem->setText(1, effectName_A);
            QStringList effectParaList_A = effect_A->EffectPara.split(";", QString::SkipEmptyParts);
            QStringList effectParaList_B = effect_B->EffectPara.split(";", QString::SkipEmptyParts);
            for(int j=0;j<effectParaList_A.count();j++)
            {
                QStringList paraSplit_A = effectParaList_A.at(j).split(":", QString::SkipEmptyParts);
                if(paraSplit_A.count()!=2)
                    continue;
                QString paraName_A = paraSplit_A.at(0);
                QString property;
                if(!paraName_A.compare("Interval", Qt::CaseInsensitive) ||
                   !paraName_A.compare("FrameRange", Qt::CaseInsensitive))
                    property = QString("Num");
                else
                    property = QString("Enum");
                QString paraInfo_A = paraSplit_A.at(1);
                QTreeWidgetItem* paraItem = new QTreeWidgetItem;
                paraItem->setText(0, paraName_A);
                paraItem->setFont(0, QFont("Arial", 9, QFont::DemiBold));
                paraItem->setData(20, 0, QVariant(paraInfo_A));
                paraItem->setData(11, 0, pItem->data(11, 0));
                paraItem->setData(11, 1, QVariant((quint32)(pStateInfo_B)));
                paraItem->setData(11, 6, QVariant((quint32)(effect_B)));
                QString toolTip = pItem->toolTip(0);
                toolTip.remove(toolTip.indexOf(":"), toolTip.length()-toolTip.indexOf(":"));
                paraItem->setToolTip(0, QString("%1/%2:%3").arg(toolTip).arg(paraName_A).arg(property));
                pItem->addChild(paraItem);
                for(int k=0;k<effectParaList_B.count();k++)
                {
                    QStringList paraSplit_B = effectParaList_B.at(k).split(":", QString::SkipEmptyParts);
                    if(paraSplit_B.count()!=2)
                        continue;
                    QString paraName_B = paraSplit_B.at(0);
                    QString paraInfo_B = paraSplit_B.at(1);
                    if(!paraName_B.compare(paraName_A, Qt::CaseInsensitive))
                    {
                        paraItem->setText(1, paraInfo_B);
                    }
                }
            }
            for(int a=0;a<effect_A->Resources.count();a++)
            {
                const RESOURCE *rcInfo_A = &(effect_A->Resources.at(a));
                QString rcName_A = rcInfo_A->RcName;
                QString rcNickName_A = rcInfo_A->RcName;
                if(!rcInfo_A->NickName.isEmpty())
                    rcNickName_A = rcInfo_A->NickName;
                if(!rcNickName_A.compare("Invalid", Qt::CaseInsensitive))
                    continue;
                QTreeWidgetItem* rcItem = new QTreeWidgetItem;
                rcItem->setText(0, rcNickName_A);
                rcItem->setFont(0, QFont("Arial", 9, QFont::DemiBold));
                rcItem->setData(10, 6, QVariant((quint32)rcInfo_A));
                rcItem->setData(11, 0, pItem->data(11, 0));
                rcItem->setData(11, 1, QVariant((quint32)(pStateInfo_B)));
                QString toolTip = pItem->toolTip(0);
                toolTip.remove(toolTip.indexOf(":"), toolTip.length()-toolTip.indexOf(":"));
                rcItem->setToolTip(0, QString("%1/%2:Rc").arg(toolTip).arg(rcName_A));
                pItem->addChild(rcItem);
                for(int b=0;b<effect_B->RcFiles.count();b++)
                {
                    const RESOURCE_INFO *rcInfo_B = &(effect_B->RcFiles.at(b));
                    rcItem->setData(11, 6, QVariant((quint32)rcInfo_B));
                    QString rcName_B = rcInfo_B->RcName;
                    if(!rcName_B.compare(rcName_A, Qt::CaseInsensitive))
                    {
                        QString tmpString = rcInfo_B->RcFile;
                        FILEPATH_TYPE tmpType = RegulatRcFile(tmpString);
                        if((tmpType==PATH_TYPEABS)||(tmpType==PATH_TYPERC))
                        {
                            if(getFileType(tmpString) == TYPE_IMAGEEDITOR)
                                rcItem->setIcon(1, QIcon(tmpString));
                        }
                        rcItem->setText(1, rcInfo_B->RcFile);
                    }
                }
            }
        }
    }
    effectNames_A.remove(effectNames_A.length()-1, 1);
    pItem->setData(20, 0, QVariant(effectNames_A));
}

bool UiPropertyWidget::slot_UpdateComInfo()
{
    if(!mComInfo_B)
        return false;
    mComInfo_B->StateMachine.clear();
    for(int i=0;i<ui->PropertyTree->topLevelItemCount();i++)
    {
        QTreeWidgetItem* StateItem = ui->PropertyTree->topLevelItem(i);
        QString stateItemName = StateItem->text(0);
        QString stateItemVaule = StateItem->text(1);
        if(!stateItemVaule.compare("Invalid", Qt::CaseInsensitive))
            continue;
        if(!stateItemName.compare("Object", Qt::CaseInsensitive))
        {}
        else if(!stateItemName.compare("Geometry", Qt::CaseInsensitive))   //>@更新坐标
        {
            for(int j=0;j<StateItem->childCount();j++)
            {
                QTreeWidgetItem* cldItem = StateItem->child(j);
                QString cldItemName = cldItem->text(0);
                if(!cldItemName.compare("Left", Qt::CaseInsensitive))
                    mComInfo_B->ComRect.setLeft(cldItem->text(1).toDouble());
                else if(!cldItemName.compare("Top", Qt::CaseInsensitive))
                    mComInfo_B->ComRect.setTop(cldItem->text(1).toDouble());
                else if(!cldItemName.compare("Width", Qt::CaseInsensitive))
                    mComInfo_B->ComRect.setWidth(cldItem->text(1).toDouble());
                else if(!cldItemName.compare("Height", Qt::CaseInsensitive))
                    mComInfo_B->ComRect.setHeight(cldItem->text(1).toDouble());
            }
            mComInfo_B->SetUpdateLevel(COM_INFO::UPDATEGEOMETRY);
        }
        else                                                       //>@更新资源
        {
            STATE_INFO* stateInfo_B = new STATE_INFO;
            mComInfo_B->StateMachine.append(stateInfo_B);
            stateInfo_B->StateName = stateItemName;
            stateInfo_B->Effect.EffectType = stateItemVaule;
            stateInfo_B->Effect.RcFiles.clear();
            for(int j=0;j<StateItem->childCount();j++)
            {
                QTreeWidgetItem* cldItem = StateItem->child(j);
                QString cldItemName = cldItem->text(0);
                QString cldItemVaule = cldItem->text(1);
                QString cldItemToolTip = cldItem->toolTip(0);
                if(!cldItemToolTip.endsWith(":Rc", Qt::CaseInsensitive))
                {
                    if(cldItemVaule.isEmpty() == false)
                    {
                        QString para = QString("%1:%2;").arg(cldItemName).arg(cldItemVaule);
                        stateInfo_B->Effect.EffectPara.append(para);
                    }
                }
                else
                {
                    QString tmpString  = cldItem->text(1);
                    if(GetRcFileType(tmpString) != PATH_TYPENONE)
                    {
                        RESOURCE_INFO rcInfo;
                        QString tmpName = getFileName(cldItem->toolTip(0));
                        rcInfo.RcName = tmpName.split(':').at(0);
                        rcInfo.RcFile = tmpString;
                        stateInfo_B->Effect.RcFiles.append(rcInfo);
                    }
                }
            }
            stateInfo_B->Effect.EffectPara.remove(stateInfo_B->Effect.EffectPara.length()-1, 1);
            IDE_TRACE_STR(stateInfo_B->Effect.EffectPara);
            mComInfo_B->SetUpdateLevel(COM_INFO::UPDATESTATE);
        }
    }
    emit sUpdate(mComInfo_B);
}

void UiPropertyWidget::on_PropertyTree_itemPressed(QTreeWidgetItem *item, int column)
{
    if((!item)||(mPreTreeItem == item))return;
    if(mPreTreeItem)
    {
        QWidget* widget = ui->PropertyTree->itemWidget(mPreTreeItem, 1);
        if(!widget)return;
        ui->PropertyTree->removeItemWidget(mPreTreeItem, 1);
        widget->deleteLater();
        mPreTreeItem = 0;
    }
    QString curLabel = item->toolTip(0);
    IDE_TRACE_STR(curLabel);
    QStringList curLabelSplit = curLabel.split(":", QString::SkipEmptyParts);
    if(curLabelSplit.count()!=2)return;
    QString curProperty = curLabelSplit.at(1);
    if(!curProperty.compare("Invalid", Qt::CaseInsensitive))
    {
        return;
    }
    else if(!curProperty.compare("Num", Qt::CaseInsensitive))
    {
        QMutiSpinBox* spinBox = new QMutiSpinBox(item, 0);
        spinBox->setRange(0, 65535);
        spinBox->setValue(item->text(1).toInt());
        spinBox->setMaximumHeight(24);
        ui->PropertyTree->setItemWidget(item, 1, spinBox);
        connect(spinBox, SIGNAL(sChanged()), this, SLOT(slot_UpdateComInfo()));
        connect(spinBox, SIGNAL(sRefreshCldItem(QTreeWidgetItem*, STATE_INFO*)), this, SLOT(slot_UpdateState(QTreeWidgetItem*, STATE_INFO*)));
    }
    else if(!curProperty.compare("Enum", Qt::CaseInsensitive))
    {
        QString paras = item->data(20, 0).toString();
        if(paras.isNull())
        {
            STATEMACHINE* stateInfo_A = (STATEMACHINE*)(item->data(10, 1).toUInt());
            if(!stateInfo_A)return;
            for(int i=0;i<stateInfo_A->Effects.count();i++)
            {
                const EFFECT *effect_A = &(stateInfo_A->Effects.at(i));
                QString effectName_A = effect_A->EffectName;
                paras.append(effectName_A);
                paras.append("/");
            }
            paras.remove(paras.length()-1, 1);
        }
        QMutiComboBox* comBox = new QMutiComboBox(item, 0);
        QString itemText = item->text(1);
        QStringList paraList;
        paraList << "Invalid";
        paraList << paras.split("/", QString::SkipEmptyParts);
        comBox->addItems(paraList);
        for(int i=0; i<paraList.count();i++)
        {
            QString para = paraList.at(i);
            if(!para.compare(itemText, Qt::CaseInsensitive))
                comBox->setCurrentIndex(i);
        }
        comBox->setMaximumHeight(24);
        ui->PropertyTree->setItemWidget(item, 1, comBox);
        connect(comBox, SIGNAL(sChanged()), this, SLOT(slot_UpdateComInfo()));
        connect(comBox, SIGNAL(sRefreshCldItem(QTreeWidgetItem*, STATE_INFO*)), this, SLOT(slot_UpdateState(QTreeWidgetItem*, STATE_INFO*)));
    }
    else if(!curProperty.compare("Rc", Qt::CaseInsensitive))
    {
        QWidget* widget = new QWidget(0);
        widget->setAttribute(Qt::WA_OpaquePaintEvent, true);
        QHBoxLayout* hLayout = new QHBoxLayout(widget);
        hLayout->setSpacing(0);
        hLayout->setMargin(0);
        QSpacerItem* spacerItem = new QSpacerItem(10, 200, QSizePolicy::Expanding, QSizePolicy::Minimum);
        hLayout->addSpacerItem(spacerItem);

        QMutiPushButton* modifyBtn = new QMutiPushButton(item, QString("..."), QIcon(":/Menu/rc/Menu/open.png"), 0);
        modifyBtn->setMaximumWidth(25);
        modifyBtn->setMaximumHeight(25);
        hLayout->addWidget(modifyBtn, 0);

        QMutiPushButton* clearBtn = new QMutiPushButton(item, QString("<-"), QIcon(":/Menu/rc/Menu/undo.png"), 0);
        clearBtn->setMaximumWidth(25);
        clearBtn->setMaximumHeight(25);
        hLayout->addWidget(clearBtn, 1);
        widget->setMaximumHeight(24);
        ui->PropertyTree->setItemWidget(item, 1, widget);
        connect(modifyBtn, SIGNAL(sChanged()), this, SLOT(slot_UpdateComInfo()));
        connect(modifyBtn, SIGNAL(sRefreshCldItem(QTreeWidgetItem*, STATE_INFO*)), this, SLOT(slot_UpdateState(QTreeWidgetItem*, STATE_INFO*)));
        connect(clearBtn, SIGNAL(sChanged()), this, SLOT(slot_UpdateComInfo()));
        connect(clearBtn, SIGNAL(sRefreshCldItem(QTreeWidgetItem*, STATE_INFO*)), this, SLOT(slot_UpdateState(QTreeWidgetItem*, STATE_INFO*)));
    }
    mPreTreeItem = item;
}

void UiPropertyWidget::on_BtnConfigure_clicked()
{
    emit sEffectSetting();
}
