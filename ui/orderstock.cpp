#include "orderstock.h"
#include "qdatetime.h"
#include "qmessagebox.h"
#include "ui_orderstock.h"

json Data;

OrderStock::OrderStock(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OrderStock)
{
    ui->setupUi(this);

    getAllData(Data);

    //Add items to comboBox
    for(auto stock : Data["Stocks"]){
        ui->SelectStock->addItem(QString::fromStdString(stock[0]));
    }
}

OrderStock::~OrderStock()
{
    delete ui;
}

void OrderStock::on_AddBtn_clicked()
{
    if(ui->Price->value()<=0) {QMessageBox::warning(this, "Invalid Input", "Price must be greater than 0");return;} //Price Handle
    if(ui->Quan->value()<=0) {QMessageBox::warning(this, "Invalid Input", "Quantity must be greater than 0");return;} //Quan Handle

    QComboBox *SelectStock = ui->SelectStock;
    Data["Stocks"][SelectStock->currentIndex()][1] = double((Data["Stocks"][SelectStock->currentIndex()][1])) + ui->Quan->value(); //Add Stock

    //Add Statement
    int len = Data["Statement"].size();
    Data["Statement"][len][0] = SelectStock->currentText().toStdString(); //Name
    Data["Statement"][len][1] = ui->Quan->value(); //Quan
    Data["Statement"][len][2] = ui->Price->value()*-1; //Price
    Data["Statement"][len][3] = QDate::currentDate().toString("dd-MM-yyyy").toStdString(); //Date
    Data["Statement"][len][4] = QTime::currentTime().toString("hh:mm:ss.zzz").toStdString(); //Time

    setAllData(Data);
    ui->Quan->setValue(0);
    ui->Price->setValue(0);
}

