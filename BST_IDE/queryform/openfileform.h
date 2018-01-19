#ifndef OPENFILEFORM_H
#define OPENFILEFORM_H

#include "baseform.h"

class OpenFileForm : public BaseForm
{
    Q_OBJECT
public:
    explicit OpenFileForm(QWidget *parent = 0);
    ~OpenFileForm();

public slots:
    virtual void accept();
    virtual void reject();
};

#endif // OPENFILEFORM_H
