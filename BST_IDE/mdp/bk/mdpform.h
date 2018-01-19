#ifndef MDPFORM_H
#define MDPFORM_H

#include "global.h"
#include "upitem.h"
#include <QListWidget>
#include <QWidget>

namespace Ui {
class MDPForm;
}

class MDPForm : public QWidget
{
    Q_OBJECT
    enum ParaType{
        ParaInvalid=0,
        ParaBool,
        ParaNum,
        ParaStr,
        ParaEnum,
        ParaCheck
    };
public:
    explicit MDPForm(QString pPath, QWidget *parent = 0);
    ~MDPForm();
    void InitPara()
    {
        mMdpManager = 0;
        mMDPPath.clear();
        mThemeListIcon = QIcon(":/Menu/rc/Menu/grid.png");
    }

    void SycncUIFromMDP(QString pPath);
    void SycncMDPFromUI();
private:
    bool SearchOB(QObjectList& pOBList, QObject* pOB, int pLevel)
    {
        pLevel--;
        if(pLevel == -1)
            return true;
        QObjectList OBList = pOB->children();
        pOBList.append(OBList);
        for(int i=0;i<OBList.count();i++)
        {
            QObject* OB = OBList.at(i);
            if(!SearchOB(pOBList, OB, pLevel))
                return false;
        }
        return true;
    }
    ParaType GetParaType(QString pKey)
    {
        if(pKey.compare("Bool", Qt::CaseInsensitive) == 0)
            return ParaBool;
        else if(pKey.compare("Num", Qt::CaseInsensitive) == 0)
            return ParaNum;
        else if(pKey.compare("String", Qt::CaseInsensitive) == 0)
            return ParaStr;
        else if(pKey.compare("Enum", Qt::CaseInsensitive) == 0)
            return ParaEnum;
        else if(pKey.compare("Check", Qt::CaseInsensitive) == 0)
            return ParaCheck;
        else
            return ParaInvalid;
    }
//>@<<In
    bool PreMapToUI(QObjectList pOBList)
    {
        for(int i=0;i<pOBList.count();i++)
        {
            QObject* OB = pOBList.at(i);
            QString OBName = OB->objectName();
            QGroupBox* groupBox = qobject_cast<QGroupBox*>(OB);
            if(groupBox != 0)
            {
                if(groupBox->isCheckable())
                {
                    groupBox->setChecked(false);
                }
            }
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(OB);
            if(lineEdit != 0)
            {
                if(OBName.contains("Network", Qt::CaseInsensitive))
                    lineEdit->setInputMask("000.000.000.000; ");
            }
        }
        return true;
    }
    bool MapToGroupBox(QString pKey, QStringList pList, QString pVaule, QObject* pOB)
    {
        QGroupBox* groupBox = qobject_cast<QGroupBox*>(pOB);
        if(groupBox == 0)
            return false;
        if(groupBox->isCheckable())
        {
            QString OBName = groupBox->objectName();
            if(!pKey.contains(OBName, Qt::CaseInsensitive))
                return false;
            groupBox->setChecked(true);
        }
    }
    bool MapToLineEdit(QString pKey, QStringList pList, QString pVaule, QObject* pOB)
    {
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(pOB);
        if(lineEdit == 0)
            return false;
        QString OBName = lineEdit->objectName();
        if(!OBName.contains(pKey, Qt::CaseInsensitive))
            return false;
        QStringList listOBName = OBName.split("_", QString::SkipEmptyParts);
        if(listOBName.count()<5)
            return false;
        ParaType paraType = GetParaType(listOBName.at(3));
        switch(paraType)
        {
            case ParaStr:
            {
                lineEdit->setText(pVaule);
                return true;
            }
            default:return false;
        }
    }
    bool MapToComboBox(QString pKey, QStringList pList, QString pVaule, QObject* pOB)
    {
        QComboBox* comboBox = qobject_cast<QComboBox*>(pOB);
        if(comboBox == 0)
            return false;
        QString OBName = comboBox->objectName();
        if(!OBName.contains(pKey, Qt::CaseInsensitive))
            return false;
        QStringList listOBName = OBName.split("_", QString::SkipEmptyParts);
        if(listOBName.count()<5)
            return false;
        ParaType paraType = GetParaType(listOBName.at(3));
        switch(paraType)
        {
            case ParaEnum:
            {
                for(int i=0;i<comboBox->count();i++)
                {
                    QString text = comboBox->itemText(i);
                    if(text.compare(pVaule, Qt::CaseInsensitive) == 0)
                    {
                        comboBox->setCurrentIndex(i);
                        return true;
                    }
                }
                return false;
            }
            default:return false;
        }
    }
    bool MapToListWidget(QString pKey, QStringList pList, QString pVaule, QObject* pOB)
    {
        QListWidget* listWidget = qobject_cast<QListWidget*>(pOB);
        if(listWidget == 0)
            return false;
        QString OBName = listWidget->objectName();
        if(!OBName.contains(pKey, Qt::CaseInsensitive))
            return false;
        QStringList listOBName = OBName.split("_", QString::SkipEmptyParts);
        if(listOBName.count()<5)
            return false;
        ParaType paraType = GetParaType(listOBName.at(3));
        switch(paraType)
        {
            case ParaEnum:
            case ParaCheck:
            {
                if(listWidget->count() == 0)
                {
                    for(int i=0;i<pList.count();i++)
                    {
                        QListWidgetItem* item = new QListWidgetItem;
                        item->setCheckState(Qt::Unchecked);
                        item->setIcon(mThemeListIcon);
                        item->setText(pList.at(i));
                        listWidget->addItem(item);
                    }
                }
                for(int i=0;i<listWidget->count();i++)
                {
                    QListWidgetItem* item = listWidget->item(i);
                    if(item->flags() != Qt::ItemIsEnabled)
                        continue;
                    item->setCheckState(Qt::Unchecked);
                }
                QStringList listVaule = pVaule.split(";", QString::SkipEmptyParts);
                for(int j=0;j<listVaule.count();j++)
                {
                    QString vaule = listVaule.at(j);
                    for(int k=0;k<listWidget->count();k++)
                    {
                        QListWidgetItem* item = listWidget->item(k);
                        QString text = item->text();
                        if(!text.compare(vaule, Qt::CaseInsensitive))
                            item->setCheckState(Qt::Checked);
                    }
                }
                return true;
            }
            default:return false;
        }
    }
    bool MapToCheckBox(QString pKey, QStringList pList, QString pVaule, QObject* pOB)
    {
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(pOB);
        if(checkBox == 0)
            return false;
        QString OBName = checkBox->objectName();
        if(!OBName.contains(pKey, Qt::CaseInsensitive))
            return false;
        QStringList listOBName = OBName.split("_", QString::SkipEmptyParts);
        if(listOBName.count()<5)
            return false;
        ParaType paraType = GetParaType(listOBName.at(3));
        switch(paraType)
        {
            case ParaBool:
            {
                if(!pVaule.compare("True", Qt::CaseInsensitive))
                    checkBox->setChecked(true);
                else
                    checkBox->setChecked(false);
                return true;
            }
            case ParaCheck:
            {
                QStringList listOBName = OBName.split("_", QString::SkipEmptyParts);
                if(listOBName.count() < 5)
                    return false;
                QString OBNameKey = listOBName.at(4);
                if(!OBNameKey.compare(pVaule, Qt::CaseInsensitive))
                {
                    checkBox->setChecked(true);
                }
                return true;
            }
            default:return false;
        }
    }
    bool MapToRadioBtn(QString pKey, QStringList pList, QString pVaule, QObject* pOB)
    {
        QRadioButton* radioButton = qobject_cast<QRadioButton*>(pOB);
        if(radioButton == 0)
            return false;
        QString OBName = radioButton->objectName();
        if(!OBName.contains(pKey, Qt::CaseInsensitive))
            return false;
        QStringList listOBName = OBName.split("_", QString::SkipEmptyParts);
        if(listOBName.count()<5)
            return false;
        ParaType paraType = GetParaType(listOBName.at(3));
        switch(paraType)
        {
            case ParaEnum:
            {
                QStringList listOBName = OBName.split("_", QString::SkipEmptyParts);
                if(listOBName.count() < 5)
                    return false;
                QString OBNameKey = listOBName.at(4);
                if(!OBNameKey.compare(pVaule, Qt::CaseInsensitive))
                {
                    radioButton->setChecked(true);
                }
                return true;
            }
            default:return false;
        }
    }
    bool MapToSlider(QString pKey, QStringList pList, QString pVaule, QObject* pOB)
    {
        QSlider* slider = qobject_cast<QSlider*>(pOB);
        if(slider == 0)
            return false;
        QString OBName = slider->objectName();
        if(!OBName.contains(pKey, Qt::CaseInsensitive))
            return false;
        QStringList listOBName = OBName.split("_", QString::SkipEmptyParts);
        if(listOBName.count()<5)
            return false;
        ParaType paraType = GetParaType(listOBName.at(3));
        switch(paraType)
        {
            case ParaNum:
            {
                int vaule = 0;
                if(!StrToInt(vaule, 10, pVaule))
                    return false;
                slider->setValue(vaule);
                return true;
            }
            case ParaEnum:
            {
                QString vaules = slider->toolTip();
                QStringList vauleList = vaules.split(";", QString::SkipEmptyParts);
                for(int i=0;i<vauleList.count();i++)
                {
                    QString vaule = vauleList.at(i);
                    if(!vaule.compare(pVaule, Qt::CaseInsensitive))
                    {
                        slider->setValue(i);
                        return true;
                    }
                }
                return false;
            }
            default:return false;
        }
    }
    bool MapToUI(QString pKey, QStringList pList, QString pVaule, QObject* pOB)
    {
        MapToGroupBox(pKey, pList, pVaule, pOB);
        MapToLineEdit(pKey, pList, pVaule, pOB);
        MapToComboBox(pKey, pList, pVaule, pOB);
        MapToListWidget(pKey, pList, pVaule, pOB);
        MapToCheckBox(pKey, pList, pVaule, pOB);
        MapToRadioBtn(pKey, pList, pVaule, pOB);
        MapToSlider(pKey, pList, pVaule, pOB);
        return true;
    }
//>@>>Out
    bool PreMapToDom(DomInfo& pDomInfo, QStringList& pList, QObject* pOB)
    {
        QString OBName = pOB->objectName();
        pList = OBName.split("_", QString::SkipEmptyParts);
        if(pList.count() < 4)
            return false;
        pDomInfo.mParLabel = QString("/CfgPara/%2/%3").arg(pList.at(0)).arg(pList.at(1));
        pDomInfo.mTagName = pList.at(2);
        pDomInfo.mAtPara = pList.at(3);
        return true;
    }
    bool EndMapToDom(DomInfo& pDomInfo)
    {
        if(pDomInfo.mAtList.endsWith(";"))
           pDomInfo.mAtList.remove(pDomInfo.mAtList.length()-1, 1);
        if(pDomInfo.mText.endsWith(";"))
           pDomInfo.mText.remove(pDomInfo.mText.length()-1, 1);
        if((pDomInfo.mParLabel.isEmpty())||(pDomInfo.mTagName.isEmpty())||(pDomInfo.mAtPara.isEmpty()))
            return false;
        return true;
    }
    bool GroupBoxMapToDom(DomInfo& pDomInfo, QObject* pOB)
    {
        QStringList list;
        if(!PreMapToDom(pDomInfo, list, pOB))
            return false;
        QGroupBox* groupBox = qobject_cast<QGroupBox*>(pOB);
        if(groupBox == 0)
            return false;
        ParaType paraType = GetParaType(pDomInfo.mAtPara);
        if(paraType == ParaInvalid)
            return false;
        QObjectList cldOBList = groupBox->children();
        bool flag = false;
        for(int i=0;i<cldOBList.count();i++)
        {
            QObject* cldOB = cldOBList.at(i);
            if(LineEditMapToDom(pDomInfo, cldOB))
                flag = true;
            else if(ComboBoxMapToDom(pDomInfo, cldOB))
                flag = true;
            else if(ListWidgetMapToDom(pDomInfo, cldOB))
                flag = true;
            else if(CheckBoxMapToDom(pDomInfo, cldOB))
                flag = true;
            else if(RadioBtnMapToDom(pDomInfo, cldOB))
                flag = true;
            else if(SliderMapToDom(pDomInfo, cldOB))
                flag = true;

        }
        return flag;
    }
    bool LineEditMapToDom(DomInfo& pDomInfo, QObject* pOB)
    {
        QStringList list;
        if(!PreMapToDom(pDomInfo, list, pOB))
            return false;
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(pOB);
        if(lineEdit == 0)
            return false;
        ParaType paraType = GetParaType(pDomInfo.mAtPara);
        if(paraType == ParaInvalid)
            return false;
        switch(paraType)
        {
            case ParaStr:
            {
                pDomInfo.mText = lineEdit->text();
                return true;
            }
            default:return false;
        }
    }
    bool ComboBoxMapToDom(DomInfo& pDomInfo, QObject* pOB)
    {
        QStringList list;
        if(!PreMapToDom(pDomInfo, list, pOB))
            return false;
        QComboBox* comboBox = qobject_cast<QComboBox*>(pOB);
        if(comboBox == 0)
            return false;
        ParaType paraType = GetParaType(pDomInfo.mAtPara);
        if(paraType == ParaInvalid)
            return false;
        switch(paraType)
        {
            case ParaEnum:
            {
                pDomInfo.mText = comboBox->currentText();
                return true;
            }
            default:return false;
        }
    }
    bool ListWidgetMapToDom(DomInfo& pDomInfo, QObject* pOB)
    {
        QStringList list;
        if(!PreMapToDom(pDomInfo, list, pOB))
            return false;
        QListWidget* listWidget = qobject_cast<QListWidget*>(pOB);
        if(listWidget == 0)
            return false;
        ParaType paraType = GetParaType(pDomInfo.mAtPara);
        if(paraType == ParaInvalid)
            return false;
        switch(paraType)
        {
            case ParaEnum:
            case ParaCheck:
            {
                for(int i=0;i<listWidget->count();i++)
                {
                    QListWidgetItem* item = listWidget->item(i);
                    pDomInfo.mAtList.append(item->text());
                    pDomInfo.mAtList.append(";");
                    if(item->checkState() == Qt::Checked)
                    {
                        pDomInfo.mText.append(item->text());
                        pDomInfo.mText.append(";");
                    }
                }
                return true;
            }
            default:return false;
        }
    }
    bool CheckBoxMapToDom(DomInfo& pDomInfo, QObject* pOB)
    {
        QStringList list;
        if(!PreMapToDom(pDomInfo, list, pOB))
            return false;
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(pOB);
        if(checkBox == 0)
            return false;
        ParaType paraType = GetParaType(pDomInfo.mAtPara);
        if(paraType == ParaInvalid)
            return false;
        switch(paraType)
        {
            case ParaBool:
            {
                if(checkBox->isChecked())
                    pDomInfo.mText = QString("True");
                else
                    pDomInfo.mText = QString("False");
                return true;
            }
            case ParaCheck:
            {
                pDomInfo.mAtList.append(list.at(list.count()-1));
                pDomInfo.mAtList.append(";");
                if(checkBox->isChecked())
                {
                    pDomInfo.mText.append(list.at(list.count()-1));
                    pDomInfo.mText.append(";");
                }
                return true;
            }
            default:return false;
        }
    }
    bool RadioBtnMapToDom(DomInfo& pDomInfo, QObject* pOB)
    {
        QStringList list;
        if(!PreMapToDom(pDomInfo, list, pOB))
            return false;
        QRadioButton* radioBtn = qobject_cast<QRadioButton*>(pOB);
        if(radioBtn == 0)
            return false;
        ParaType paraType = GetParaType(pDomInfo.mAtPara);
        if(paraType == ParaInvalid)
            return false;
        switch(paraType)
        {
            case ParaEnum:
            {
                pDomInfo.mAtList.append(list.at(list.count()-1));
                pDomInfo.mAtList.append(";");
                if(radioBtn->isChecked())
                {
                    pDomInfo.mText.append(list.at(list.count()-1));
                    pDomInfo.mText.append(";");
                }
                return true;
            }
            default:return false;
        }
    }
    bool SliderMapToDom(DomInfo& pDomInfo, QObject* pOB)
    {
        QStringList list;
        if(!PreMapToDom(pDomInfo, list, pOB))
            return false;
        QSlider* slider = qobject_cast<QSlider*>(pOB);
        if(slider == 0)
            return false;
        ParaType paraType = GetParaType(pDomInfo.mAtPara);
        if(paraType == ParaInvalid)
            return false;
        int vaule = slider->value();
        switch(paraType)
        {
            case ParaNum:
            {
                pDomInfo.mText = QString("%1").arg(vaule);
                return true;
            }
            case ParaEnum:
            {
                QString paras = slider->toolTip();
                QStringList paraList = paras.split(";", QString::SkipEmptyParts);
                if(vaule >= paraList.count())
                    return false;
                pDomInfo.mAtList = paras;
                pDomInfo.mText = paraList.at(vaule);
                return true;
            }
            default:return false;
        }
    }
    bool MapToDom(DomInfo& pDomInfo, QObject* pOB)
    {
        pDomInfo.mAtList.clear();
        pDomInfo.mText.clear();
        QObject* parOB = pOB->parent();
        QGroupBox* groupBox = qobject_cast<QGroupBox*>(parOB);
        if(groupBox != 0)
        {
            if(groupBox->isCheckable())
            {
                if(!groupBox->isChecked())
                    return false;
            }
        }
        bool flag = false;
        if(GroupBoxMapToDom(pDomInfo, pOB))
            flag = true;
        else if(LineEditMapToDom(pDomInfo, pOB))
            flag = true;
        else if(ComboBoxMapToDom(pDomInfo, pOB))
            flag = true;
        else if(ListWidgetMapToDom(pDomInfo, pOB))
            flag = true;
        else if(CheckBoxMapToDom(pDomInfo, pOB))
            flag = true;
        else if(RadioBtnMapToDom(pDomInfo, pOB))
            flag = true;
        else if(SliderMapToDom(pDomInfo, pOB))
            flag = true;
        if(flag)
            return EndMapToDom(pDomInfo);
        return false;
    }

