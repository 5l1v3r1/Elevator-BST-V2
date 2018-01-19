#include "eventmapform.h"
#include "ui_eventmapform.h"

EventMapForm::EventMapForm(QString pMapPath, QWidget *parent) :
    TabPage(parent),
    ui(new Ui::EventMapForm)
{
    ui->setupUi(this);
    mMutex = new QMutex(QMutex::Recursive);
    mUIManager = 0;
    mMapManager = 0;
    m_TreeItem = 0;

    InitConnect();
    ui->EventMapTable->setEnabled(false);
    mMapPAth = pMapPath;
    QString UIPath = getFileDirectory(mMapPAth);
    UIPath.append(D_UINAME);
    SycncActionTreeFromTheme(UIPath);
    SycncEventTableFromMapAndTheme(mMapPAth);
    ui->EventTree->expandAll();
    ui->ActionTree->expandAll();
}

EventMapForm::~EventMapForm()
{
    if(mMutex)
        delete mMutex;
    ReleaseConnect();
    CloseThemeUI();
    CloseMap();
    delete ui;
}

void EventMapForm::InitConnect()
{
    connect(ui->EventTree, SIGNAL(sDragEnter(QDragEnterEvent*)), this, SLOT(slot_EventTreeDragEnter(QDragEnterEvent*)));
    connect(ui->EventTree, SIGNAL(sDrop(QDropEvent*)), this, SLOT(slot_EventTreeDrop(QDropEvent*)));
    connect(ui->EventTree, SIGNAL(sStartDrag(Qt::DropActions)), this, SLOT(slot_EventTreeStartDrag(Qt::DropActions)));

    connect(ui->EventMapTable, SIGNAL(sDragEnter(QDragEnterEvent*)), this, SLOT(slot_EventMapTableDragEnter(QDragEnterEvent*)));
    connect(ui->EventMapTable, SIGNAL(sDrop(QDropEvent*)), this, SLOT(slot_EventMapTableDrop(QDropEvent*)));
    connect(ui->EventMapTable, SIGNAL(sStartDrag(Qt::DropActions)), this, SLOT(slot_EventMapTableStartDrag(Qt::DropActions)));

    connect(ui->ActionTree, SIGNAL(sDragEnter(QDragEnterEvent*)), this, SLOT(slot_ActionTreeDragEnter(QDragEnterEvent*)));
    connect(ui->ActionTree, SIGNAL(sDrop(QDropEvent*)), this, SLOT(slot_ActionTreeDrop(QDropEvent*)));
    connect(ui->ActionTree, SIGNAL(sStartDrag(Qt::DropActions)), this, SLOT(slot_ActionTreeStartDrag(Qt::DropActions)));
    connect(ui->ActionTree, SIGNAL(sMousePressEvent(QMouseEvent*)), this, SLOT(slot_MousePressEvent(QMouseEvent*)));
}

