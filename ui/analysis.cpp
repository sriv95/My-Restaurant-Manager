#include "analysis.h"
#include "ui_analysis.h"
#include <header/json.h>
#include <QtCore>
#include <QtGui>
#include <QtCharts/QtCharts>
#include <QtCharts/QLineSeries>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

analysis::analysis(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::analysis)
{
    ui->setupUi(this);
    this->setWindowTitle("Analysis");
    startUI_setup();
    Show_Chart();
}

analysis::~analysis()
{
    delete ui;
}

//========================================//

json menus , statement;

struct Dishes
{
    vector<string> name;
    vector<int> amount;
};

struct Drinks
{
    vector<string> name;
    vector<int> amount;
};

vector<Dishes> Dishes_data;
vector<Drinks> Drinks_data;

vector<string> all_Dishes_name;
vector<string> all_Drinks_name;

Dishes template_Dishes;
Drinks template_Drinks;

//========================================//

vector<double> chartData_income , chartData_expenses;
vector<QDate> chartData_Date;

vector<QDate> week_dates , month_dates , year_dates;

QChartView *chartView;

//========================================//

void analysis::startUI_setup()
{
    chartView = 0;
    Refresh_calendar();

    ui->calendar_start_date->setEnabled(false);
    ui->calendar_end_date->setEnabled(false);

    ui->NO_DATA->hide();

    getData(menus , "Menus");
    getData(statement , "Statement");

    //========================================//

    ui->Top_Dished->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->Top_Dished->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);


    ui->Top_Drinks->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->Top_Drinks->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);


    //========================================//

    for (unsigned int i = 0 ; i < menus.size() ; i++)
    {
        if (menus[i][2] == "Dishes") all_Dishes_name.push_back(menus[i][0]);
        else if (menus[i][2] == "Drinks") all_Drinks_name.push_back(menus[i][0]);
    }

    template_Dishes.name = all_Dishes_name;
    for(unsigned int i = 0 ; i < all_Dishes_name.size() ; i++) template_Dishes.amount.push_back(0);

    template_Drinks.name = all_Drinks_name;
    for(unsigned int i = 0 ; i < all_Drinks_name.size() ; i++) template_Drinks.amount.push_back(0);

    // qDebug() << "template_Dishes";
    // for (int i = 0 ; i < template_Dishes.name.size() ; i++)
    // {
    //     qDebug() << "name : " << template_Dishes.name[i] << "amount : " << template_Dishes.amount[i];
    // }

    // qDebug() << "template_Drinks";
    // for (int i = 0 ; i < template_Drinks.name.size() ; i++)
    // {
    //     qDebug() << "name : " << template_Drinks.name[i] << "amount : " << template_Drinks.amount[i];
    // }

    //========================================//

    for (unsigned int i = 0 ; i < statement.size() ; i++)
    {
        QDate Date_ = QDate::fromString(QString::fromStdString(statement[i][3]) , "dd-MM-yyyy");

        auto it = std::find(chartData_Date.begin(), chartData_Date.end(), Date_);
        if (it == chartData_Date.end())
        {
            chartData_Date.push_back(Date_);
            chartData_income.push_back(0);
            chartData_expenses.push_back(0);

            int newindex = chartData_Date.size() - 1;

            if (statement[i][2] >= 0) chartData_income[newindex] += abs(double(statement[i][2]));
            else chartData_expenses[newindex] += abs(double(statement[i][2]));

            //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//

            Dishes_data.push_back(template_Dishes);
            Drinks_data.push_back(template_Drinks);

            string name_menu_i = statement[i][0];
            int amount_menu_i = statement[i][1];

            if (statement[i][2] >= 0)
            {
                if (std::find(Dishes_data[newindex].name.begin() , Dishes_data[newindex].name.end() , name_menu_i) != Dishes_data[newindex].name.end())
                {
                    int index_menus = std::distance(Dishes_data[newindex].name.begin() , std::find(Dishes_data[newindex].name.begin() , Dishes_data[newindex].name.end() , name_menu_i));
                    Dishes_data[newindex].amount[index_menus] += amount_menu_i;
                }
                else
                {
                    int index_menus = std::distance(Drinks_data[newindex].name.begin() , std::find(Drinks_data[newindex].name.begin() , Drinks_data[newindex].name.end() , name_menu_i));
                    Drinks_data[newindex].amount[index_menus] += amount_menu_i;
                }
            }
            //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
        }
        else
        {
            int index = std::distance(chartData_Date.begin() , it);

            if (statement[i][2] > 0) chartData_income[index] += abs(double(statement[i][2]));
            else chartData_expenses[index] += abs(double(statement[i][2]));

            //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//

            string name_menu_i = statement[i][0];
            int amount_menu_i = statement[i][1];

            if (statement[i][2] >= 0)
            {
                if (std::find(Dishes_data[index].name.begin() , Dishes_data[index].name.end() , name_menu_i) != Dishes_data[index].name.end())
                {
                    int index_menus = std::distance(Dishes_data[index].name.begin() , std::find(Dishes_data[index].name.begin() , Dishes_data[index].name.end() , name_menu_i));
                    Dishes_data[index].amount[index_menus] += amount_menu_i;
                }
                else
                {
                    int index_menus = std::distance(Drinks_data[index].name.begin() , std::find(Drinks_data[index].name.begin() , Drinks_data[index].name.end() , name_menu_i));
                    Drinks_data[index].amount[index_menus] += amount_menu_i;
                }
            }
            //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
        }
    }



    vector<QDate> unSort_chartData_Date = ::chartData_Date;
    std::sort(chartData_Date.begin() , chartData_Date.end());
    vector<unsigned int> sorted_index;

    for (unsigned int i = 0 ; i < unSort_chartData_Date.size() ; i++)
    {
        if (std::find(sorted_index.begin() , sorted_index.end() , i) != sorted_index.end()) continue;

        unsigned int chnaged_index = std::distance(chartData_Date.begin() , std::find(chartData_Date.begin() , chartData_Date.end() , unSort_chartData_Date[i]));
        if (chnaged_index != i)
        {
            // qDebug() << QString::number(chnaged_index);

            double swap_income_data = chartData_income[chnaged_index];
            chartData_income[chnaged_index] = chartData_income[i];
            chartData_income[i] = swap_income_data;

            double swap_expenses_data = chartData_expenses[chnaged_index];
            chartData_expenses[chnaged_index] = chartData_expenses[i];
            chartData_expenses[i] = swap_expenses_data;

            //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//

            Dishes swap_Dishes_data = Dishes_data[chnaged_index];
            Dishes_data[chnaged_index] = Dishes_data[i];
            Dishes_data[i] = swap_Dishes_data;

            Drinks swap_Drinks_data = Drinks_data[chnaged_index];
            Drinks_data[chnaged_index] = Drinks_data[i];
            Drinks_data[i] = swap_Drinks_data;

            //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//

            sorted_index.push_back(i);
            sorted_index.push_back(chnaged_index);
        }
    }

    ui->comboBox_search_mode->setCurrentIndex(5);

    for (unsigned int i = 0 ; i < chartData_Date.size() ; i++)
    {
        qDebug() << "Date : "<< chartData_Date[i].toString("dd-MM-yyyy") << "Income : " << QString::number(chartData_income[i]) << "expenses : " << QString::number(chartData_expenses[i]);

        qDebug() << "Dishes";
        for (int j = 0 ; j < Dishes_data[i].name.size() ; j++)
        {
            qDebug() << "name : " << Dishes_data[i].name[j] << "amount : " << Dishes_data[i].amount[j];
        }

        qDebug() << "Drinks";
        for (int j = 0 ; j < Drinks_data[i].name.size() ; j++)
        {
            qDebug() << "name : " << Drinks_data[i].name[j] << "amount : " << Drinks_data[i].amount[j];
        }

        qDebug() << "\n";
    }
}


