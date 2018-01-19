#include "diagramsetting.h"
#include "ui_diagramsetting.h"

EffectSetting::EffectSetting(QWidget *parent, QString pControl, QString pOldControl) :
    QDialog(parent)
{
    m_MainLayout = new QVBoxLayout;
    this->setLayout(m_MainLayout);

    this->setWindowTitle(tr("EffectParameter Setting"));

    //>@
    m_StringList = SplitString(pControl, ";");
    if(m_StringList.count() <= 0)
    {
        deleteLater();
        return;
    }

    QStringList tmpParaList = SplitString(pOldControl, ";");
    QString initValue;
    for(int i=0;i<m_StringList.count();i++)
    {
        QStringList tmpList = SplitString(m_StringList.at(i), ":");
        if(tmpList.count() <= 0)
            continue;
        for(int j=0;j<tmpParaList.count();j++)
        {
            QString tmpString = tmpParaList.at(j);
            if(tmpString.contains(tmpList.at(0)))
            {
                QStringList tmpLastList = SplitString(tmpString, ":");
                if(tmpLastList.count()==2)
                    initValue = tmpLastList.at(1);
                break;
            }
        }
        CreateUi(this, m_MainLayout, tmpList, initValue);
    }

    //>@
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    m_ButtonBox = new QDialogButtonBox(this);
    m_ButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    connect(m_ButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_ButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    tmpHBoxLayout->addWidget(m_ButtonBox);
    m_MainLayout->addItem(tmpHBoxLayout);
}

void EffectSetting::accept()
{
    m_EffectPara.clear();
    for(int i=0;i<m_StringList.count();i++)
    {
        QString EffectString = m_StringList.at(i);
        QStringList tmpList = SplitString(EffectString, ":");
        QString tmpString = tmpList.at(0);
        if(tmpString.isEmpty())
            continue;
        QString tmpParaString;
        tmpParaString.clear();
        if(tmpString.compare(PARA_DIRECTION, Qt::CaseInsensitive) == 0 ||
            tmpString.compare(PARA_ORIENTATION, Qt::CaseInsensitive) == 0 ||
            tmpString.compare(PARA_CURVESHAPE, Qt::CaseInsensitive) == 0 ||
            tmpString.compare(PARA_STYLE, Qt::CaseInsensitive) == 0 ||
            tmpString.compare(PARA_PATTERN, Qt::CaseInsensitive) == 0 ||
            tmpString.compare(PARA_ALIGN, Qt::CaseInsensitive) == 0)
        {
            QComboBox *tmpComboBox = findChild<QComboBox *>(tmpString);
            if(tmpComboBox->currentText().isEmpty()==false)
                tmpParaString = QString("%1:%2").arg(tmpString).arg(tmpComboBox->currentText());
        }
        else if(tmpString.compare(PARA_AUTOPLAY, Qt::CaseInsensitive) == 0)  //>@自动运行
        {
            QString para;
            QCheckBox *tmpCheckBox = findChild<QCheckBox *>(tmpString);
            Qt::CheckState tmpCheck = tmpCheckBox->checkState();
            if(tmpCheck == Qt::Unchecked)
                para = "N";
            else if(tmpCheck == Qt::Checked)
                para = "Y";
            tmpParaString = QString("%1:%2").arg(tmpString).arg(para);
        }
        else        //>@默认的均为数字编辑
        {
            QSpinBox *tmpSpinBox = findChild<QSpinBox *>(tmpString);
            if(tmpSpinBox && tmpSpinBox->text().isEmpty()==false)
                tmpParaString = QString("%1:%2").arg(tmpString).arg(tmpSpinBox->text());
        }

        if(tmpParaString.isEmpty()==false)
        {
            m_EffectPara.append(tmpParaString);
            if(i < m_StringList.count()-1)
            {
                m_EffectPara.append(";");
            }
        }
    }
    QDialog::accept();
}

void EffectSetting::reject()
{
    QDialog::reject();
}

bool EffectSetting::CreateUi(QWidget *pParentWidget, QLayout *pLayout, QStringList pUiPara, QString pInitValue)
{
    if(pParentWidget == 0 || pLayout == 0 || pUiPara.count() <= 0)
    {
        return false;
    }

    QString tmpString = pUiPara.at(0);
    if(tmpString.isEmpty())
        return false;
    if(tmpString.compare(PARA_DIRECTION, Qt::CaseInsensitive) == 0 ||
       tmpString.compare(PARA_ORIENTATION, Qt::CaseInsensitive) == 0 ||
       tmpString.compare(PARA_CURVESHAPE, Qt::CaseInsensitive) == 0 ||
       tmpString.compare(PARA_STYLE, Qt::CaseInsensitive) == 0 ||
       tmpString.compare(PARA_PATTERN, Qt::CaseInsensitive) == 0 ||
       tmpString.compare(PARA_ALIGN, Qt::CaseInsensitive) == 0)
    {
        //>@Combobox
        QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
        QLabel *tmpLabel = new QLabel(tmpString, pParentWidget);
        tmpLabel->setMinimumWidth(100);
        tmpLabel->setMaximumWidth(100);

        QComboBox *tmpComboBox = new QComboBox(pParentWidget);
        tmpComboBox->setObjectName(tmpString);
        tmpComboBox->setMinimumWidth(200);
        tmpComboBox->setMaximumWidth(200);

        tmpString = pUiPara.at(1);
        if(tmpString.isEmpty())
            return false;
        QStringList tmpTextLsit = SplitString(tmpString, "/");
        tmpComboBox->addItems(tmpTextLsit);

        if(pInitValue.isEmpty() == false)
        {
            for(int i=0;i<tmpTextLsit.count();i++)
            {
                if(pInitValue == tmpTextLsit.at(i))
                    tmpComboBox->setCurrentIndex(i);
            }
        }

        tmpHBoxLayout->addWidget(tmpLabel);
        tmpHBoxLayout->addWidget(tmpComboBox);
        pLayout->addItem(tmpHBoxLayout);
    }
    else if(tmpString.compare(PARA_AUTOPLAY, Qt::CaseInsensitive) == 0)  //>@自动运行
    {
        //>@checkbox
        QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
        QCheckBox *tmpCheckBox = new QCheckBox(tmpString, pParentWidget);
        tmpCheckBox->setObjectName(tmpString);
        tmpCheckBox->setMinimumWidth(200);

        if(pInitValue.isEmpty() == false)
        {
            if(pInitValue == QString("N"))
                tmpCheckBox->setCheckState(Qt::Unchecked);
            else if(pInitValue == QString("Y"))
                tmpCheckBox->setCheckState(Qt::Checked);
        }

        tmpHBoxLayout->addWidget(tmpCheckBox);
        pLayout->addItem(tmpHBoxLayout);
    }
    else
    {
        //>@SpinBox
        QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
        QLabel *tmpLabel = new QLabel(tmpString, pParentWidget);
        tmpLabel->setMinimumWidth(100);
        tmpLabel->setMaximumWidth(100);

        QSpinBox *tmpSpinBox = new QSpinBox(pParentWidget);
        tmpSpinBox->setObjectName(tmpString);
        tmpSpinBox->setMinimum(0);
        tmpSpinBox->setMaximum(99999);
        tmpSpinBox->setMinimumWidth(200);
        tmpSpinBox->setMaximumWidth(200);

        if(pInitValue.isEmpty() == false)
        {
            tmpSpinBox->setValue(pInitValue.toInt());
        }
        else
        {
            tmpSpinBox->setValue(100);
        }

        tmpHBoxLayout->addWidget(tmpLabel);
        tmpHBoxLayout->addWidget(tmpSpinBox);
        pLayout->addItem(tmpHBoxLayout);
    }
    return true;
}

DiagramSetting::DiagramSetting(uiManager *pManager, COM_INFO *pComInfo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiagramSetting)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Menu/rc/Menu/setting.png"));
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    m_CurState = 0;
    m_CurSelectItem = 0;
    m_IsValid = false;
    m_CurStateIndex = -1;   //>@状态机组合框当前显示的项序号
    m_CurEffectIndex = -1;  //>@特效组合框当前显示的项序号

    m_Manager = pManager;
    m_ComInfo = pComInfo;

    if(m_Manager)
        m_CurRcDir = m_Manager->m_FileDir;        //>@当前打开的文件所在的目录，默认是ui文件所在目录

    ui->EffectEditTree->clear();
    ui->EffectEditTree->setColumnWidth(0, 150);

    if(m_ComInfo == 0)
    {
        IDE_TRACE();
        m_IsValid = false;
        return;
    }
    m_ComCategory = GlobalPara.m_ComCategoryGroup.value(m_ComInfo->ComPath);
    if(m_ComCategory == 0)
    {
        IDE_TRACE();
        m_IsValid = false;
        return;
    }
    setWindowTitle(tr("%1 Settings").arg(m_ComInfo->ComPath));
    m_IsValid = true;

    InitUI();
}

