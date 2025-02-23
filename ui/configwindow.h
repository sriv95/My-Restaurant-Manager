#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QDialog>

namespace Ui {
class configwindow;
}

class configwindow : public QDialog
{
    Q_OBJECT

public:
    explicit configwindow(QWidget *parent = nullptr);
    ~configwindow();

private:
    Ui::configwindow *ui;
};

#endif // CONFIGWINDOW_H