void analysis::Update_Selectable_and_Highlight_DateRange()
{
    QDate startDate = ui->calendar_start_date->selectedDate();
    QDate endDate = ui->calendar_end_date->selectedDate();
    ui->calendar_start_date->setMaximumDate(endDate);
    ui->calendar_end_date->setMinimumDate(startDate);

    // ลบ format เก่าทั้งหมดออก
    ui->calendar_start_date->setDateTextFormat(QDate(), QTextCharFormat());
    ui->calendar_end_date->setDateTextFormat(QDate(), QTextCharFormat());

    // สร้าง Format สำหรับไฮไลต์
    QTextCharFormat Highlight_Range_Format;
    Highlight_Range_Format.setBackground(Qt::green);
    Highlight_Range_Format.setForeground(Qt::black);

    for (QDate date = startDate.addDays(1) ; date <= endDate; date = date.addDays(1))
    {
        ui->calendar_start_date->setDateTextFormat(date, Highlight_Range_Format);
        ui->calendar_end_date->setDateTextFormat(date.addDays(-1), Highlight_Range_Format);
    }
}


void analysis::Update_Highlight_Week()
{
    week_dates = Get_Week_Dates(ui->calendar_start_date->selectedDate());

    // ลบ format เก่าทั้งหมดออก
    ui->calendar_start_date->setDateTextFormat(QDate(), QTextCharFormat());
    ui->calendar_end_date->setDateTextFormat(QDate(), QTextCharFormat());

    // สร้าง Format สำหรับไฮไลต์
    QTextCharFormat Highlight_Format;
    Highlight_Format.setBackground(Qt::green);
    Highlight_Format.setForeground(Qt::black);

    for (unsigned int i = 0 ; i < week_dates.size() ; i++)
    {
        if (week_dates[i] != ui->calendar_start_date->selectedDate()) ui->calendar_start_date->setDateTextFormat(week_dates[i], Highlight_Format);
    }
}