DiagramSetting::~DiagramSetting()
{
    delete ui;
}

void DiagramSetting::InitUI()
{
    if(m_IsValid == false)
        return;
    ui->StateMachineType->clear();
    for(int i=0;i<m_ComCategory->StateMachines.count();i++)
    {
        STATEMACHINE* tmpState = m_ComCategory->StateMachines.at(i);
        ui->StateMachineType->addItem(tmpState->StateName);
    }
    ui->StateMachineType->setCurrentIndex(0);

    ui->EditRcPath->clear();
    ui->EffectPara->clear();

    UpdateTree(m_ComInfo);
}

void DiagramSetting::DelTreeAllItem(QTreeWidgetItem *parentItem)
{
    QList<QTreeWidgetItem*> ItemList = parentItem->takeChildren();
    QTreeWidgetItem* tmpItem;
    for(int i=0;i<ItemList.count();i++)
    {
        tmpItem = ItemList.at(i);
        parentItem->removeChild(tmpItem);
    }
}

//>@返回父item
QTreeWidgetItem *DiagramSetting::DelTreeItem(QTreeWidgetItem *pItem)
{
    QTreeWidgetItem* tmpParentItem = pItem->parent();
    if(tmpParentItem == 0)
        return 0;
    tmpParentItem->removeChild(pItem);
    return tmpParentItem;
}

