#include "opentabledialog.h"
#include "ui_opentabledialog.h"
#include <QMessageBox>
//#include <Qtimer>

OpenTableDialog::OpenTableDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OpenTableDialog)
{
    ui->setupUi(this);

    connect(ui->one, &QPushButton::clicked, this, &OpenTableDialog::on_numBtn_clicked);
    connect(ui->two, &QPushButton::clicked, this, &OpenTableDialog::on_numBtn_clicked);
    connect(ui->three, &QPushButton::clicked, this, &OpenTableDialog::on_numBtn_clicked);
    connect(ui->four, &QPushButton::clicked, this, &OpenTableDialog::on_numBtn_clicked);

    buttonSound = new QMediaPlayer(this);
    buttonAudio = new QAudioOutput(this);
    buttonSound->setAudioOutput(buttonAudio);
    buttonSound->setSource(QUrl("qrc:/Sounds/Button.mp3"));
}

OpenTableDialog::~OpenTableDialog()
{
    delete ui;
}

void OpenTableDialog::playButtonSound()
{
    if (buttonSound->playbackState() == QMediaPlayer::PlayingState) {
        buttonSound->setPosition(0);
    } else {
        buttonSound->play();
    }
}

void OpenTableDialog::on_numBtn_clicked()
{
    QPushButton *buttonSender = qobject_cast<QPushButton*>(sender());
    QString buttonText = buttonSender->text();
    ui->TextInput->setPlainText(buttonText);
}

void OpenTableDialog::on_buttonBox_accepted()
{
    playButtonSound();
    bool testvalue;
    int seat = ui->TextInput->toPlainText().trimmed().toInt(&testvalue);
    qDebug()<< testvalue;
    if(testvalue == true and seat > 0) emit returnValue(ui->TextInput->toPlainText());
    else
    {
        QMessageBox::about(this , "warning" , "Invalid data type. It should be an integer non-negative." );

    }
    this->close();
    //QTimer::singleShot(300, this, &OpenTableDialog::close);
}

void OpenTableDialog::on_buttonBox_rejected() //cansel
{
    playButtonSound();
    this->close();
    //QTimer::singleShot(300, this, &OpenTableDialog::close);
}
