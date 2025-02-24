#ifndef STOCKWINDOW_H
#define STOCKWINDOW_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "header/json.h"


using namespace std;

namespace Ui {
class StockWindow;
}


class StockWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StockWindow(QWidget *parent = nullptr);
    ~StockWindow();

private slots:

    void on_AddBtn_clicked();

    void on_DeleteBtn_clicked();

    void on_SaveBtn_clicked();




private:
    Ui::StockWindow *ui;
    json stockJson;


    void refreshTable();
    void loadfromjson();
    void savetojson();
    void on_tableWidget_itemChanged(QTableWidgetItem *item);

    QMediaPlayer *buttonSound;
    QAudioOutput *buttonAudio;

    void playButtonSound();


};

#endif // STOCKWINDOW_H
