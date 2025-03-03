#include "reserve.h"
#include "ui_reserve.h"
#include "header/json.h"
#include <QMessageBox>

reserve::reserve(int tableNo,QWidget *parent)
    : QDialog(parent), ui(new Ui::reserve)
{
    ui->setupUi(this);
    setFixedSize(389,430);

    ui->TableNum->setText(QString::number(tableNo));

}

reserve::~reserve()
{
    delete ui;
}


void reserve::on_btnReserve_clicked()
{
    int tableNum = ui->TableNum->text().toInt();

    QString reservationTime = ui->ReservationTime->text();
    QString customerName = ui->CustomerName->text();
    QString phoneNum = ui->PhoneNum->text();

    if (reservationTime.isEmpty() || customerName.isEmpty() || phoneNum.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please fill in all fields correctly.");
        return;
    }

    json newData;
    newData[0] = tableNum;
    newData[1] = reservationTime.toStdString();
    newData[2] = customerName.toStdString();
    newData[3] = phoneNum.toStdString();

    json Data;
    getData(Data, "Reservation");

    int len = lenData(Data);

    qDebug()<<QString::fromStdString(Data.dump());
    Data[len] = newData;
    qDebug()<<QString::fromStdString(Data.dump());
    setData(Data, "Reservation");

    json tablesData;
    getData(tablesData, "Tables");

    for (auto &table : tablesData) {
        if (table["No"] == tableNum) {
            table["Reserved"] = reservationTime.toStdString();
            break;
        }
    }

    setData(tablesData, "Tables");

    this->close();
}

void reserve::on_Cancelbtn_clicked()
{
    this->close();
}