void EventMapForm::ReleaseConnect()
{
    disconnect(ui->EventTree, SIGNAL(sDragEnter(QDragEnterEvent*)), 0, 0);
    disconnect(ui->EventTree, SIGNAL(sDrop(QDropEvent*)), 0, 0);
    disconnect(ui->EventTree, SIGNAL(sStartDrag(Qt::DropActions)), 0, 0);

    disconnect(ui->EventMapTable, SIGNAL(sDragEnter(QDragEnterEvent*)), 0, 0);
    disconnect(ui->EventMapTable, SIGNAL(sDrop(QDropEvent*)), 0, 0);
    disconnect(ui->EventMapTable, SIGNAL(sStartDrag(Qt::DropActions)), 0, 0);

    disconnect(ui->ActionTree, SIGNAL(sDragEnter(QDragEnterEvent*)), 0, 0);
    disconnect(ui->ActionTree, SIGNAL(sDrop(QDropEvent*)), 0, 0);
    disconnect(ui->ActionTree, SIGNAL(sStartDrag(Qt::DropActions)), 0, 0);
}
/**
//>@Fun*****************************************************
*/
void EventMapForm::ShowPixmap(QString pPath, QIcon pIcon)
{
    int height = ui->ViewLabel->size().height();
    int width = ui->ViewLabel->size().width();
    height = (height<200)? height: 200;
    width = (width<250)? width: 250;
    QPixmap pixmap(pPath);
    if(!pixmap.isNull())
        ui->ViewLabel->setPixmap(pixmap.scaled(width, height, Qt::IgnoreAspectRatio, Qt::FastTransformation));
    else if(!pIcon.isNull())
        ui->ViewLabel->setPixmap(pIcon.pixmap(width, height));
}
/**
//>@Sycnc*****************************************************
*/
bool EventMapForm::SycncActionTreeFromTheme(QString pPath)
{
    mUIPAth = pPath;
    if(!OpenThemeUI(pPath))
       return false;
    ui->UiFilePath->setText(mUIPAth);
    for(int i=0;i<ui->ActionTree->topLevelItemCount();i++)
    {
        QTreeWidgetItem* parActionTreeItem = ui->ActionTree->topLevelItem(i);
        parActionTreeItem->setDisabled(false);
        parActionTreeItem->takeChildren();
    }
	ui->ActionTree->clear();
    QList<COM_TYPE> keyList = mUIManager->m_ComElementGroup.keys();
    for(int i=0;i<keyList.count();i++)
    {
        COM_TYPE key = keyList.at(i);
        QDomElement comStruct = mUIManager->m_ComElementGroup.value(key);
        ActionType actionType = GetActionType(key);
        if(actionType == ActionInvalid)
            continue;
        QString OBName = getComponetName(key);
        QTreeWidgetItem* parItem;
        QList<QTreeWidgetItem*> parItemList = ui->ActionTree->findItems(OBName, Qt::MatchFixedString, 0);
        if(parItemList.isEmpty())
        {
            parItem = new QTreeWidgetItem;
            parItem->setIcon(0, QIcon(":/Menu/rc/Menu/grid.png"));
            parItem->setText(0, OBName);
            parItem->setToolTip(0, OBName + "-Pre/Next");
            ui->ActionTree->addTopLevelItem(parItem);
        }
        else
        {
            parItem = parItemList.at(0);
        }
        QDomNodeList tmpStateList = mUIManager->GetComStateList(comStruct);
        QHash<QString,QString> rcInfo;
        for(int j=0;j<tmpStateList.count();j++)
        {
            QDomElement tmpStateElement = tmpStateList.at(j).toElement();
            QDomNodeList tmpRcList = tmpStateElement.childNodes();
            for(int k=0;k<tmpRcList.count();k++)
            {
                QDomElement tmpRcElement = tmpRcList.at(k).toElement();
                QString tmpRcName = tmpRcElement.tagName();
                QString tmpRcFile = tmpRcElement.text();
                if(tmpRcName.compare(D_RCINVALID, Qt::CaseInsensitive) == 0)
                    continue;
                rcInfo.insert(tmpRcName, tmpRcFile);
            }
        }
        QStringList rcNameList = rcInfo.keys();
        for(int L=0;L<rcNameList.count();L++)
        {
            QTreeWidgetItem* rcItem = new QTreeWidgetItem;
            QString rcName = rcNameList.at(L);
            QString path = QString("%1%2").arg(getFileDirectory(mUIPAth)).arg(rcInfo.value(rcName));
            QString toolTip = QString("*");
            int rcNum = 0;
            if(GetRcNum(rcNum, rcName))
            {
                toolTip = QString("%1-%2").arg(OBName).arg(rcNum);
            }
            rcItem->setIcon(0, QIcon(path));
            rcItem->setText(0, rcName);
            rcItem->setToolTip(0, toolTip);
            rcItem->setText(1, path);
            parItem->addChild(rcItem);
        }
    }
    for(int m=0;m<ui->ActionTree->topLevelItemCount();m++)
    {
        QTreeWidgetItem* parActionTreeItem = ui->ActionTree->topLevelItem(m);
        if(parActionTreeItem->childCount() == 0)
        {
            parActionTreeItem->setDisabled(true);
        }
    }
    return true;
}

