#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <QDialog>
#include <vector>

using namespace std;

namespace Ui {
class analysis;
}

class analysis : public QDialog
{
    Q_OBJECT

public:
    explicit analysis(QWidget *parent = nullptr);
    ~analysis();

private slots:

    void on_calendar_end_date_selectionChanged();

    void on_calendar_start_date_selectionChanged();

    void on_Refresh_clicked();

    void on_comboBox_search_mode_currentIndexChanged(int index);

    void on_calendar_start_date_currentPageChanged(int year, int month);

    void on_accept_clicked();

private:
    Ui::analysis *ui;

    void startUI_setup();

    void Show_Chart();

    void Update_Selectable_and_Highlight_DateRange();

    vector<QDate> Get_Week_Dates(QDate selectedDate);

    vector<QDate> Get_Month_Dates(QDate selected_Date);

    vector<QDate> Get_Year_Dates(QDate selected_Date);

    void Update_Highlight_Week();

    void Update_Highlight_Month();

    void Update_Highlight_Year();

    void Refresh_calendar();
};

#endif // ANALYSIS_H
