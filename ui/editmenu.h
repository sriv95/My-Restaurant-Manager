#ifndef EDITMENU_H
#define EDITMENU_H

#include "qtablewidget.h"
#include <QDialog>
#include <QMediaPlayer>
#include <QAudioOutput>

namespace Ui {
class editmenu;
}

class editmenu : public QDialog
{
    Q_OBJECT

public:
    explicit editmenu(QWidget *parent = nullptr);
    ~editmenu();

private slots:
    void on_RefreshBtn_clicked(bool);

    void on_AddMenuBtn_clicked();

    void on_DelMenuBtn_clicked();

    void on_AddIngBtn_clicked();

    void RefreshIng(int);

    void on_SaveMenuBtn_clicked();

    void onMenuTableItemChanged(QTableWidgetItem *);


private:
    Ui::editmenu *ui;

    QMediaPlayer *buttonSound;
    QAudioOutput *buttonAudio;

    void playButtonSound();
};

#endif // EDITMENU_H
