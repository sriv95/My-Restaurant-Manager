#ifndef RESERVE_H
#define RESERVE_H

#include <QDialog>
#include <QString>


namespace Ui {
class reserve;
}

class reserve : public QDialog
{
    Q_OBJECT

public:
    explicit reserve(int tableNo,QWidget *parent = nullptr);
    ~reserve();

signals:
    void tableReserved(int tableId, const QString &customerName);

private slots:

    void on_btnReserve_clicked();

    void on_Cancelbtn_clicked();

private:
    Ui::reserve *ui;

};

#endif // RESERVE_H