//>@返回添加的Item
QTreeWidgetItem *DiagramSetting::AddTreeItem(QTreeWidgetItem *parentItem, QStringList contentList, bool Editable)
{
    static int i = 0;
    QTreeWidgetItem *pNewItem = new QTreeWidgetItem(parentItem, contentList);
    pNewItem->setTextAlignment(0, Qt::AlignLeft || Qt::AlignVCenter);
    pNewItem->setTextAlignment(1, Qt::AlignLeft || Qt::AlignVCenter);
    pNewItem->setBackgroundColor(0, (i%2)?QColor(254,235,208):QColor(255,250,179));
    pNewItem->setBackgroundColor(1, (i%2)?QColor(254,235,208):QColor(255,250,179));
    pNewItem->setFont(0, QFont());
    if(Editable)
        pNewItem->setFlags(Qt::ItemIsEditable |Qt::ItemIsEnabled);
    parentItem->addChild(pNewItem);
    i++;
    return pNewItem;
}

//>@添加某个状态特效资源
QTreeWidgetItem *DiagramSetting::AddTreeItem(QTreeWidgetItem *parentItem, STATE_INFO* pState)
{
    if(pState ==0 || pState->StateName.isEmpty())
        return 0;
    QTreeWidgetItem *pNewItem = AddTreeItem(parentItem, QStringList()<<pState->StateName, false);
    QTreeWidgetItem *pNewSubItem = AddTreeItem(pNewItem, QStringList()<<QString("EffectType")<<pState->Effect.EffectType, false);
    for(int i=0;i<pState->Effect.RcFiles.count();i++)
    {
        RESOURCE_INFO tmpRc = pState->Effect.RcFiles.at(i);
        AddTreeItem(pNewSubItem, QStringList()<<tmpRc.RcName<<tmpRc.RcFile, false);
    }
    return pNewItem;
}

void DiagramSetting::UpdateTree(COM_INFO *pComInfo)
{
    ui->EffectEditTree->clear();
    m_RootTreeItem = new QTreeWidgetItem(ui->EffectEditTree, QStringList(QString("StateMachine")));
    m_RootTreeItem->setBackground(0, QColor(185,185,185));
    m_RootTreeItem->setBackground(1, QColor(185,185,185));
    for(int i=0;i<pComInfo->StateMachine.count();i++)
    {
        STATE_INFO *tmpState = pComInfo->StateMachine.at(i);
        AddTreeItem(m_RootTreeItem, tmpState);
    }
    ui->EffectEditTree->expandAll();
}

void DiagramSetting::on_StateMachineType_currentIndexChanged(const QString &arg1)
{
    for(int i=0;i<m_ComCategory->StateMachines.count();i++)
    {
        STATEMACHINE* tmpState = m_ComCategory->StateMachines.at(i);
        if(arg1 == tmpState->StateName)
        {
            //>@保存当前选中的状态机下标
            m_CurStateIndex = i;
            m_CurState = tmpState;
            ui->EffectType->clear();
            ui->RcName->clear();
            for(int j=0;j<tmpState->Effects.count();j++)
            {
                EFFECT tmpEffect = tmpState->Effects.at(j);
                ui->EffectType->addItem(tmpEffect.EffectName);
            }
            ui->EffectType->setCurrentIndex(0);
            break;
        }
    }
}