    bool CreatBootMdp(QString pPath)
    {
        CloseBootMdp();
        mMdpManager = new mdpManager(pPath);
        if(!mMdpManager->NewMDP())
            return false;
        CloseBootMdp();
        if(!OpenBootMdp(pPath))
            return false;
        return true;
    }
    bool OpenBootMdp(QString pPath)
    {
        CloseBootMdp();
        mMdpManager = new mdpManager(pPath);
        if(!mMdpManager->OpenMDP())
        {
            mMdpManager->m_IsChanged = false;
            delete mMdpManager;
            mMdpManager = 0;
            IDE_TRACE_STR("OpenBootMdp#Failed");
            return false;
        }
        return true;
    }
    void CloseBootMdp()
    {
        if(mMdpManager)
        {
            mMdpManager->m_IsChanged = false;
            delete mMdpManager;
            mMdpManager = 0;
        }
    }
    bool SaveBootMdp()
    {
        if(mMdpManager)
        {
            mMdpManager->m_IsChanged = true;
            if(!mMdpManager->SaveMDP())
            {
                IDE_TRACE_STR("SaveBootMDP#Failed");
                return false;
            }
            return true;
        }
        return false;
    }

    mdpManager*                 mMdpManager;
    QString                     mMDPPath;
    QIcon                       mThemeListIcon;
private slots:
    void on_PHTypeList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_Sreen_LCD_Resolution_Enum_Vaule_valueChanged(int value);

    void on_Sreen_LCD_luminance_Num_Vaule_valueChanged(int value);

    void on_Sreen_LCD_Sound_Num_Vaule_valueChanged(int value);

    void on_Sreen_USBCAM_Resolution_Enum_Vaule_valueChanged(int value);

    void on_SaveBtn_clicked();

    void on_RefreshBtn_clicked();

    void on_AddThemeBtn_clicked();

    void on_DelThemeBtn_clicked();

private:
    Ui::MDPForm *ui;
};

#endif // MDPFORM_H