void analysis::Update_Highlight_Month()
{
    month_dates = Get_Month_Dates(ui->calendar_start_date->selectedDate());

    // ลบ format เก่าทั้งหมดออก
    ui->calendar_start_date->setDateTextFormat(QDate(), QTextCharFormat());
    ui->calendar_end_date->setDateTextFormat(QDate(), QTextCharFormat());

    // สร้าง Format สำหรับไฮไลต์
    QTextCharFormat Highlight_Format;
    Highlight_Format.setBackground(Qt::green);
    Highlight_Format.setForeground(Qt::black);

    for (unsigned long int i = 0 ; i < month_dates.size() ; i++)
    {
        if (month_dates[i] != ui->calendar_start_date->selectedDate()) ui->calendar_start_date->setDateTextFormat(month_dates[i], Highlight_Format);
    }

    // for (int i = 0 ; i < month_dates.size() ; i++)
    // {
    //     qDebug() << i+1 << ". " << month_dates[i].toString("dd-MM-yyyy");
    // }
}


void analysis::Update_Highlight_Year()
{
    year_dates = Get_Year_Dates(ui->calendar_start_date->selectedDate());

    // ลบ format เก่าทั้งหมดออก
    ui->calendar_start_date->setDateTextFormat(QDate(), QTextCharFormat());
    ui->calendar_end_date->setDateTextFormat(QDate(), QTextCharFormat());

    // สร้าง Format สำหรับไฮไลต์
    QTextCharFormat Highlight_Format;
    Highlight_Format.setBackground(Qt::green);
    Highlight_Format.setForeground(Qt::black);

    for (unsigned long int i = 0 ; i < year_dates.size() ; i++)
    {
        if (year_dates[i] != ui->calendar_start_date->selectedDate()) ui->calendar_start_date->setDateTextFormat(year_dates[i], Highlight_Format);
    }

    // for (int i = 0 ; i < year_dates.size() ; i++)
    // {
    //     qDebug() << i+1 << ". " << year_dates[i].toString("dd-MM-yyyy");
    // }
}


void analysis::on_calendar_start_date_selectionChanged()
{
    int searchmode = ui->comboBox_search_mode->currentIndex();
    if (searchmode == 0)
    {
        QDate startDate = ui->calendar_start_date->selectedDate();
        QDate endDate = ui->calendar_end_date->selectedDate();
        if (startDate > endDate) ui->calendar_end_date->setSelectedDate(startDate);
        Update_Selectable_and_Highlight_DateRange();
        Show_Chart();
    }
    else if (searchmode == 2)
    {
        Update_Highlight_Week();
        Show_Chart();
    }
    else if (searchmode == 3)
    {
        Update_Highlight_Month();
        Show_Chart();
    }
    else if (searchmode == 4)
    {
        Update_Highlight_Year();
        Show_Chart();
    }
    else Show_Chart();
}


