#ifndef NEWFILEFORM_H
#define NEWFILEFORM_H

#include "baseform.h"

class newFileForm : public BaseForm
{
    Q_OBJECT
    
public:
    explicit newFileForm(QWidget *parent = 0);
    ~newFileForm();
public slots:
    virtual void accept();
    virtual void reject();
};

#endif // NEWFILEFORM_H
