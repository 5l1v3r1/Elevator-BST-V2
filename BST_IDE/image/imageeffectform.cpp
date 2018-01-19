#include "imageeffectform.h"
#include "ui_imageeffectform.h"

ImageEffectForm::ImageEffectForm(IMAGEITEMINFO *pItemInfo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageEffectForm)
{
    ui->setupUi(this);
    m_StartColor = Qt::white;
    m_EndColor = Qt::white;
    m_TransparentValue = 100;
    QTimer::singleShot(100, this, SLOT(UpdatePreview()));
    connect(ui->Preview, SIGNAL(UpdatePreview()), this, SLOT(UpdatePreview()));
    connect(ui->FileListTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(slot_ImagePreview(QTreeWidgetItem*,int)));
    m_ItemInfo = pItemInfo;
    ui->SelectTemplateGroup->setHidden(true);
}

ImageEffectForm::~ImageEffectForm()
{
    delete ui;
}

void ImageEffectForm::on_StartColorBtn_clicked()
{
    QColor tmpColor = QColorDialog::getColor(QColorDialog::ShowAlphaChannel);
    if(tmpColor.isValid())
    {
        m_StartColor = tmpColor;
        UpdatePreview();
    }
}

void ImageEffectForm::on_EndColorBtn_clicked()
{
    QColor tmpColor = QColorDialog::getColor(QColorDialog::ShowAlphaChannel);
    if(tmpColor.isValid())
    {
        m_EndColor = tmpColor;
        UpdatePreview();
    }
}

void ImageEffectForm::on_TransparentSlider_valueChanged(int value)
{
    m_TransparentValue = value;
    UpdatePreview();
}

void ImageEffectForm::on_TemplateTree_itemChanged(QTreeWidgetItem *item, int column)
{

}

void ImageEffectForm::UpdatePreview()
{
    UpdateImage();
}

bool ImageEffectForm::UpdateImage()
{
    if(ui->BtnAdvanced->isChecked())  //>@高级
    {
        QImage tmpImage(ui->Preview->size(), QImage::Format_ARGB32_Premultiplied);
        if(tmpImage.isNull())
        {
            IDE_TRACE();
            return false;
        }
        tmpImage.fill(Qt::transparent); //>@创建透明图层
        QPainter painter(&tmpImage);
        painter.setRenderHint(QPainter::Antialiasing, true);//消除锯齿
        QLinearGradient tmpGradient(QPoint(0,0), QPoint(ui->Preview->width(), 0));
        //创建了一个QConicalGradient对象实例，参数分别为中心坐标和初始角度
        tmpGradient.setColorAt(0, m_StartColor);
        tmpGradient.setColorAt(1, m_EndColor);
        painter.setBrush(QBrush(tmpGradient));
        painter.setOpacity(((qreal)m_TransparentValue)/100.0);
        painter.drawRect(ui->Preview->rect());
        painter.end();
        m_EffectPic = QPixmap::fromImage(tmpImage);
        ui->Preview->setPixmap(m_EffectPic);
    }
    else                                //>@选择文件级别
    {
        QImage tmpImage(ui->FileSelectPath->text());
        if(tmpImage.isNull())
        {
            IDE_TRACE();
            return false;
        }
        m_EffectPic = QPixmap::fromImage(tmpImage);
        ui->FilePreview->setPixmap(QPixmap::fromImage(tmpImage.scaled(ui->FilePreview->size())));
    }
    return true;
}

void ImageEffectForm::accept()
{
    if(UpdateImage() == false)
    {
        QMessageBox::warning(this, tr("Warn"),
                             tr("The pixmap is invalid, please check it!"),
                             QMessageBox::Ok);
        return;
    }
    QDialog::accept();
}

void ImageEffectForm::reject()
{
    QDialog::reject();
}

void ImageEffectForm::slot_ImagePreview(QTreeWidgetItem *pItem, int pColumn)
{
    if(pItem)
    {
        QString fileName = pItem->statusTip(0);
        ui->FileSelectPath->setText(fileName);
        UpdateImage();
    }
}

void ImageEffectForm::on_BtnSelect_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(0, tr("Open Image File"),
                                         QDir::currentPath(),
                                         QString("*.bmp *.png *.jpg *.svg"));
    if (fileName.isEmpty() || !isFileExist(fileName))
        return;
    ui->FileSelectPath->setText(fileName);
    ui->FileListTree->UpdateTreeWidget(getFileDirectory(fileName), QStringList()<<"bmp" << "png" << "jpg");
    UpdateImage();
}

void ImageEffectForm::on_BtnAdvanced_clicked(bool checked)
{
    if(checked)
    {
        ui->SelectTemplateGroup->setHidden(false);
        ui->SelectFileGroup->setHidden(true);
    }
    else
    {
        ui->SelectTemplateGroup->setHidden(true);
        ui->SelectFileGroup->setHidden(false);
    }
}