void analysis::on_calendar_end_date_selectionChanged()
{
    int searchmode = ui->comboBox_search_mode->currentIndex();
    if (searchmode == 0)
    {
        QDate startDate = ui->calendar_start_date->selectedDate();
        QDate endDate = ui->calendar_end_date->selectedDate();
        if (startDate > endDate) ui->calendar_start_date->setSelectedDate(endDate);
        Update_Selectable_and_Highlight_DateRange();
        Show_Chart();
    }
}


void analysis::on_Refresh_clicked()
{
    Refresh_calendar();
    Refresh_calendar(); // ต้องมีสองอันเนื่องจากเมื่ออันแรกเปลี่ยนหน้าแล้วถ้าให้ mode : moth และ year อยู่จะทำให้ on_calendar_start_date_currentPageChanged(int year, int month)
                        // ทำงานและเปลี่ยนเป็นวันที่ 1 จึงต้อง Refresh_calendar(); อีกครั้งเพื่อให้วันที่ถูดต้อง

    int searchmode = ui->comboBox_search_mode->currentIndex();
    switch (searchmode) {
    case 0:
    case 1:
        Show_Chart();
        break;
    case 2:
        Update_Highlight_Week();
        Show_Chart();
        break;
    case 3:
        Update_Highlight_Month();
        Show_Chart();
        break;
    case 4:
        Update_Highlight_Year();
        Show_Chart();
        break;
    case 5:
        Show_Chart();
        break;
    }
}


void analysis::Refresh_calendar()
{
    ui->calendar_start_date->setSelectedDate(QDate::currentDate());
    ui->calendar_end_date->setSelectedDate(QDate::currentDate());

    ui->calendar_start_date->setMaximumDate(QDate::currentDate().addYears(1000));
    ui->calendar_end_date->setMinimumDate(QDate::currentDate().addYears(-1000));

    // ลบ format ทั้งหมดออก
    ui->calendar_start_date->setDateTextFormat(QDate(), QTextCharFormat());
    ui->calendar_end_date->setDateTextFormat(QDate(), QTextCharFormat());
}


void analysis::on_comboBox_search_mode_currentIndexChanged(int searchmode)
{
    switch (searchmode) {
    case 0:
        Refresh_calendar();
        ui->calendar_start_date->setEnabled(true);
        ui->calendar_end_date->setEnabled(true);
        Show_Chart();
        break;
    case 1:
        Refresh_calendar();
        ui->calendar_start_date->setEnabled(true);
        ui->calendar_end_date->setEnabled(false);
        Show_Chart();
        break;
    case 2:
        Refresh_calendar();
        ui->calendar_start_date->setEnabled(true);
        ui->calendar_end_date->setEnabled(false);
        Update_Highlight_Week();
        Show_Chart();
        break;
    case 3:
        Refresh_calendar();
        ui->calendar_start_date->setEnabled(true);
        ui->calendar_end_date->setEnabled(false);
        Update_Highlight_Month();
        Show_Chart();
        break;
    case 4:
        Refresh_calendar();
        ui->calendar_start_date->setEnabled(true);
        ui->calendar_end_date->setEnabled(false);
        Update_Highlight_Year();
        Show_Chart();
        break;
    case 5:
        Refresh_calendar();
        ui->calendar_start_date->setEnabled(false);
        ui->calendar_end_date->setEnabled(false);
        Show_Chart();
        break;
    }
}


vector<QDate> analysis::Get_Week_Dates(QDate selected_Date)
{
    vector<QDate> week_Dates;

    // หาววันแรกของสัปดา
    int day_Of_Week = selected_Date.dayOfWeek();  // 1 = จันทร์, 7 = อาทิตย์
    QDate day_start_Of_Week = selected_Date.addDays(-(day_Of_Week - 1));

    // เพิ่มวันทั้งอาทิตย์เข้า vector
    for (int i = 0; i < 7; ++i)
    {
        week_Dates.push_back(day_start_Of_Week.addDays(i));
    }

    return week_Dates;
}


