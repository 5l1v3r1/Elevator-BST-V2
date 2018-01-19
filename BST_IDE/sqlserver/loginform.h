#ifndef SPLASHFORM_H
#define SPLASHFORM_H

#include <QDialog>

namespace Ui {
class SplashForm;
}

class SplashForm : public QDialog
{
    Q_OBJECT
    
public:
    explicit SplashForm(QWidget *parent = 0);
    ~SplashForm();
    
    void accept();
    void reject();

private:
    Ui::SplashForm *ui;
};

#endif // SPLASHFORM_H