bool EventMapForm::SycncEventTableFromMapAndTheme(QString pPath)
{
    if(mUIManager == 0)
    {
        QMessageBox msgBox;
        msgBox.setText("Please Selcet a UI, Then check refresh button");
        msgBox.exec();
        return false;
    }
    if(!OpenMap(pPath))
    {
        if(!CreatMap(pPath))
            return false;
    }
    ui->EventMapTable->setEnabled(true);
    int totalRowCnt = ui->EventMapTable->rowCount();
    for(int i=0;i<totalRowCnt;i++)
    {
        ui->EventMapTable->removeRow(0);
    }
    int rowCnt = 0;
    QStringList labelList = mMapManager->m_CfgparaElementGroup.keys();
    for(int i=0;i<labelList.count();i++)
    {
        QString eventLabel = labelList.at(i);
        if(GetEventType(eventLabel) == EventInvalid)
            continue;
        QDomElement parElement =  mMapManager->m_CfgparaElementGroup.value(eventLabel);
        if(parElement.isNull())
            continue;
        QDomNodeList nodeList = parElement.childNodes();
        for(int j=0;j<nodeList.count();j++)
        {
            QDomNode node = nodeList.at(j);
            if(node.isNull())
                continue;
            QDomElement element = node.toElement();
            if(element.isNull())
                continue;
            QStringList List = eventLabel.split("/", QString::SkipEmptyParts);
            if(List.isEmpty())
                continue;
//>@AddEvent
            QString eventName = List.at(List.count() - 1);
            QTreeWidgetItem* eventTreeItem;
            for(int k=0;k<ui->EventTree->topLevelItemCount();k++)
            {
                QTreeWidgetItem* parEventTreeItem = ui->EventTree->topLevelItem(k);
                for(int L=0;L<parEventTreeItem->childCount();L++)
                {
                    eventTreeItem = parEventTreeItem->child(L);
                    QString eventText = eventTreeItem->text(0);
                    if(!eventName.compare(eventText, Qt::CaseInsensitive))
                        goto GT_FindEvent;
                }
            }
            return false;
GT_FindEvent:
            QTableWidgetItem* eventItem = new QTableWidgetItem;
            eventItem->setIcon(eventTreeItem->icon(0));
            eventItem->setText(element.tagName());
            eventItem->setToolTip(eventTreeItem->toolTip(0));
            ui->EventMapTable->setRowCount(rowCnt+1);
            ui->EventMapTable->setItem(rowCnt,0,eventItem);
//>@AddAction
            QString eventInfo = element.text();
            QStringList actionTextList = eventInfo.split(";", QString::SkipEmptyParts);
            for(int m=0;m<actionTextList.count();m++)
            {
                QString actionText = actionTextList.at(m);
                QTreeWidgetItem* actionTreeItem;
                QHash<int, QTreeWidgetItem*>paraItemHash;
                for(int n=0;n<ui->ActionTree->topLevelItemCount();n++)
                {
                    QTreeWidgetItem* parActionTreeItem = ui->ActionTree->topLevelItem(n);
                    QString actionTooltip = parActionTreeItem->toolTip(0);
                    if(actionText.contains("Next"))
                    {
                        actionTooltip = actionTooltip.remove("Next", Qt::CaseInsensitive);
                        actionTooltip = actionTooltip.remove("/", Qt::CaseInsensitive);
                        actionTooltip = actionTooltip.remove("Pre", Qt::CaseInsensitive);
                        actionTooltip += "Next";
                    }
                    else if(actionText.contains("Pre"))
                    {
                        actionTooltip = actionTooltip.remove("Next", Qt::CaseInsensitive);
                        actionTooltip = actionTooltip.remove("/", Qt::CaseInsensitive);
                        actionTooltip = actionTooltip.remove("Pre", Qt::CaseInsensitive);
                        actionTooltip += "Pre";
                    }
                    if(!actionText.compare(actionTooltip, Qt::CaseInsensitive))
                    {
                        parActionTreeItem->setToolTip(0, actionTooltip);
                        actionTreeItem = parActionTreeItem;
                        goto GT_FindAction;
                    }
                    for(int r=0;r<parActionTreeItem->childCount();r++)
                    {
                        actionTreeItem = parActionTreeItem->child(r);
                        actionTooltip = actionTreeItem->toolTip(0);
                        if(actionText.contains("Floor"))
                        {
                            QStringList paraList = actionText.split("-", QString::SkipEmptyParts);
                            for(int i=1;i<paraList.count();i++)
                            {
                                QString key = QString("Floor-%1").arg(paraList.at(i));
                                if(!key.compare(actionTooltip, Qt::CaseInsensitive))
                                    paraItemHash.insert(i, parActionTreeItem->child(r));
                            }
                        }
                        else
                        {
                            if(!actionText.compare(actionTooltip, Qt::CaseInsensitive))
                                goto GT_FindAction;
                        }
                    }
                }
                {
                    QSize size(100, 120);
                    QImage fillImage(size, QImage::Format_ARGB32_Premultiplied);
                    QPainter painter(&fillImage);
                    painter.setCompositionMode(QPainter::CompositionMode_Source);
                    painter.fillRect(fillImage.rect(), Qt::transparent);
                    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
                    quint32 width = size.width();
                    quint32 height = size.height();
                    QList<int> paraItemKeyList = paraItemHash.keys();
                    quint32 bitCnt = paraItemKeyList.count();
                    if(bitCnt == 0)
                    {
                        IDE_TRACE_INT(bitCnt);
                        return false;
                    }
                    quint32 width0 = width/bitCnt;
                    QString text = QString("Floor");
                    for(int i=0;i<bitCnt;i++)
                    {
                        QTreeWidgetItem* item = paraItemHash.value(paraItemKeyList.at(i));
                        QImage image = item->icon(0).pixmap(100,120).toImage();
                        painter.drawImage(QRect(width0*i,0,width0,height), image, image.rect());
                        text.append("-");
                        text.append(QString("%1").arg(paraItemKeyList.at(i)));
                    }
                    painter.end();
                    actionTreeItem = new QTreeWidgetItem;
                    actionTreeItem->setIcon(0, QIcon(QPixmap::fromImage(fillImage)));
                    actionTreeItem->setToolTip(0, text);
                    goto GT_FindAction;
                }
                continue;
GT_FindAction:
                QStringList list =  actionText.split("-", QString::SkipEmptyParts);
                if(list.isEmpty())
                    continue;
                QString action = list.at(0);
                int row = rowCnt;
                int column = 0;
                for(int s=1;s<ui->EventMapTable->columnCount();s++)
                {
                    QTableWidgetItem *columnHeaderItem = ui->EventMapTable->horizontalHeaderItem(s);
                    QString columnHeaderName = columnHeaderItem->text();
                    if(!action.compare(columnHeaderName, Qt::CaseInsensitive))
                    {
                        column = s;
                        goto GT_InitFindColumn;
                    }
                }
                continue;
GT_InitFindColumn:
                QTableWidgetItem* actionItem = new QTableWidgetItem;
                actionItem->setIcon(actionTreeItem->icon(0));
                actionItem->setText(actionTreeItem->toolTip(0));
                actionItem->setToolTip(actionTreeItem->text(1));
                ui->EventMapTable->setItem(row,column,actionItem);
            }
            rowCnt++;
        }
    }
    ui->EventMapTable->sortByColumn(0, Qt::AscendingOrder);
    return true;
}

