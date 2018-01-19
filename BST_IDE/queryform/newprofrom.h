#ifndef NEWPROFROM_H
#define NEWPROFROM_H

#include "baseform.h"

class NewProFrom : public BaseForm
{
    Q_OBJECT
public:
    explicit NewProFrom(QWidget *parent = 0);
    ~NewProFrom();
public slots:
    virtual void accept();
    virtual void reject();
};

#endif // NEWPROFROM_H
