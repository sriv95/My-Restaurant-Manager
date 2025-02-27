#ifndef ORDERSTOCK_H
#define ORDERSTOCK_H

#include <QDialog>
#include "header/json.h"
#include <QMediaPlayer>
#include <QAudioOutput>


namespace Ui {
class OrderStock;
}

class OrderStock : public QDialog
{
    Q_OBJECT

public:
    explicit OrderStock(QWidget *parent = nullptr);
    ~OrderStock();

private slots:
    void on_AddBtn_clicked();

private:
    Ui::OrderStock *ui;

    QMediaPlayer *buttonSound;
    QAudioOutput *buttonAudio;

    QMediaPlayer *OpenMenuSound;
    QAudioOutput *OpenMenuAudio;

    QMediaPlayer *SelectSound;
    QAudioOutput *SelectAudio;

    void playButtonSound();
    void playOpenMenuSound();
    void playSelectSound();
};

#endif // ORDERSTOCK_H