void EventMapForm::SycncMapFromUI()
{
    if(mMapManager == 0)
        return;
    mMapManager->ClrMap();
    for(int i=0;i<ui->EventMapTable->rowCount();i++)
    {
        QTableWidgetItem* eventItem = ui->EventMapTable->item(i, 0);
        DomInfo domInfo;
        domInfo.mParLabel = eventItem->toolTip().insert(0, "/Map");
        domInfo.mTagName = eventItem->text();
        domInfo.mAtPara.clear();
        domInfo.mAtList.clear();
        for(int j=1;j<ui->EventMapTable->columnCount();j++)
        {
            QTableWidgetItem* actionItem = ui->EventMapTable->item(i, j);
            if(actionItem == 0)
                continue;
            domInfo.mText.append(actionItem->text());
            domInfo.mText.append(QString(";"));
        }
        domInfo.mText.remove(domInfo.mText.length()-1, 1);
        mMapManager->AddDomNewNode(domInfo);
    }
    SaveMap();
}
/**
//>@AddToMapForm*****************************************************
*/
bool EventMapForm::AddEventToMapForm(QIcon pIcon, QString pText, QString pToolTip, QPoint pPoint)
{
    for(int i=0;i<ui->EventMapTable->rowCount();i++)
    {
        QTableWidgetItem *eventItem = ui->EventMapTable->item(i, 0);
        if(eventItem == 0)
            continue;
        QString eventText = eventItem->text();
        if(!pText.compare(eventText, Qt::CaseInsensitive))
            return false;
    }
    int row=0,column=0;
    if(!MapToTable(ui->EventMapTable, pPoint, row, column))
    {
        ui->EventMapTable->setRowCount(ui->EventMapTable->rowCount()+1);
        row = ui->EventMapTable->rowCount()-1;
    }
    column = 0;
    QTableWidgetItem* curItem = new QTableWidgetItem;
    curItem->setIcon(pIcon);
    curItem->setText(pText);
    curItem->setToolTip(pToolTip);
    ui->EventMapTable->setItem(row,column,curItem);
    return true;
}