vector<QDate> analysis::Get_Month_Dates(QDate selected_Date)
{
    vector<QDate> month_Dates;

    // หาววันแรกและวันสุดท้ายของเดือน
    QDate start_Of_Month(selected_Date.year(), selected_Date.month(), 1); // วันที่ 1 ของเดือนที่เลือก
    QDate end_Of_Month = start_Of_Month.addMonths(1).addDays(-1); // จะได้วันที่ 1 ของเดือนถัดไป และลบด้วย 1 วัน จะได้วันสุดท้ายของเดือนที่เลือก

     // เพิ่มวันทั้งเดือนเข้า vector
    for (QDate date = start_Of_Month; date <= end_Of_Month; date = date.addDays(1))
    {
        month_Dates.push_back(date);
    }

    return month_Dates;
}


vector<QDate> analysis::Get_Year_Dates(QDate selected_Date)
{
    vector<QDate> year_Dates;

    // หาววันแรกและวันสุดท้ายของปี
    QDate start_Of_year(selected_Date.year(), 1, 1); // วันที่ 1 ของปีที่เลือก
    QDate end_Of_year = start_Of_year.addYears(1).addDays(-1); // จะได้วันที่ 1 ของปีถัดไป และลบด้วย 1 วันจะได้วันสุดท้ายของของปีที่เลือก

    // เพิ่มวันทั้งเดือนเข้า vector
    for (QDate date = start_Of_year; date <= end_Of_year; date = date.addDays(1))
    {
        year_Dates.push_back(date);
    }

    return year_Dates;
}


void analysis::on_calendar_start_date_currentPageChanged(int year, int month)
{
    int searchmode = ui->comboBox_search_mode->currentIndex();
    switch (searchmode)
    {
    case 3:
    case 4:
        ui->calendar_start_date->setSelectedDate(QDate(year , month , 1));
        Show_Chart();
        break;
    }

    // Show_Chart(); //ถ้าให้โชว์ตรงนี้แล้วยังไม่เคยกดเข้า mode week แล้วจากเดือนหรือปีเปลี่ยนไปโหมด week Show_Chart(); จะถูดสั่งให้แสดงใน mode ของ week ก่อนที่จะมีการเก็บค่า week_dates ทำให้แสดงผลไม่ได้และ crashed
}


vector<QDate> analysis::Date_range_now()
{
    vector<QDate> Date_range(2);
    int searchmode = ui->comboBox_search_mode->currentIndex();
    switch (searchmode)
    {
    case 0:
        Date_range[0] = ui->calendar_start_date->selectedDate();
        Date_range[1] = ui->calendar_end_date->selectedDate();
        break;
    case 1:
        Date_range[0] = ui->calendar_start_date->selectedDate();
        Date_range[1] = Date_range[0];
        break;
    case 2:
        Date_range[0] = week_dates[0];
        Date_range[1] = week_dates[week_dates.size()-1];
        break;
    case 3:
        Date_range[0] = month_dates[0];
        Date_range[1] = month_dates[month_dates.size()-1];
        break;
    case 4:
        Date_range[0] = year_dates[0];
        Date_range[1] = year_dates[year_dates.size()-1];
        break;
    case 5:
        Date_range[0] = QDate::currentDate().addYears(-1000);
        Date_range[1] = QDate::currentDate().addYears(1000);
        break;
    }

    return Date_range;
}


