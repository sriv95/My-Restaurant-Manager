#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

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
private:
    Ui::configwindow *ui;
    QNetworkAccessManager *networkManager;
    QString currentVersion = "v1.0";
};

#endif // CONFIGWINDOW_H