bool EventMapForm::AddActionToMapForm(QIcon pIcon, QString pText, QString pToolTip, QPoint pPoint)
{
    QStringList labelList = pText.split("-", QString::SkipEmptyParts);
    if(labelList.count()!=2)
        return false;
    QString type = labelList.at(0);
    QString para = labelList.at(1);
    int row=0,column=0;
    if(!MapToTable(ui->EventMapTable, pPoint, row, column))
        return false;
    QString columnName = labelList.at(0);
    for(int i=1;i<ui->EventMapTable->columnCount();i++)
    {
        QTableWidgetItem *columnHeaderItem = ui->EventMapTable->horizontalHeaderItem(i);
        if(columnHeaderItem == 0)
            continue;
        QString columnHeaderName = columnHeaderItem->text();
        if(!columnName.compare(columnHeaderName, Qt::CaseInsensitive))
        {
            column = i;
            goto GT_AddFindColumn;
        }
    }
    return false;
GT_AddFindColumn:
    QTableWidgetItem *eventItem = ui->EventMapTable->item(row, 0);
    if(eventItem == 0)
        return false;
    QTableWidgetItem* existItem = ui->EventMapTable->item(row, column);
    if(existItem)
    {
        QString itemName = existItem->text();
        QStringList listItemName = itemName.split("-", QString::SkipEmptyParts);
        if(!listItemName.isEmpty()&&
          (!listItemName.contains("Next", Qt::CaseInsensitive))&&
          (!listItemName.contains("Pre", Qt::CaseInsensitive))&&
          (!listItemName.at(0).compare("Floor", Qt::CaseInsensitive)))
        {
            quint32 cnt = listItemName.count();
            float divide = 0;
            switch(cnt)
            {
                case 2:
                    divide = 0.5;
                    pText = QString("Floor-%1-%2").arg(listItemName.at(1)).arg(para);
                    break;
                case 3:
                    divide = 0.667;
                    pText = QString("Floor-%1-%2-%3").arg(listItemName.at(1)).arg(listItemName.at(2)).arg(para);
                    break;
                default:break;
            }
            if(divide != 0)
            {
                QImage image(pToolTip);
                if(image.isNull())
                {
                    image = pIcon.pixmap(100,120).toImage();
                }
                pIcon = GenerateFloorIcon(QSize(100,120),
                                          divide,
                                          existItem->icon().pixmap(100,120).toImage(),
                                          image);
                pToolTip = QString();
            }
        }
    }
    QTableWidgetItem* curItem = new QTableWidgetItem;
    curItem->setIcon(pIcon);
    curItem->setText(pText);
    curItem->setToolTip(pToolTip);
    ui->EventMapTable->setItem(row,column,curItem);
    return true;
}