void DiagramSetting::on_EffectType_currentIndexChanged(const QString &arg1)
{
    if(m_CurState == 0)
        return;
    for(int j=0;j<m_CurState->Effects.count();j++)
    {
        EFFECT tmpEffect = m_CurState->Effects.at(j);
        if(tmpEffect.EffectName == arg1)
        {
            m_CurEffectPara = tmpEffect.EffectPara;
            m_CurEffectIndex = j;
            ui->RcName->clear();
            for(int k=0;k<tmpEffect.Resources.count();k++)
            {
                RESOURCE tmpRc = tmpEffect.Resources.at(k);
                ui->RcName->addItem(tmpRc.RcName);
                if(tmpRc.RcName == QString("Invalid"))
                {
                    ui->EditRcPath->setText("*");
                }
            }
            SetRcNameCurrentIndex(0);
            break;
        }
    }
}

void DiagramSetting::on_RcName_currentIndexChanged(int index)
{
    if(m_CurEffectIndex < m_CurState->Effects.count() && m_CurEffectIndex>=0)
    {
        EFFECT tmpEffect = m_CurState->Effects.at(m_CurEffectIndex);
        if(tmpEffect.EffectName != ui->EffectType->currentText())
        {
            return;
        }
        if(index < ui->RcName->count() && index >= 0)
        {
            QString tmpString = ui->RcName->itemText(index);
            if(index < tmpEffect.Resources.count() && index >= 0)
            {
                QString tmpRcName = tmpEffect.Resources.at(index).RcName;
                QString tmpNickName = tmpEffect.Resources.at(index).NickName;
                if(tmpString == tmpRcName)
                {
                    ui->NickName->setText(tmpNickName);
                }
            }
        }
    }
}

void DiagramSetting::SetRcNameCurrentIndex(int pIndex)
{
    ui->RcName->setCurrentIndex(pIndex);
}

void DiagramSetting::on_BtnRcBrowser_clicked()
{
    //获取当前选中的资源,然后设置资源类型
    if(m_CurState == 0)
        return;
    for(int j=0;j<m_CurState->Effects.count();j++)
    {
        EFFECT tmpEffect = m_CurState->Effects.at(j);
        if(tmpEffect.EffectName == ui->EffectType->currentText())
        {
            for(int k=0;k<tmpEffect.Resources.count();k++)
            {
                RESOURCE tmpRc = tmpEffect.Resources.at(k);
                if(tmpRc.RcName == ui->RcName->currentText())
                {
                    int i;
                    QString tmpString("Resources (*.");
                    for(i=0;i<tmpRc.RcTypes.count()-1;i++)
                    {
                        tmpString.append(tmpRc.RcTypes.at(i));
                        tmpString.append(QString(" *."));
                    }
                    tmpString.append(tmpRc.RcTypes.at(i));
                    tmpString.append(")");
                    QString fileName = QFileDialog::getOpenFileName(this,
                                                                    tr("Select File"),
                                                                    m_CurRcDir,
                                                                    tmpString);
                    if (fileName.isEmpty())
                        return;
                    m_CurRcDir = getFileDirectory(fileName);
                    ui->EditRcPath->setText(fileName);
                    return;
                }
            }
        }
    }
}

//>@资源名组合框自动增1
void DiagramSetting::updateRcNameCombo()
{
    int index = ui->RcName->currentIndex();
    if(index + 1 < ui->RcName->count())
        SetRcNameCurrentIndex(index+1);
}

