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
    setMaximumSize(400,300);

    getAllData(Data);

    //Add items to comboBox
    for(auto stock : Data["Stocks"]){
        ui->SelectStock->addItem(QString::fromStdString(stock[0]));
    }

    buttonSound = new QMediaPlayer(this);
    buttonAudio = new QAudioOutput(this);
    buttonSound->setAudioOutput(buttonAudio);
    buttonSound->setSource(QUrl("qrc:/Sounds/Button.mp3"));

    OpenMenuSound = new QMediaPlayer(this);
    OpenMenuAudio = new QAudioOutput(this);
    OpenMenuSound->setAudioOutput(OpenMenuAudio);
    OpenMenuSound->setSource(QUrl("qrc:/Sounds/OpenCombobox.mp3"));

    SelectSound = new QMediaPlayer(this);
    SelectAudio = new QAudioOutput(this);
    SelectSound->setAudioOutput(SelectAudio);
    SelectSound->setSource(QUrl("qrc:/Sounds/Combobox_click.mp3"));

    connect(ui->SelectStock, &QComboBox::showPopup, this, &OrderStock::playOpenMenuSound);
    connect(ui->SelectStock, &QComboBox::activated, this, &OrderStock::playSelectSound);
}

OrderStock::~OrderStock()
{
    delete ui;
}

void OrderStock::playButtonSound()
{
    if (buttonSound->playbackState() == QMediaPlayer::PlayingState) {
        buttonSound->setPosition(0);
    } else {
        buttonSound->play();
    }
}

void OrderStock::playSelectSound()
{
    if (SelectSound->playbackState() == QMediaPlayer::PlayingState) {
        SelectSound->setPosition(0);
    } else {
        SelectSound->play();
    }
}

void OrderStock::playOpenMenuSound()
{
    if (OpenMenuSound->playbackState() == QMediaPlayer::PlayingState) {
        OpenMenuSound->setPosition(0);
    } else {
        OpenMenuSound->play();
    }
}

void OrderStock::on_AddBtn_clicked()
{
    playButtonSound();
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