QIcon EventMapForm::GenerateFloorIcon(QSize pSize, float pDivide, QImage pImage0, QImage pImage1)
{
    QImage fillImage(pSize, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&fillImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(fillImage.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    quint32 width = pSize.width();
    quint32 height = pSize.height();
    quint32 width0 = width*pDivide;
    quint32 width1 = width - width0;
    painter.drawImage(QRect(0,0,width0,height), pImage0, pImage0.rect());
    painter.drawImage(QRect(width0,0,width1,height), pImage1, pImage1.rect());
    painter.end();
    return QIcon(QPixmap::fromImage(fillImage));
}

EventMapForm::EventType EventMapForm::GetEventType(QString pKey)
{
    if(!pKey.compare("/Com/Floor", Qt::CaseInsensitive))
        return EventFloor;
    else if(!pKey.compare("/Com/Arrow", Qt::CaseInsensitive))
        return EventArrow;
    else if(!pKey.compare("/Com/Function", Qt::CaseInsensitive))
        return EventFunction;
    else if(!pKey.compare("/Com/Audio", Qt::CaseInsensitive))
        return EventAudio;
    else if(!pKey.compare("/System/Timer", Qt::CaseInsensitive))
        return EventTimer;
    else if(!pKey.compare("/System/Alarm", Qt::CaseInsensitive))
        return EventAlarm;
    else
        return EventInvalid;
}
EventMapForm::EventParaType EventMapForm::GetEventParaType(QString pKey)
{
    if(!pKey.compare("Num", Qt::CaseInsensitive))
        return Num;
    else if(!pKey.compare("Str", Qt::CaseInsensitive))
        return Str;
    else if(!pKey.compare("Enum", Qt::CaseInsensitive))
        return Enum;
    else if(!pKey.compare("Check", Qt::CaseInsensitive))
        return Check;
    else
        return EventParaInvalid;
}
QString EventMapForm::GetEventInfo(QWidget* pPage)
{
    QString OBName = pPage->objectName();
    QStringList labelList = OBName.split("_", QString::SkipEmptyParts);
    if(labelList.count() != 2)
        return QString();
    QString parTagname = labelList.at(0);
    QString tagname = labelList.at(1);
    QHash<int, QString> paraHash;

    QGroupBox* groupBox = pPage->findChild<QGroupBox *>(tagname);
    if(groupBox == 0)
        return QString();
    QObjectList cldList = groupBox->children();
    for(int i=0;i<cldList.count();i++)
    {
        QObject* cld = cldList.at(i);
        QString cldName = cld->objectName();
        if(cldName.contains(tagname) && cldName.contains("Para"))
        {
            QStringList infoList = cldName.split("_", QString::SkipEmptyParts);
            if(infoList.count() != 4)
                continue;
            QString realOBName = infoList.at(1);
            QString paraDex = infoList.at(2);
            paraDex.replace("Para", "", Qt::CaseInsensitive);
            bool flag = false;
            int key = paraDex.toInt(&flag, 10);
            if(!flag)
                continue;
            EventParaType eventParaType = GetEventParaType(infoList.at(3));
            if(eventParaType == EventParaInvalid)
                continue;
            QString vaule;
            QComboBox* comBox = qobject_cast<QComboBox*>(cld);
            QSlider* slider = qobject_cast<QSlider*>(cld);
            QRadioButton* RDBtn = qobject_cast<QRadioButton*>(cld);
            QCheckBox* checkBox = qobject_cast<QCheckBox*>(cld);
            QLabel* label = qobject_cast<QLabel*>(cld);
            if(comBox != 0)
            {
                switch(eventParaType)
                {
                    case Num:vaule = QString("%1").arg(comBox->currentIndex());break;
                    case Str:vaule = comBox->currentText();break;
                    default:continue;
                }
            }
            else if(slider != 0)
            {
                switch(eventParaType)
                {
                    case Num:vaule = QString("%1").arg(slider->value());break;
                    default:continue;
                }
            }
            else if((RDBtn != 0)&&(RDBtn->isChecked()))
            {
                switch(eventParaType)
                {
                    case Enum:vaule = realOBName;break;
                    default:continue;
                }
            }
            else if(checkBox != 0)
            {
                switch(eventParaType)
                {
                    case Check:
                        if(checkBox->isChecked())
                            vaule = QString("Enable");
                        else
                            vaule = QString("Disable");
                        break;
                    default:continue;
                }
            }
            else if(label != 0)
            {
                switch(eventParaType)
                {
                    case Str:
                        vaule = label->text();
                        break;
                    default:continue;
                }
            }
            else
            {
                continue;
            }
            paraHash.insert(key, vaule);
        }
    }
    if(!paraHash.isEmpty())
    {
        QString event = tagname;
        for(int j=0;j<=paraHash.count();j++)
        {
            event.append("-");
            event.append(paraHash.value(j));
        }
        if(event.endsWith("-"))
            event.remove(event.length()-1,1);
        QString label = QString("/%1/%2/%3").arg(parTagname).arg(tagname).arg(event);
        return label;
    }
    return QString();
}

bool EventMapForm::MapToTable(QTableWidget* pTableWidget, QPoint pPoint, int& pRow, int& pColumn)
{
    QModelIndex model = pTableWidget->indexAt(pPoint);
    if(!model.isValid())
        return false;
    pRow = model.row();
    pColumn = model.column();
    return true;
}

EventMapForm::ActionType EventMapForm::GetActionType(COM_TYPE pKey)
{
    if(pKey == COM_ARROW)
    {
        return ActionArrow;
    }
    else if(pKey == COM_FLOOR)
    {
        return ActionFloor;
    }
    else if(pKey == COM_FUNCTION)
    {
        return ActionFunction;
    }
    else if(pKey == COM_DESKTOP)
    {
        return ActionDesktop;
    }
    else if(pKey == COM_VIDEO)
    {
        return ActionVideo;
    }
    else if(pKey == COM_AUDIO)
    {
        return ActionAudio;
    }
    else if(pKey == COM_IMAGE)
    {
        return ActionImage;
    }
    else if(pKey == COM_TEXT)
    {
        return ActionTxt;
    }
    else if(pKey == COM_LOGO)
    {
        return ActionLogo;
    }
    else if(pKey == COM_NAMEPLATE)
    {
        return ActionNamePlate;
    }
    else if(pKey == COM_WEATHER)
    {
        return ActionWeather;
    }
    else
    {
        return ActionInvalid;
    }
}
bool EventMapForm::GetRcNum(int& pNum, QString pKey)
{
    for(int i=0;i<pKey.length();i++)
    {
        QString key = pKey;
        key.remove(0, i);
        if(StrToInt(pNum, 10, key))
            return true;
    }
    return false;
}

bool EventMapForm::OpenThemeUI(QString pPath)
{
    mUIPAth = pPath;
    CloseThemeUI();
    mUIManager = new uiManager(pPath);
    if(!mUIManager->OpenUI())
    {
        CloseThemeUI();
        IDE_TRACE_STR("OpenThemeUI#Failed");
        return false;
    }
    return true;
}
void EventMapForm::CloseThemeUI()
{
    if(mUIManager)
    {
        mUIManager->m_IsChanged = false;
        delete mUIManager;
        mUIManager = 0;
    }
}

bool EventMapForm::CreatMap(QString pPath)
{
    CloseMap();
    mMapManager = new mapManager(pPath);
    if(!mMapManager->NewMAP())
        return false;
    CloseMap();
    if(!OpenMap(pPath))
        return false;
    return true;
}
bool EventMapForm::OpenMap(QString pPath)
{
    CloseMap();
    mMapManager = new mapManager(pPath);
    if(!mMapManager->OpenMAP())
    {
        mMapManager->m_IsChanged = false;
        delete mMapManager;
        mMapManager = 0;
        IDE_TRACE_STR("OpenMap#Failed");
        return false;
    }
    return true;
}
void EventMapForm::CloseMap()
{
    if(mMapManager)
    {
        mMapManager->m_IsChanged = false;
        delete mMapManager;
        mMapManager = 0;
    }
}
bool EventMapForm::SaveMap()
{
    if(mMapManager)
    {
        mMapManager->m_IsChanged = true;
        if(!mMapManager->SaveMAP())
        {
            IDE_TRACE_STR("SaveMap#Failed");
            mMapManager->m_IsChanged = false;
            return false;
        }
        mMapManager->m_IsChanged = false;
        return true;
    }
    return false;
}

void EventMapForm::SyncTreeItem(FileTreeItem *pTreeItem)
{
    m_TreeItem = pTreeItem;
}

/**
//>@EventTreeDragAndDrop*****************************************************
*/
void EventMapForm::slot_EventTreeDragEnter(QDragEnterEvent* event)
{
//    IDE_TRACE_STR("EventTreeDragEnter");
    event->accept();
}

void EventMapForm::slot_EventTreeDrop(QDropEvent *event)
{
//    IDE_TRACE_STR("EventTreeDrop");
    event->accept();
    int preCnt = ui->EventMapTable->rowCount();
    QList<QTableWidgetItem*> itemList = ui->EventMapTable->selectedItems();
    int delCnt = 0;
    for(int i=0;i<itemList.count();i++)
    {
        QTableWidgetItem* selectItem = itemList.at(i);
        if(selectItem->column() != 0)
            continue;
        delCnt ++;
        ui->EventMapTable->removeRow(selectItem->row());
    }
    ui->EventMapTable->setRowCount(preCnt - delCnt);
}

void EventMapForm::slot_EventTreeStartDrag(Qt::DropActions)
{
//    IDE_TRACE_STR("EventTreeStartDrag");
    QTreeWidgetItem* item = ui->EventTree->currentItem();
    if(GetEventType(item->toolTip(0)) == EventInvalid)
        return;

    QPixmap pixmap = item->icon(0).pixmap(50,50);
    QString eventInfo = GetEventInfo(ui->EventPage->currentWidget());
    QStringList paraList = eventInfo.split("/", QString::SkipEmptyParts);
    if(paraList.isEmpty())
        return;
    QString text = paraList.at(paraList.count() - 1);
    QString toolTip = item->toolTip(0);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream<<item->icon(0)<<text<<toolTip;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("Event", itemData);

    QDrag *drag = new QDrag(ui->EventTree);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
    drag->setMimeData(mimeData);
    drag->exec(Qt::MoveAction);
}
/**
//>@EventMapDragAndDrop*****************************************************
*/
void EventMapForm::slot_EventMapTableDragEnter(QDragEnterEvent* event)
{
//    IDE_TRACE_STR("EventMapTableDragEnter");
    event->accept();
}

void EventMapForm::slot_EventMapTableDrop(QDropEvent *event)
{
//    IDE_TRACE_STR("EventMapTableDrop");
    if(event->mimeData()->hasFormat("Event"))
    {
        QByteArray itemData = event->mimeData()->data("Event");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        QIcon icon;
        QString text;
        QString toolTip;
        dataStream>>icon>>text>>toolTip;
        if(AddEventToMapForm(icon, text, toolTip, event->pos()))
            event->accept();
        else
            event->ignore();
    }
    else if(event->mimeData()->hasFormat("Action"))
    {
        QByteArray itemData = event->mimeData()->data("Action");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        QIcon icon;
        QString text;
        QString toolTip;
        dataStream>>icon>>text>>toolTip;
        if(AddActionToMapForm(icon, text, toolTip, event->pos()))
            event->accept();
        else
            event->ignore();
    }
}

void EventMapForm::slot_EventMapTableStartDrag(Qt::DropActions actions)
{
    ui->EventMapTable->UpStartDrag(actions);
//    IDE_TRACE_STR("EventMapTableStartDrag");
}
/**
//>@ActionTreeDragAndDrop*****************************************************
*/
void EventMapForm::slot_ActionTreeDragEnter(QDragEnterEvent* event)
{
//    IDE_TRACE_STR("ActionTreeDragEnter");
    event->accept();
}

void EventMapForm::slot_ActionTreeDrop(QDropEvent* event)
{
//    IDE_TRACE_STR("ActionTreeDrop");
    event->accept();
    QList<QTableWidgetItem*> itemList = ui->EventMapTable->selectedItems();
    for(int i=0;i<itemList.count();i++)
    {
        QTableWidgetItem* selectItem = itemList.at(i);
        if(selectItem->column() == 0)
            continue;
        ui->EventMapTable->takeItem(selectItem->row(), selectItem->column());
    }
}

void EventMapForm::slot_ActionTreeStartDrag(Qt::DropActions)
{
//    IDE_TRACE_STR("ActionTreeStartDrag");
    QTreeWidgetItem* item = ui->ActionTree->currentItem();

    QPixmap pixmap = item->icon(0).pixmap(75,75);
    QString text = item->toolTip(0);
    QString toolTip = item->text(1);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream<<item->icon(0)<<text<<toolTip;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("Action", itemData);

    QDrag *drag = new QDrag(ui->ActionTree);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
    drag->setMimeData(mimeData);
    drag->exec(Qt::MoveAction);
}

void EventMapForm::slot_MousePressEvent(QMouseEvent* event)
{
    ui->ActionTree->downMousePressEvent(event);
    QTreeWidgetItem* item = ui->ActionTree->currentItem();
    if(item)
    {
        QString toolTip = item->toolTip(0);
        QStringList paraList = toolTip.split("-", QString::SkipEmptyParts);
        if(paraList.count() == 2)
        {
            QString para0 = paraList.at(0);
            QString para1 = paraList.at(1);
            if(para1.contains("Next")||para1.contains("Pre"))
            {
                switch(event->button())
                {
                    case Qt::LeftButton:
                        item->setToolTip(0, QString("%1-Next").arg(para0));
                        break;
                    case Qt::RightButton:
                        item->setToolTip(0, QString("%1-Pre").arg(para0));
                        break;
                }
            }
        }
    }
}
/**
//>@UICtrl*****************************************************
*/
void EventMapForm::on_EventTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(current == 0)
        return;
    ui->EventTree->setExpanded(ui->EventTree->currentIndex(), true);
    ui->EventPage->setCurrentIndex(GetEventType(current->toolTip(0)));
}

void EventMapForm::on_EventMapTable_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    if(current == 0)
        return;
    ShowPixmap(current->toolTip(), current->icon());
}

