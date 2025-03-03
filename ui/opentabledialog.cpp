#include "opentabledialog.h"
#include "ui_opentabledialog.h"
#include <QMessageBox>
//#include <Qtimer>

OpenTableDialog::OpenTableDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OpenTableDialog)
{
    ui->setupUi(this);
    setMaximumSize(400,300);

    connect(ui->one, &QPushButton::clicked, this, &OpenTableDialog::on_numBtn_clicked);
    connect(ui->two, &QPushButton::clicked, this, &OpenTableDialog::on_numBtn_clicked);
    connect(ui->three, &QPushButton::clicked, this, &OpenTableDialog::on_numBtn_clicked);
    connect(ui->four, &QPushButton::clicked, this, &OpenTableDialog::on_numBtn_clicked);

}

OpenTableDialog::~OpenTableDialog()
{
    delete ui;
}

void OpenTableDialog::on_numBtn_clicked()
{
    QPushButton *buttonSender = qobject_cast<QPushButton*>(sender());
    QString buttonText = buttonSender->text();
    ui->TextInput->setPlainText(buttonText);
}

void OpenTableDialog::on_buttonBox_accepted()
{
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
    this->close();
    //QTimer::singleShot(300, this, &OpenTableDialog::close);
}
