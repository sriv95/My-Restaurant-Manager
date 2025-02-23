#include "configwindow.h"
#include "ui_configwindow.h"

configwindow::configwindow(QWidget *parent,QMap<QString,QPalette> PalettesMap)
    : QDialog(parent)
    , ui(new Ui::configwindow)
{
    ui->setupUi(this);
}

configwindow::~configwindow()
{
    delete ui;
}
