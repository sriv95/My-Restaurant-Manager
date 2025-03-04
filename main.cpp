#include <QtWidgets/QApplication>
#include "mainwindow.h"
#include "header/json.h"

#include "ui/configwindow.h"

int main(int argc, char *argv[])
{    QApplication a(argc, argv);

    settings = new QSettings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    settings->setValue("version","v2.1.2");

    RestuarantManagement w;
    w.setWindowTitle("My Restaurant Manager "+settings->value("version").toString());
    w.show();

    return a.exec();
}
