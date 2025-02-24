#ifndef STATEMENT_H
#define STATEMENT_H

#include <QDialog>
#include <QMediaPlayer>
#include <QAudioOutput>

namespace Ui {
class Statement;
}

class Statement : public QDialog
{
    Q_OBJECT

public:
    explicit Statement(QWidget *parent = nullptr);
    ~Statement();

private slots:
    //void on_Month_ComboBox_activated(int index);

   //void on_Year_Input_activated(int index);

private:
    Ui::Statement *ui;
    void loadData();

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

#endif // STATEMENT_H
