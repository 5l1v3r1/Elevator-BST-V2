#ifndef MIDDLETOPTABXML_H
#define MIDDLETOPTABXML_H

#include "tabpage.h"
#include "ui_middletoptabxml.h"

namespace Ui {
class MiddleTopTabXML;
}

class MiddleTopTabXML : public TabPage
{
    Q_OBJECT
public:
    explicit MiddleTopTabXML(QWidget *parent = 0);
    MiddleTopTabXML(QWidget *parent, QString FileName);
    ~MiddleTopTabXML();

    xmlParse *testParse;
    
private:
    Ui::MiddleTopTabXML *ui;
};

#endif // MIDDLETOPTABXML_H