void analysis::Show_Chart()
{
    ui->NO_DATA->hide();

    //==================================================================================//

    if (chartView != 0)
    {
        ui->Chart_Layout->removeWidget(chartView);
        delete chartView;
        chartView = 0;
    }

    //==================================================================================//

    vector<QDate> Date_Range_now = Date_range_now();
    QDate minDate_range = Date_Range_now[0];
    QDate maxDate_range = Date_Range_now[1];

    //==================================================================================//

    // Create a line series
    QLineSeries *series_income = new QLineSeries;
    QLineSeries *series_expenses = new QLineSeries;

    int add_count = 0;
    for (unsigned int i = 0; i < chartData_Date.size(); i++)
    {
        if (chartData_Date[i] < minDate_range or chartData_Date[i] > maxDate_range) continue;

        qint64 xValue = chartData_Date[i].startOfDay().toMSecsSinceEpoch();
        series_income->append(xValue, chartData_income[i]);
        series_expenses->append(xValue, chartData_expenses[i]);
        add_count++;
    }

    //==================================================================================//
    QPointF income_ONE_point , expenses_ONE_point;
    if (add_count == 1)
    {
        income_ONE_point = series_income->at(0);
        expenses_ONE_point = series_expenses->at(0);

        series_income->clear();
        series_income->append(income_ONE_point.x()+1 , income_ONE_point.y());
        series_income->append(income_ONE_point.x()+2 , income_ONE_point.y());

        series_expenses->clear();
        series_expenses->append(expenses_ONE_point.x()+1 , expenses_ONE_point.y());
        series_expenses->append(expenses_ONE_point.x()+2 , expenses_ONE_point.y());
    }

    //==================================================================================//

    QPen pen_income(Qt::green); //ตั้งค่าสีให้กับเส้นกราฟ
    pen_income.setWidth(2);  // ตั้งค่าความหนาของเส้นกราฟ
    series_income->setName("Income"); // ชื่อให้กับเส้นกราฟ
    series_income->setPen(pen_income); //ตั้งค่าเส้นของกราฟ income ให้เป็น pen_income ตามที่ตั้งค่าก่อนหน้า

    QPen pen_expenses(Qt::red); //ตั้งค่าสีให้กับเส้นกราฟ
    pen_expenses.setWidth(2);  // ตั้งค่าความหนาของเส้นกราฟ
    series_expenses->setName("expenses"); // ชื่อให้กับเส้นกราฟ
    series_expenses->setPen(pen_expenses); //ตั้งค่าเส้นของกราฟ income ให้เป็น pen_income ตามที่ตั้งค่าก่อนหน้า

    //==================================================================================//

    // Create a chart
    QChart *chart = new QChart;
    chart->addSeries(series_income); //เพิ่มเส้นกราฟ series_income ลงไปใน chart
    chart->addSeries(series_expenses); //เพิ่มเส้นกราฟ series_expenses ลงไปใน chart
    chart->setTitle("Analysis"); //ตั้งชื่อ chart

    //==================================================================================//

    //ตั้งค่าแต่ละแกนของกราฟ
    //chart->createDefaultAxes(); //เป็นฟังก์ชันใน Qt Charts ที่ใช้ สร้างแกน X และแกน Y โดยอัตโนมัติ ตามข้อมูลที่เพิ่มเข้าไปในกราฟ
    // ใช้ QDateTimeAxis สำหรับแกน X
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setFormat("dd MMM yy"); // ตั้งค่าการแสดงผลวันที่
    axisX->setTitleText("Date");
    if (add_count >= 10) axisX->setTickCount(10);
    else if (add_count < 10 and add_count > 1) axisX->setTickCount(add_count);
    else if (add_count == 1)
    {
        axisX->setRange(QDateTime::fromMSecsSinceEpoch(income_ONE_point.x()+1) , QDateTime::fromMSecsSinceEpoch(income_ONE_point.x()+2));
        axisX->setTickCount(2);
    }
    else
    {
        axisX->setRange(QDateTime(minDate_range , QTime(0,0,1)) , QDateTime(maxDate_range , QTime(0,0,2)));
        axisX->setTickCount(2);
    }


    \
    vector<long double> max_y(2);
    max_y.push_back(*max_element(chartData_income.begin() , chartData_income.end()));
    max_y.push_back(*max_element(chartData_expenses.begin() , chartData_expenses.end()));
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, *max_element(max_y.begin() , max_y.end()));
    axisY->setTickCount(10);
    axisY->setLabelFormat("%.2f");
    axisY->setTitleText("Amount");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series_income->attachAxis(axisX);
    series_income->attachAxis(axisY);
    series_expenses->attachAxis(axisX);
    series_expenses->attachAxis(axisY);

    //==================================================================================//

    // Create chart view
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing); //ทำให้เส้นคมชัดขึ้น

    //==================================================================================//

    // Add chart view to layout
    ui->Chart_Layout->addWidget(chartView);

    //==================================================================================//

    if (add_count == 0)
    {
        ui->NO_DATA->show();
    }

    //💓💓💓💓//
    Summary();
}


