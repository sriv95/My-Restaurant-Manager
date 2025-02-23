#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include "qabstractbutton.h"
#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class configwindow;
}

class configwindow : public QDialog
{
    Q_OBJECT

public:
    explicit configwindow(QWidget *parent = nullptr, QMap<QString, QPalette> = {});
    ~configwindow();
private slots:
    void onVersionCheckFinished(QNetworkReply *reply);
    void on_ThemeSelect_currentTextChanged(const QString &arg1);
    void on_buttonBox_clicked(QAbstractButton *button);

signals:
    void returnValue(const QString &data);

private:
    Ui::configwindow *ui;
    QNetworkAccessManager *networkManager;
    QString currentVersion = "v1.0";
    QMap<QString,QPalette> PalettesMap = {};
};

#endif // CONFIGWINDOW_H
