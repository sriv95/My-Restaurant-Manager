#include "reserve.h"
#include "ui_reserve.h"
#include "header/json.h"
#include <QMessageBox>

reserve::reserve(int tableNo,QWidget *parent)
    : QDialog(parent), ui(new Ui::reserve)
{
    ui->setupUi(this);

    ui->TableNum->setText(QString::number(tableNo));

    buttonSound = new QMediaPlayer(this);
    buttonAudio = new QAudioOutput(this);
    buttonSound->setAudioOutput(buttonAudio);
    buttonSound->setSource(QUrl("qrc:/Sounds/Button.mp3"));

}

reserve::~reserve()
{
    delete ui;
}

void reserve::playButtonSound()
{
    if (buttonSound->playbackState() == QMediaPlayer::PlayingState) {
        buttonSound->setPosition(0);
    } else {
        buttonSound->play();
    }
}

void reserve::on_btnReserve_clicked()
{
    playButtonSound();
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
    playButtonSound();
    this->close();
}