void EventMapForm::on_ActionTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(current == 0)
        return;
    ui->ActionTree->setExpanded(ui->ActionTree->currentIndex(), true);
    ShowPixmap(current->text(1), current->icon(0));
}

void EventMapForm::on_Timer_TriggerTime_Para1_Num_valueChanged(int value)
{
    ui->TimerVauleLabel->setText(QString("%1").arg(value));
}

void EventMapForm::on_SelectUIBtn_clicked()
{
    static QString sSelectPath = QDir::currentPath();
    QString UIPath = QFileDialog::getOpenFileName
            (this, tr("Open File"),
             sSelectPath,
             tr("UI (*.ui)"));
    SycncActionTreeFromTheme(UIPath);
    sSelectPath = getParentDirectory(UIPath);
}

void EventMapForm::on_BtnAddRow_Schadule_clicked()
{
    ui->EventMapTable->setRowCount(ui->EventMapTable->rowCount()+1);
}

void EventMapForm::on_BtnDelRow_Schadule_clicked()
{
    ui->EventMapTable->setRowCount(ui->EventMapTable->rowCount()-1);
}

void EventMapForm::on_BtnRefresh_Schadule_clicked()
{
    SycncEventTableFromMapAndTheme(mMapPAth);
}

void EventMapForm::on_BtnSave_Schadule_clicked()
{
    SycncMapFromUI();
}


void EventMapForm::on_BtnAscending_Schadule_clicked()
{
    ui->EventMapTable->sortByColumn(0, Qt::AscendingOrder);
}

void EventMapForm::on_BtnDescendingOrder_Schadule_clicked()
{
    ui->EventMapTable->sortByColumn(0, Qt::DescendingOrder);
}

void EventMapForm::on_EventTree_activated(const QModelIndex &index)
{

}