void analysis::on_accept_clicked()
{
    Show_Chart();
    Summary();
}


void analysis::Summary()
{
    //======================================================================================//

    vector<QDate> Date_Range_now = Date_range_now();
    QDate minDate_range = Date_Range_now[0];
    QDate maxDate_range = Date_Range_now[1];

    //======================================================================================//

    vector<Dishes> Dishes_data_in_range;
    vector<Drinks> Drinks_data_in_range;
    vector<QDate> Date_in_range;

    for (unsigned int i = 0; i < chartData_Date.size(); i++)
    {
        if (chartData_Date[i] < minDate_range or chartData_Date[i] > maxDate_range) continue;

        Date_in_range.push_back(chartData_Date[i]);
        Dishes_data_in_range.push_back(Dishes_data[i]);
        Drinks_data_in_range.push_back(Drinks_data[i]);
    }

    //======================================================================================//

    // qDebug() << "===============================";
    // for (unsigned int i = 0 ; i < Date_in_range.size() ; i++)
    // {
    //     qDebug() << "Date : "<< Date_in_range[i].toString("dd-MM-yyyy") ;

    //     qDebug() << "Dishes";
    //     for (int j = 0 ; j < Dishes_data_in_range[i].name.size() ; j++)
    //     {
    //         qDebug() << "name : " << Dishes_data_in_range[i].name[j] << "amount : " << Dishes_data_in_range[i].amount[j];
    //     }

    //     qDebug() << "Drinks";
    //     for (int j = 0 ; j < Drinks_data_in_range[i].name.size() ; j++)
    //     {
    //         qDebug() << "name : " << Drinks_data_in_range[i].name[j] << "amount : " << Drinks_data_in_range[i].amount[j];
    //     }

    //     qDebug() << "\n";
    // }

    //======================================================================================//

    Dishes count_Dishes_data_in_range = template_Dishes;
    Drinks count_Drinks_data_in_range = template_Drinks;

    for (unsigned int i = 0 ; i < Date_in_range.size() ; i++)
    {
        for (unsigned int j = 0 ; j < Dishes_data_in_range[i].name.size() ; j++)
        {
            string name_menu = Dishes_data_in_range[i].name[j];
            int amount = Dishes_data_in_range[i].amount[j];

            int index_in_count_Dishes = std::distance(count_Dishes_data_in_range.name.begin() , std::find(count_Dishes_data_in_range.name.begin() , count_Dishes_data_in_range.name.end() , name_menu));
            count_Dishes_data_in_range.amount[index_in_count_Dishes] += amount;
        }

        for (unsigned int j = 0 ; j < Drinks_data_in_range[i].name.size() ; j++)
        {
            string name_menu = Drinks_data_in_range[i].name[j];
            int amount = Drinks_data_in_range[i].amount[j];

            int index_in_count_Drinks = std::distance(count_Drinks_data_in_range.name.begin() , std::find(count_Drinks_data_in_range.name.begin() , count_Drinks_data_in_range.name.end() , name_menu));
            count_Drinks_data_in_range.amount[index_in_count_Drinks] += amount;
        }
    }

    //======================================================================================//

    // qDebug() << "\n\ncount_in_range";
    // qDebug() << "Dished";
    // for (int i = 0 ; i < count_Dishes_data_in_range.name.size() ; i++)
    // {
    //     qDebug() << "Name : " << count_Dishes_data_in_range.name[i] << " Amount : " << count_Dishes_data_in_range.amount[i];
    // }

    // qDebug() << "Drinks";
    // for (int i = 0 ; i < count_Drinks_data_in_range.name.size() ; i++)
    // {
    //     qDebug() << "Name : " << count_Drinks_data_in_range.name[i] << " Amount : " << count_Drinks_data_in_range.amount[i];
    // }

    //======================================================================================//
    // Sort count_data_in_range //

    for(unsigned int i = 0 ; i < count_Dishes_data_in_range.name.size() ; i++)
    {
        int max = count_Dishes_data_in_range.amount[i];
        int index_max = i;
        for (unsigned int j = i ; j < count_Dishes_data_in_range.name.size() ; j++)
        {
            if (count_Dishes_data_in_range.amount[j] > max)
            {
                max = count_Dishes_data_in_range.amount[j];
                index_max = j;
            }
        }

        // swap
        int amount1 = count_Dishes_data_in_range.amount[i];
        int amount2 = max;
        count_Dishes_data_in_range.amount[i] = amount2;
        count_Dishes_data_in_range.amount[index_max] = amount1;

        string name1 = count_Dishes_data_in_range.name[i];
        string name2 = count_Dishes_data_in_range.name[index_max];
        count_Dishes_data_in_range.name[i] = name2;
        count_Dishes_data_in_range.name[index_max] = name1;


    }

    for(unsigned int i = 0 ; i < count_Drinks_data_in_range.name.size() ; i++)
    {
        int max = count_Drinks_data_in_range.amount[i];
        int index_max = i;
        for (unsigned int j = i ; j < count_Drinks_data_in_range.name.size() ; j++)
        {
            if (count_Drinks_data_in_range.amount[j] > max)
            {
                max = count_Drinks_data_in_range.amount[j];
                index_max = j;
            }
        }

        // swap
        int amount1 = count_Drinks_data_in_range.amount[i];
        int amount2 = max;
        count_Drinks_data_in_range.amount[i] = amount2;
        count_Drinks_data_in_range.amount[index_max] = amount1;

        string name1 = count_Drinks_data_in_range.name[i];
        string name2 = count_Drinks_data_in_range.name[index_max];
        count_Drinks_data_in_range.name[i] = name2;
        count_Drinks_data_in_range.name[index_max] = name1;


    }

    //======================================================================================//

    qDebug() << "\n\ncount_in_range";
    qDebug() << "Dished";
    for (int i = 0 ; i < count_Dishes_data_in_range.name.size() ; i++)
    {
        qDebug() << "Name : " << count_Dishes_data_in_range.name[i] << " Amount : " << count_Dishes_data_in_range.amount[i];
    }

    qDebug() << "Drinks";
    for (int i = 0 ; i < count_Drinks_data_in_range.name.size() ; i++)
    {
        qDebug() << "Name : " << count_Drinks_data_in_range.name[i] << " Amount : " << count_Drinks_data_in_range.amount[i];
    }

    //======================================================================================//

    ui->Top_Dished->clearContents();
    for (int i = ui->Top_Dished->rowCount()-1 ; i >= 0  ; i--) ui->Top_Dished->removeRow(i);

    ui->Top_Drinks->clearContents();
    for (int i = ui->Top_Drinks->rowCount()-1 ; i >= 0  ; i--) ui->Top_Drinks->removeRow(i);

    for (unsigned int i = 0 ; i < count_Dishes_data_in_range.name.size() ; i++)
    {
        ui->Top_Dished->insertRow(ui->Top_Dished->rowCount());
        ui->Top_Dished->setItem(i , 0 , new QTableWidgetItem(QString::fromStdString(count_Dishes_data_in_range.name[i])));
        ui->Top_Dished->setItem(i , 1 , new QTableWidgetItem(QString::number(count_Dishes_data_in_range.amount[i])));
    }

    for (unsigned int i = 0 ; i < count_Drinks_data_in_range.name.size() ; i++)
    {
        ui->Top_Drinks->insertRow(ui->Top_Drinks->rowCount());
        ui->Top_Drinks->setItem(i , 0 , new QTableWidgetItem(QString::fromStdString(count_Drinks_data_in_range.name[i])));
        ui->Top_Drinks->setItem(i , 1 , new QTableWidgetItem(QString::number(count_Drinks_data_in_range.amount[i])));
    }

    //======================================================================================//
    //======================================================================================//
    //======================================================================================//
}
