#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include "qabstractbutton.h"
#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QSettings>

extern QSettings* settings;

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

    void on_updateBtn_clicked();

    void on_fontComboBox_currentFontChanged(const QFont &f);

signals:
    void returnValue(const QString &data);

private:
    Ui::configwindow *ui;
    QNetworkAccessManager *networkManager;
    QString currentVersion = "";
    QMap<QString,QPalette> PalettesMap = {};
};

#endif // CONFIGWINDOW_H
