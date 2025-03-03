#include "jsoncheck.h"
#include "ui_jsoncheck.h"
#include <QMessageBox>
#include <QFileDialog>
#include <header/json.h>

jsoncheck::jsoncheck(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::jsoncheck)
{
    ui->setupUi(this);
    setFixedSize(719,621);
    ui->textlabel->setAlignment(Qt::AlignCenter);
    ui->textlabel->setWordWrap(true);
    ui->continueBtn->setVisible(false);

    if (checkData()) {
        ui->textlabel->setText("✅ Data found at [ " + QString::fromStdString(getPATH()) + " ] ✅");
        ui->continueBtn->setVisible(true);
    } else {
        ui->textlabel->setText("⚠️ Data Not Found ⚠️");
    }
}



jsoncheck::~jsoncheck()
{
    delete ui;
}


void jsoncheck::on_newBtn_clicked()
{
    newData();
    QString filePath = QDir::currentPath() + "/data.json";
    updateFilePath(filePath.toStdString());

        QMessageBox Created;
        Created.setText("data.json has been created successfully. 😃😘💪🏿💪🏿");
        Created.setIcon(QMessageBox::Information);
        Created.setWindowFlags(Qt::Popup);
        Created.exec();

        ui->textlabel->setText("Data created at [ " + filePath + " ] ✔️");
        ui->continueBtn->setVisible(true);

}



void jsoncheck::on_openBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select data.json", "", "JSON Files (*.json)");

    if (!filePath.isEmpty()) {

        updateFilePath(filePath.toStdString());

        QMessageBox Opened;
        Opened.setText("data.json has been opened successfully. 😉💋💋");
        Opened.setIcon(QMessageBox::Information);
        Opened.setWindowFlags(Qt::Popup);
        Opened.exec();

        ui->textlabel->setText("Data opened at [ " + filePath + " ] ✔️");
        ui->continueBtn->setVisible(true);
        } else {
        qDebug() << "No file selected";
    }

}


void jsoncheck::on_continueBtn_clicked()
{
    this->close();
}

void jsoncheck::on_cancelBtn_clicked()
{
    forcecloseTriggered = true;
    this->close();
}