//>@只有在增加资源时，包括修改某一个资源文件路径（如果增加的文件名相同，但路径不行，则仅更改路径）以及新建某个资源，资源名组合框才会自加1。
void DiagramSetting::on_BtnAdd_clicked()
{
    QString StateMachineString, EffectTypeString, RcNameString, RcPathString;
    StateMachineString = ui->StateMachineType->currentText();
    //>@检查状态机是否选择
    if(StateMachineString.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Add Error"),
                             tr("Please select a StateMachine!"),
                             QMessageBox::Close);
        return;
    }
    else
    {
        //>@检查特效是否选择
        EffectTypeString = ui->EffectType->currentText();
        if(EffectTypeString.isEmpty())
        {
            QMessageBox::warning(this,
                                 tr("Add Error"),
                                 tr("Please select a Effect!"),
                                 QMessageBox::Close);
            return;
        }
        else
        {
            //>@检查资源名是否选择
            RcNameString = ui->RcName->currentText();
            if(RcNameString.isEmpty())
            {
                QMessageBox::warning(this,
                                     tr("Add Error"),
                                     tr("Please select a resource name!"),
                                     QMessageBox::Close);
                return;
            }
            else
            {
                RcPathString = ui->EditRcPath->text();
                if(RcPathString.isEmpty())
                {
                    QMessageBox::warning(this,
                                         tr("Add Error"),
                                         tr("Please select a resource file!"),
                                         QMessageBox::Close);
                    return;
                }
                else
                {
                    //>@执行添加，首先修改COM,然后通过COM更新Tree
                    //>@查看是否在当前状态机下修改，如果是，则修改，否则创建。
                    for(int i=0;i<m_ComInfo->StateMachine.count();i++)
                    {
                        STATE_INFO* tmpState = m_ComInfo->StateMachine.at(i);
                        if(tmpState->StateName == StateMachineString)
                        {
                            //>@一个状态机只有一种特效，此处为修改特效
                            if(tmpState->Effect.EffectType != EffectTypeString)
                            {
                                tmpState->Effect.EffectPara = ui->EffectPara->text();//>@修改特效参数
                                tmpState->Effect.EffectType = EffectTypeString;
                                tmpState->Effect.RcFiles.clear(); //>@清空资源列表
                                RESOURCE_INFO tmpRcInfo;
                                tmpRcInfo.RcName = RcNameString;
                                tmpRcInfo.RcFile = RcPathString;
                                tmpState->Effect.RcFiles.append(tmpRcInfo);
                                UpdateTree(m_ComInfo);

                                //>@资源名组合框自动增1
                                updateRcNameCombo();
                                return;
                            }
                            else
                            {
                                //>@如果是在当前特效下，则只需查看资源文件中是否有相同的资源名，如果有，则替换，没有则创建
                                for(int j=0;j<tmpState->Effect.RcFiles.count();j++)
                                {
                                    RESOURCE_INFO tmpRcInfo = tmpState->Effect.RcFiles.at(j);
                                    //>@资源名相同且路径名不同则替换
                                    if(tmpRcInfo.RcName == RcNameString)
                                    {
                                        if(tmpRcInfo.RcFile != RcPathString)
                                        {
                                            tmpState->Effect.RcFiles.removeAt(j);
                                            RESOURCE_INFO RcInfo;
                                            RcInfo.RcName = RcNameString;
                                            RcInfo.RcFile = RcPathString;
                                            tmpState->Effect.RcFiles.append(RcInfo);
                                            UpdateTree(m_ComInfo);

                                            //>@资源名组合框自动增1
                                            updateRcNameCombo();
                                        }
                                        return;
                                    }
                                }
                                //>@如果当前特效中没有相同资源名的文件，则创建，同时资源名组合框自动增1
                                RESOURCE_INFO RcInfo;
                                RcInfo.RcName = RcNameString;
                                RcInfo.RcFile = RcPathString;
                                tmpState->Effect.RcFiles.append(RcInfo);
                                UpdateTree(m_ComInfo);

                                //>@资源名组合框自动增1
                                updateRcNameCombo();
                                return;
                            }
                            return;
                        }
                    }
                    //>@添加一个状态机
                    STATE_INFO* State = new STATE_INFO;
                    State->StateName = StateMachineString;
                    State->Effect.EffectPara = ui->EffectPara->text();//>@修改特效参数
                    State->Effect.EffectType = EffectTypeString;
                    RESOURCE_INFO tmpRcInfo;
                    tmpRcInfo.RcName = RcNameString;
                    tmpRcInfo.RcFile = RcPathString;
                    State->Effect.RcFiles.append(tmpRcInfo);
                    m_ComInfo->StateMachine.append(State);
                    UpdateTree(m_ComInfo);

                    //>@资源名组合框自动增1
                    updateRcNameCombo();
                }
            }
        }
    }
}

