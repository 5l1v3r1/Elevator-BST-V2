#ifndef GENERATORFORM_H
#define GENERATORFORM_H

#include "global.h"
#include <QWidget>

namespace Ui {
class GeneratorForm;
}

class GeneratorForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit GeneratorForm(QWidget *parent = 0);
    ~GeneratorForm();
    
private:
    Ui::GeneratorForm *ui;
};

#endif // GENERATORFORM_H
