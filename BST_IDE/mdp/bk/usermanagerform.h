#ifndef USERMANAGERFORM_H
#define USERMANAGERFORM_H

#include <QWidget>

namespace Ui {
class userManagerForm;
}

class userManagerForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit userManagerForm(QWidget *parent = 0);
    ~userManagerForm();
    
private:
    Ui::userManagerForm *ui;
};

#endif // USERMANAGERFORM_H