void DiagramSetting::on_BtnModify_clicked()
{
    if(m_CurSelectItem == 0)
    {
        QMessageBox::warning(this,
                             tr("Modify Error"),
                             tr("Please select resource \nin the ResourcesTree \non the left!"),
                             QMessageBox::Close);
        return;
    }
    QTreeWidgetItem* tmpParentItem = m_CurSelectItem->parent();
    if(tmpParentItem)
    {
        //>@同步ComInfo操作
        QString curName = m_CurSelectItem->text(0);
        QString curRc = m_CurSelectItem->text(1);
        QString parentName = tmpParentItem->text(0);
        QString parentRc = tmpParentItem->text(1);
        if(curRc.isEmpty()) //>@当前选中的是状态行，则退出
        {
            m_CurSelectItem = 0;
            return;
        }
        else if(curName == QString("EffectType") || parentRc.isEmpty()) //>@当前选中的是特效行，可以更改特效参数
        {
            //>@在ComInfo中查找到待修改的特效
            for(int i=0;i<m_ComInfo->StateMachine.count();i++)
            {
                STATE_INFO* tmpState = m_ComInfo->StateMachine.at(i);
                if(tmpState->Effect.EffectType == curRc)
                {
                    QString tmpEffectPara = ui->EffectPara->text();
                    //>@查看是否和已有参数相同
                    if(tmpState->Effect.EffectPara != tmpEffectPara)
                    {
                        //>@询问是否修改特效参数
                        int ret = QMessageBox::warning(this,
                                             tr("Effect Parameter"),
                                             tr("Do you want to modify parameter[%1] to parameter[%2]")
                                             .arg(tmpState->Effect.EffectPara).arg(tmpEffectPara),
                                             QMessageBox::Yes | QMessageBox::No,
                                             QMessageBox::No);
                        if(ret == QMessageBox::Yes)
                        {
                            tmpState->Effect.EffectPara = tmpEffectPara;
                        }
                    }
                    break;
                }
            }
        }
        else if(parentName == QString("EffectType")) //>@当前选中的是资源行，仅可以修改资源文件路径
        {
            for(int i=0;i<m_ComInfo->StateMachine.count();i++)
            {
                STATE_INFO* tmpState = m_ComInfo->StateMachine.at(i);
                for(int j=0;j<tmpState->Effect.RcFiles.count();j++)
                {
                    RESOURCE_INFO tmpRc = tmpState->Effect.RcFiles.at(j);
                    if(tmpRc.RcFile == curRc && tmpRc.RcName == curName)
                    {
                        //>@查找到ComInfo中的资源项
                        QString tmpString = ui->EditRcPath->text();
                        if(tmpRc.RcFile != tmpString)
                        {
                            RESOURCE_INFO tmpNewRc;
                            tmpNewRc.RcName = tmpRc.RcName;
                            tmpNewRc.RcFile = tmpString;
                            tmpState->Effect.RcFiles.removeAt(j);
                            tmpState->Effect.RcFiles.append(tmpNewRc);
                        }
                        break;
                    }
                }
            }
        }
        else
        {
            m_CurSelectItem = 0;
            return;
        }
        UpdateTree(m_ComInfo);
    }
    m_CurSelectItem = 0;
}

