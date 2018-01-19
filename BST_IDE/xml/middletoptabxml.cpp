#include "middletoptabxml.h"
#include "qobjectxmlmodel.h"
#include "xmlsyntaxhighlighter.h"
#include <QXmlFormatter>

MiddleTopTabXML::MiddleTopTabXML(QWidget *parent) :
    TabPage(parent),
    ui(new Ui::MiddleTopTabXML)
{
}

MiddleTopTabXML::MiddleTopTabXML(QWidget *parent, QString FileName):
    TabPage(parent),
    ui(new Ui::MiddleTopTabXML)
{
    ui->setupUi(this);

    /* Setup the font. */
    {
        QFont font("Courier");
        font.setFixedPitch(true);

        ui->xmlTextEdit->setFont(font);
    }
    {
        QFile queryFile(FileName);
        queryFile.open(QIODevice::ReadOnly);
        ui->xmlTextEdit->setPlainText(QString::fromUtf8(queryFile.readAll()));
    }
}

MiddleTopTabXML::~MiddleTopTabXML()
{
    delete ui;
}
