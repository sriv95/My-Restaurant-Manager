#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QDialog>
#include <QMediaPlayer>
#include <QAudioOutput>

namespace Ui {
class employee;
}

class employee : public QDialog
{
    Q_OBJECT

public:
    explicit employee(QWidget *parent = nullptr);
    ~employee();

private slots:

    void on_Refresh_Button_clicked();

    void Refresh_TableData();

    void on_Add_Button_clicked();

    void on_Delete_Button_clicked();

    bool Check_empty_cell_in_table();

    bool Check_Correct_DataType_in_cell();

    void on_Save_Button_clicked();

private:
    Ui::employee *ui;

    QMediaPlayer *buttonSound;
    QAudioOutput *buttonAudio;

    void playButtonSound();
};

#endif // EMPLOYEE_H