void DiagramSetting::on_BtnDelete_clicked()
{
    if(m_CurSelectItem == 0)
    {
        QMessageBox::warning(this,
                             tr("Delete Error"),
                             tr("Please select resource \nin the ResourcesTree \non the left!"),
                             QMessageBox::Close);
        return;
    }
    QTreeWidgetItem* tmpParentItem = m_CurSelectItem->parent();
    if(tmpParentItem)
    {
        //tmpParentItem->removeChild(m_CurSelectItem);
        //>@同步ComInfo操作
        QString curName = m_CurSelectItem->text(0);
        QString curRc = m_CurSelectItem->text(1);
        QString parentName = tmpParentItem->text(0);
        QString parentRc = tmpParentItem->text(1);
        if(curRc.isEmpty()) //>@当前选中的是状态行
        {
            for(int i=0;i<m_ComInfo->StateMachine.count();i++)
            {
                STATE_INFO* tmpState = m_ComInfo->StateMachine.at(i);

                if(tmpState->StateName == curName)
                {
                    ui->StateMachineType->clear();
                    ui->StateMachineType->addItem(tmpState->StateName);

                    m_ComInfo->StateMachine.removeAt(i);
                    break;
                }
            }
        }
        else if(curName == QString("EffectType") || parentRc.isEmpty()) //>@当前选中的是特效行，会将状态机也删除，因为状态机仅有一个特效，那么此时状态机无用了
        {
            for(int i=0;i<m_ComInfo->StateMachine.count();i++)
            {
                STATE_INFO* tmpState = m_ComInfo->StateMachine.at(i);
                if(tmpState->Effect.EffectType == curRc)
                {
                    ui->StateMachineType->clear();
                    ui->StateMachineType->addItem(parentName);
                    for(int j=0;j<ui->EffectType->count();j++)
                    {
                        QString tmpString = ui->EffectType->itemText(j);
                        if(tmpString == curRc)
                        {
                            ui->EffectType->setCurrentIndex(j);
                            break;
                        }
                    }

                    m_ComInfo->StateMachine.removeAt(i);
                    break;
                }
            }
        }
        else if(parentName == QString("EffectType")) //>@当前选中的是资源行
        {
            for(int i=0;i<m_ComInfo->StateMachine.count();i++)
            {
                STATE_INFO* tmpState = m_ComInfo->StateMachine.at(i);
                for(int j=0;j<tmpState->Effect.RcFiles.count();j++)
                {
                    RESOURCE_INFO tmpRc = tmpState->Effect.RcFiles.at(j);
                    if(tmpRc.RcFile == curRc && tmpRc.RcName == curName)
                    {
                        tmpState->Effect.RcFiles.removeAt(j);
                    }
                }
            }
        }
        else
        {
            m_CurSelectItem = 0;
            return;
        }
        on_BtnRefresh_clicked();
    }
    m_CurSelectItem = 0;
}

void DiagramSetting::on_BtnClear_clicked()
{
    m_ComInfo->StateMachine.clear();
    on_BtnRefresh_clicked();
}

void DiagramSetting::on_EffectEditTree_itemSelectionChanged()
{
    QList<QTreeWidgetItem*> tmpList = ui->EffectEditTree->selectedItems();
    if(tmpList.count() <= 0)
        return;
    m_CurSelectItem = tmpList.at(0);
    if(m_CurSelectItem == 0)
        return;
    QTreeWidgetItem* tmpParentItem = m_CurSelectItem->parent();
    if(tmpParentItem == 0)  //>@当前选中的是顶级项，此时将右下边的状态机恢复到默认即可
    {
        ui->StateMachineType->clear();
        for(int i=0;i<m_ComCategory->StateMachines.count();i++)
        {
            STATEMACHINE* tmpState = m_ComCategory->StateMachines.at(i);
            ui->StateMachineType->addItem(tmpState->StateName);
        }
        ui->StateMachineType->setCurrentIndex(0);
    }
    else
    {
        //tmpParentItem->removeChild(m_CurSelectItem);
        //>@同步ComInfo操作
        QString curName = m_CurSelectItem->text(0);
        QString curRc = m_CurSelectItem->text(1);
        QString parentName = tmpParentItem->text(0);
        QString parentRc = tmpParentItem->text(1);
        if(curRc.isEmpty()) //>@当前选中的是状态行
        {
            for(int i=0;i<m_ComInfo->StateMachine.count();i++)
            {
                STATE_INFO* tmpState = m_ComInfo->StateMachine.at(i);
                if(!tmpState)
                    continue;
                if(tmpState->StateName == curName)
                {
                    for(int j=0;j<ui->StateMachineType->count();j++)
                    {
                        QString tmpString = ui->StateMachineType->itemText(j);
                        if(tmpString == curName)
                        {
                            ui->StateMachineType->setCurrentIndex(j);
                            break;
                        }
                    }
                    break;
                }
            }
        }
        else if(curName == QString("EffectType") || parentRc.isEmpty()) //>@当前选中的是特效行，会将状态机也删除，因为状态机仅有一个特效，那么此时状态机无用了
        {
            for(int i=0;i<m_ComInfo->StateMachine.count();i++)
            {
                STATE_INFO* tmpState = m_ComInfo->StateMachine.at(i);
                if(tmpState->Effect.EffectType == curRc)
                {
                    for(int j=0;j<ui->StateMachineType->count();j++)
                    {
                        QString tmpString = ui->StateMachineType->itemText(j);
                        if(tmpString == parentName)
                        {
                            ui->StateMachineType->setCurrentIndex(j);
                            break;
                        }
                    }
                    ui->EffectPara->update();
                    ui->EffectPara->setText(tmpState->Effect.EffectPara);
                    ui->EffectType->update();
                    for(int j=0;j<ui->EffectType->count();j++)
                    {
                        QString tmpString = ui->EffectType->itemText(j);
                        if(tmpString == curRc)
                        {
                            ui->EffectType->setCurrentIndex(j);
                            break;
                        }
                    }
                    break;
                }
            }
        }
        else if(parentName == QString("EffectType")) //>@当前选中的是资源行
        {
            for(int i=0;i<m_ComInfo->StateMachine.count();i++)
            {
                STATE_INFO* tmpState = m_ComInfo->StateMachine.at(i);
                for(int j=0;j<tmpState->Effect.RcFiles.count();j++)
                {
                    RESOURCE_INFO tmpRc = tmpState->Effect.RcFiles.at(j);
                    if(tmpRc.RcFile == curRc && tmpRc.RcName == curName)
                    {
                        for(int j=0;j<ui->StateMachineType->count();j++)
                        {
                            QString tmpString = ui->StateMachineType->itemText(j);
                            if(tmpString == tmpParentItem->parent()->text(0))
                            {
                                ui->StateMachineType->setCurrentIndex(j);
                                break;
                            }
                        }

                        ui->EffectType->update();
                        for(int j=0;j<ui->EffectType->count();j++)
                        {
                            QString tmpString = ui->EffectType->itemText(j);
                            if(tmpString == parentRc)
                            {
                                ui->EffectType->setCurrentIndex(j);
                                break;
                            }
                        }
                        ui->RcName->update();
                        for(int j=0;j<ui->RcName->count();j++)
                        {
                            QString tmpString = ui->RcName->itemText(j);
                            if(tmpString == curName)
                            {
                                SetRcNameCurrentIndex(j);
                                ui->EditRcPath->setText(curRc);
                                break;
                            }
                        }
                    }
                }
            }
            //>@预览此资源
            PreviewRC(curRc);
        }
    }
}

void DiagramSetting::PreviewRC(QString pFile)
{
    //>@预览此资源
    FILEPATH_TYPE tmpFileType = RegulatRcFile(pFile, m_Manager);
    if(tmpFileType != PATH_TYPENONE)
    {
        RC_TYPE tmpRcType = getRcType(pFile);
        switch(tmpRcType)
        {
            case RC_BMP:
            case RC_JPG:
            case RC_PNG:
            {
                ui->LabelPreview->ShowPixmap(pFile);
                break;
            }
            case RC_GIF:
            {
                ui->LabelPreview->ShowGIF(pFile);
                break;
            }
            case RC_SVG:
            {
                ui->LabelPreview->ShowSVG(pFile);
                break;
            }
            case RC_QML:
            {
                ui->LabelPreview->ShowQML(pFile);
                break;
            }
            case RC_TXT:
            {
                break;
            }
            case RC_MP3:
            case RC_WAV:
            case RC_MP4:
            case RC_AVI:
            case RC_RMVB:
            case RC_FLV:
            {
                ui->LabelPreview->PlayMedia(pFile);
                break;
            }
            default:
                break;
        }
    }
}

void DiagramSetting::on_BtnEffectParaEdit_clicked()
{
    m_CurStateIndex = ui->StateMachineType->currentIndex();
    m_CurEffectIndex = ui->EffectType->currentIndex();
    STATEMACHINE *tmpState = m_ComCategory->StateMachines.at(m_CurStateIndex);
    if(!tmpState)
        return;
    EFFECT tmpEffect = tmpState->Effects.at(m_CurEffectIndex);
    if(tmpEffect.EffectName != ui->EffectType->currentText())
    {
        IDE_TRACE_STR(tmpEffect.EffectName);
        IDE_TRACE_STR(ui->EffectType->currentText());
        return;
    }
    m_CurEffectPara = tmpEffect.EffectPara;
    if(m_CurEffectPara.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("The current effect parameter is invalid,\nPlease correct!"),
                             QMessageBox::Close);
        return;
    }

    //>@增加从ui->EffectPara中获取已设置的值
    QString tmpLastPara = ui->EffectPara->text();

    EffectSetting myEffectSetting(0, m_CurEffectPara, tmpLastPara);
    int ret = myEffectSetting.exec();
    if(ret == QDialog::Accepted)
    {
        if(myEffectSetting.m_EffectPara.isEmpty()==false)
            ui->EffectPara->setText(myEffectSetting.m_EffectPara);
    }

    m_CurEffectPara.clear();
}

void DiagramSetting::on_BtnRefresh_clicked()
{
    InitUI();
}

