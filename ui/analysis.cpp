#include "analysis.h"
#include "ui_analysis.h"
#include <header/json.h>
// #include <QtCore>
// #include <QtGui>
#include <QtCharts/QtCharts>
#include <QtCharts/QLineSeries>
#include <vector>
#include <algorithm>
#include <cmath>
#include <QTextFormat>
#include <QScatterSeries>

using namespace std;

analysis::analysis(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::analysis)
{
    ui->setupUi(this);
    this->setWindowTitle("Analysis");
    startUI_setup();
    Show_Chart();


    // ส่วนของการเล่นเสียงเมื่อมีการกดปุ่ม//
    AcceptSound = new QMediaPlayer(this);
    AcceptAudio = new QAudioOutput(this);
    AcceptSound->setAudioOutput(AcceptAudio);
    AcceptSound->setSource(QUrl("qrc:/Sounds/Accept.mp3"));

    buttonSound = new QMediaPlayer(this);
    buttonAudio = new QAudioOutput(this);
    buttonSound->setAudioOutput(buttonAudio);
    buttonSound->setSource(QUrl("qrc:/Sounds/Button.mp3"));

    OpenMenuSound = new QMediaPlayer(this);
    OpenMenuAudio = new QAudioOutput(this);
    OpenMenuSound->setAudioOutput(OpenMenuAudio);
    OpenMenuSound->setSource(QUrl("qrc:/Sounds/OpenCombobox.mp3"));

    SelectSound = new QMediaPlayer(this);
    SelectAudio = new QAudioOutput(this);
    SelectSound->setAudioOutput(SelectAudio);
    SelectSound->setSource(QUrl("qrc:/Sounds/Combobox_click.mp3"));

    connect(ui->comboBox_search_mode, &QComboBox::showPopup, this, &analysis::playOpenMenuSound);
    connect(ui->comboBox_search_mode, &QComboBox::activated, this, &analysis::playSelectSound);
    connect(ui->comboBox_Scale_mode, &QComboBox::showPopup, this, &analysis::playOpenMenuSound);
    connect(ui->comboBox_Scale_mode, &QComboBox::activated, this, &analysis::playSelectSound);
    // ส่วนของการเล่นเสียงเมื่อมีการกดปุ่ม//
}

analysis::~analysis()
{
    CloseUI();
    delete ui;
}

//=================== sound =====================//
void analysis::playAcceptSound()
{
    if (AcceptSound->playbackState() == QMediaPlayer::PlayingState) {
        AcceptSound->setPosition(0);
    } else {
        AcceptSound->play();
    }
}

void analysis::playButtonSound()
{
    if (buttonSound->playbackState() == QMediaPlayer::PlayingState) {
        buttonSound->setPosition(0);
    } else {
        buttonSound->play();
    }
}

void analysis::playSelectSound()
{
    if (SelectSound->playbackState() == QMediaPlayer::PlayingState) {
        SelectSound->setPosition(0);
    } else {
        SelectSound->play();
    }
}

void analysis::playOpenMenuSound()
{
    if (OpenMenuSound->playbackState() == QMediaPlayer::PlayingState) {
        OpenMenuSound->setPosition(0);
    } else {
        OpenMenuSound->play();
    }
}
//=================== sound =====================//



//vvvvvvvvvvvvvvvvvvvv------- Analysis -------vvvvvvvvvvvvvvvvvvvv//
//============================ struct ============================//

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

//=================== global variable =====================//

json menus , statement;

vector<Dishes> Dishes_data;
vector<Drinks> Drinks_data;

vector<string> all_Dishes_name;
vector<string> all_Drinks_name;

Dishes template_Dishes;
Drinks template_Drinks;

//=================== global variable =====================//

double Total_Income_in_selected_range = 0;
double Total_Expenses_in_selected_range = 0;

vector<double> chartData_income , chartData_expenses;
vector<QDateTime> chartData_Date;

vector<double> chartData_income_scaled_QDate , chartData_expenses_scaled_QDate;
vector<QDateTime> chartData_Date_scaled_QDate;

vector<QDate> week_dates , month_dates , year_dates;

QChartView *chartView;

//=================== global variable =====================//

void analysis::startUI_setup()
{
    chartView = 0; // chartView เป็นตัวแสดงกราฟ pointer ตั้งให้เป็น 0 คือกราฟยังไม่ถูกสร้าง
    Refresh_calendar(); // ทำการ Refresh ปฏิทินทั้งสองตัว

    ui->calendar_start_date->setEnabled(false); // ตั้งไม้ให้เลือกวันในปฏิทินเริ่มต้นได้
    ui->calendar_end_date->setEnabled(false); // ตั้งไม้ให้เลือกวันในปฏิทินสิ้นสุดได้

    ui->NO_DATA->hide(); // ซ่อนข้อความว่า "NO DATA"

    getData(menus , "Menus"); // ดึงข้อมูลเมนูจากไฟล์ json key "Menus" มาไว้ที่ตัวแปร menus
    getData(statement , "Statement"); // ดึงข้อมูลเมนูจากไฟล์ json key "Statement" มาไว้ที่ตัวแปร statement

    //========================================//

    ui->Top_Dished->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); // ทำให้ตาราง Top_Dished column 0 ขยายตามขนาดของข้อความ
    ui->Top_Dished->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // ทำให้ตาราง Top_Dished column 1 ขยายตามขนาดของข้อความ

    ui->Top_Drinks->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); // ทำให้ตาราง Top_Drinks column 0 ขยายตามขนาดของข้อความ
    ui->Top_Drinks->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // ทำให้ตาราง Top_Drinks column 1 ขยายตามขนาดของข้อความ

    //========================================//

    for (unsigned int i = 0 ; i < menus.size() ; i++) // วนลูปเพื่อเก็บชื่อเมนูอาหารทั้งหมดมาไว้ใน all_Dishes_name และเมนูเครื่องดื่มทั้งหมดมาไว้ใน all_Drinks_name
    {
        if (menus[i][2] == "Dishes") all_Dishes_name.push_back(menus[i][0]);
        else if (menus[i][2] == "Drinks") all_Drinks_name.push_back(menus[i][0]);
    }

    template_Dishes.name = all_Dishes_name; // กำหนดชื่อเมนูอาหารทั้งหมดให้กับ template_Dishes
    for(unsigned int i = 0 ; i < all_Dishes_name.size() ; i++) template_Dishes.amount.push_back(0); // กำหนดจำนวนเมนูอาหารทั้งหมดให้กับ template_Dishes ให้เป็น 0

    template_Drinks.name = all_Drinks_name; // กำหนดชื่อเมนูเครื่องดื่มทั้งหมดให้กับ template_Drinks
    for(unsigned int i = 0 ; i < all_Drinks_name.size() ; i++) template_Drinks.amount.push_back(0); // กำหนดจำนวนเมนูเครื่องดื่มทั้งหมดให้กับ template_Drinks ให้เป็น 0

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
    //อ่านข้อมูลจาก statement และนำข้อมูลมาเก็บไว้ในตัวแปร chartData_income , chartData_expenses , chartData_Date , Dishes_data , Drinks_data
    for (unsigned int i = 0 ; i < statement.size() ; i++)
    {
        QString String_DateTime = QString::fromStdString(statement[i][3]) + " " + QString::fromStdString(statement[i][4]); //แปลงข้อมูลวันเวลาจาก statement ให้เป็น QString เพื่อเอาไปแปลงเป็น QDateTime
        QDateTime Date_ = QDateTime::fromString(String_DateTime , "dd-MM-yyyy HH:mm:ss.zzz"); // ถ้าใช้เป็น "dd-MM-yyyy HH:mm:ss:ms" จะไม่สามารถทำงานได้เนื่องจากการเขียนที่ถูกคือ "dd-MM-yyyy HH:mm:ss.zzz" ✅✅✅

        auto it = std::find(chartData_Date.begin(), chartData_Date.end(), Date_); // หาว่าวันที่นี้มีข้อมูลอยู่แล้วหรือยัง
        if (it == chartData_Date.end()) // ถ้าไม่มีจะ == chartData_Date.end() หรือตัวสุดท้าย+1
        {
            chartData_Date.push_back(Date_); // ทำการเก็บวันที่ลงไปใน chartData_Date
            chartData_income.push_back(0); // ทำการเก็บรายได้ลงไปใน chartData_income โดยเริ่มต้นเป็น 0
            chartData_expenses.push_back(0); // ทำการเก็บรายจ่ายลงไปใน chartData_expenses โดยเริ่มต้นเป็น 0

            int newindex = chartData_Date.size() - 1; // หา index ของวันที่ที่เพิ่มเข้ามาใหม่

            if (statement[i][2] >= 0) chartData_income[newindex] += abs(double(statement[i][2])); // ถ้าจำนวนเงินมากกว่าหรือเท่ากับ 0 ให้เก็บข้อมูลรายได้ไว้ใน chartData_income (รายรับ)
            else chartData_expenses[newindex] += abs(double(statement[i][2])); // ถ้าจำนวนเงินน้อยกว่า 0 ให้เก็บข้อมูลรายจ่ายไว้ใน chartData_expenses (รายจ่าย)

            //^^^^^^^^^^^^^^^^^^^^^^^^ ส่วนของการเก็บข้อมูลมาไว้ใน chartData_Date , chartData_income , chartData_expenses ^^^^^^^^^^^^^^^^^^^^^^^^^^^//

            Dishes_data.push_back(template_Dishes); // ทำการเก็บข้อมูลเมนูอาหารที่ว่างไว้ไว้ใน Dishes_data
            Drinks_data.push_back(template_Drinks); // ทำการเก็บข้อมูลเมนูเครื่องดื่มที่ว่างไว้ไว้ใน Drinks_data

            string name_menu_i = statement[i][0]; // เก็บชื่อเมนูที่เพิ่มเข้ามา
            int amount_menu_i = statement[i][1]; // เก็บจำนวนเมนูที่เพิ่มเข้ามา

            if (statement[i][2] >= 0) // ให้ทำงานเมื่อเป็นรายรับ คือมีการสั่งอาหาร
            {
                if (std::find(Dishes_data[newindex].name.begin() , Dishes_data[newindex].name.end() , name_menu_i) != Dishes_data[newindex].name.end()) // หาว่าชื่อเมนูอาหารนี้มีอยู่ใน Dishes_data หรือไม่ ถ้าไม่แปลว่าเป็นเมนูเครื่องดื่มแล้วให้รัน else
                {
                    int index_menus = std::distance(Dishes_data[newindex].name.begin() , std::find(Dishes_data[newindex].name.begin() , Dishes_data[newindex].name.end() , name_menu_i)); // หา index ของเมนูอาหารนี้
                    Dishes_data[newindex].amount[index_menus] += amount_menu_i; // เพิ่มจำนวนเมนูอาหารนี้
                }
                else
                {
                    int index_menus = std::distance(Drinks_data[newindex].name.begin() , std::find(Drinks_data[newindex].name.begin() , Drinks_data[newindex].name.end() , name_menu_i)); // หา index ของเมนูเครื่องดื่มนี้
                    Drinks_data[newindex].amount[index_menus] += amount_menu_i; // เพิ่มจำนวนเมนูเครื่องดื่มนี้
                }
            }
            //^^^^^^^^^^^^^^^^^^^^^^^^ ส่วนของการเก็บข้อมูลมาไว้ใน Dishes_data , Drinks_data ^^^^^^^^^^^^^^^^^^^^^^^^^^^//
        }
        else // ถ้าเจอว่ามีข้อมูลเวลานั้นอยู่แล้ว หรือเป็นวันเวลาที่เคยเก็บข้อมูลไปแล้ว ให้ทำตามคำสั่งนี้
        {
            int index = std::distance(chartData_Date.begin() , it); // หา index ของวันที่นั้น

            if (statement[i][2] > 0) chartData_income[index] += abs(double(statement[i][2])); // ถ้าจำนวนเงินมากกว่า 0 ให้เก็บข้อมูลรายได้ไว้ใน chartData_income (รายรับ)
            else chartData_expenses[index] += abs(double(statement[i][2])); // ถ้าจำนวนเงินน้อยกว่าหรือเท่ากับ 0 ให้เก็บข้อมูลรายจ่ายไว้ใน chartData_expenses (รายจ่าย)

            //^^^^^^^^^^^^^^^^^^^^^^^^ ส่วนของการเก็บข้อมูลมาไว้ใน chartData_Date , chartData_income , chartData_expenses ^^^^^^^^^^^^^^^^^^^^^^^^^^^//

            string name_menu_i = statement[i][0]; // เก็บชื่อเมนูที่เพิ่มเข้ามา
            int amount_menu_i = statement[i][1]; // เก็บจำนวนเมนูที่เพิ่มเข้ามา

            if (statement[i][2] >= 0) // ให้ทำงานเมื่อเป็นรายรับ คือมีการสั่งอาหาร
            {
                if (std::find(Dishes_data[index].name.begin() , Dishes_data[index].name.end() , name_menu_i) != Dishes_data[index].name.end()) // หาว่าชื่อเมนูอาหารนี้มีอยู่ใน Dishes_data หรือไม่ ถ้าไม่แปลว่าเป็นเมนูเครื่องดื่มแล้วให้รัน else
                {
                    int index_menus = std::distance(Dishes_data[index].name.begin() , std::find(Dishes_data[index].name.begin() , Dishes_data[index].name.end() , name_menu_i)); // หา index ของเมนูอาหารนี้
                    Dishes_data[index].amount[index_menus] += amount_menu_i; // เพิ่มจำนวนเมนูอาหารนี้
                }
                else
                {
                    int index_menus = std::distance(Drinks_data[index].name.begin() , std::find(Drinks_data[index].name.begin() , Drinks_data[index].name.end() , name_menu_i)); // หา index ของเมนูเครื่องดื่มนี้
                    Drinks_data[index].amount[index_menus] += amount_menu_i; // เพิ่มจำนวนเมนูเครื่องดื่มนี้
                }
            }
            //^^^^^^^^^^^^^^^^^^^^^^^^ ส่วนของการเก็บข้อมูลมาไว้ใน Dishes_data , Drinks_data ^^^^^^^^^^^^^^^^^^^^^^^^^^^//
        }
    }

    //===================================================================//
    // ทำการเรียงข้อมูลหใหม่ ให้เรียงตามวันเวลา เพื่อกันข้อมูลใน statement มีข้อผิดพลาดในการเรียงลำดับเวลา //
    vector<QDateTime> unSort_chartData_Date = ::chartData_Date; // ทำการเก็บข้อมูล chartData_Date ไว้ใน unSort_chartData_Date
    std::sort(chartData_Date.begin() , chartData_Date.end()); // เรียงข้อมูลใน chartData_Date ใหม่
    vector<unsigned int> sorted_index; // สร้าง vector ไว้เก็บ index ของข้อมูลที่เรียงแล้ว

    for (unsigned int i = 0 ; i < unSort_chartData_Date.size() ; i++) // วนลูปเพื่อเรียงข้อมูลใหม่
    {
        if (std::find(sorted_index.begin() , sorted_index.end() , i) != sorted_index.end()) continue; // ถ้า index นี้เคยเรียงแล้วให้ข้ามไป

        unsigned int chnaged_index = std::distance(chartData_Date.begin() , std::find(chartData_Date.begin() , chartData_Date.end() , unSort_chartData_Date[i])); // หา index ว่าข้อมูลที่เรียงแล้วไปอยู่ตรงไหน
        if (chnaged_index != i) // ถ้า index ที่เรียงแล้วไม่ได้อยู่ตรงกับ index ก่อนหน้า ให้ทำการสลับข้อมูล
        {
            // qDebug() << QString::number(chnaged_index);

            // สลับข้อมูลของ chartData_income , chartData_expenses ตามวันที่ที่เรียงแล้วก่อนหน้า
            double swap_income_data = chartData_income[chnaged_index];
            chartData_income[chnaged_index] = chartData_income[i];
            chartData_income[i] = swap_income_data;

            double swap_expenses_data = chartData_expenses[chnaged_index];
            chartData_expenses[chnaged_index] = chartData_expenses[i];
            chartData_expenses[i] = swap_expenses_data;

            //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
            // สลับข้อมูลของ Dishes_data , Drinks_data ตามวันที่ที่เรียงแล้วก่อนหน้า
            Dishes swap_Dishes_data = Dishes_data[chnaged_index];
            Dishes_data[chnaged_index] = Dishes_data[i];
            Dishes_data[i] = swap_Dishes_data;

            Drinks swap_Drinks_data = Drinks_data[chnaged_index];
            Drinks_data[chnaged_index] = Drinks_data[i];
            Drinks_data[i] = swap_Drinks_data;

            //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//

            // เก็บ index ที่เรียงแล้วไว้
            sorted_index.push_back(i);
            sorted_index.push_back(chnaged_index);
        }
    }

    // for (unsigned int i = 0 ; i < chartData_Date.size() ; i++)
    // {
    //     qDebug() << "Date : "<< chartData_Date[i].toString("dd-MM-yyyy HH:mm:ss.zzz") << "Income : " << QString::number(chartData_income[i]) << "expenses : " << QString::number(chartData_expenses[i]);

    //     qDebug() << "Dishes";
    //     for (int j = 0 ; j < Dishes_data[i].name.size() ; j++)
    //     {
    //         qDebug() << "name : " << Dishes_data[i].name[j] << "amount : " << Dishes_data[i].amount[j];
    //     }

    //     qDebug() << "Drinks";
    //     for (int j = 0 ; j < Drinks_data[i].name.size() ; j++)
    //     {
    //         qDebug() << "name : " << Drinks_data[i].name[j] << "amount : " << Drinks_data[i].amount[j];
    //     }

    //     qDebug() << "\n";
    // }

    //===================================================================//
    // ข้อมูลที่เก็บไปก่อนหน้าจะเก็บข้อมูลละเอียดระดับ ms แต่ตอนแสดงกราฟหากมีข้อมูลมากเกินไปจะดูยาก จึงต้อง scale ความละเอียดของข้อมูลลงเป็นระดับวันที่ //
    vector<QDateTime> Un_mergeData_chartData_Date_scaled_QDate;

    for (unsigned int i = 0 ; i < chartData_Date.size() ; i++) // ลูปเท่ากับขนาดของ chartData_Date
    {
        Un_mergeData_chartData_Date_scaled_QDate.push_back(QDateTime(chartData_Date[i].date() , QTime(0,0,0,0))); // ทำการเก็บข้อมูลวันที่ของ chartData_Date และตัดส่วนของเวลาออกให้เป็น 00:00:00.000 ไว้ใน Un_mergeData_chartData_Date_scaled_QDate
    }

    for (unsigned int i = 0 ; i < Un_mergeData_chartData_Date_scaled_QDate.size() ; i++) // ลูปเท่ากับขนาดของ Un_mergeData_chartData_Date_scaled_QDate
    {
        QDateTime Date_ = Un_mergeData_chartData_Date_scaled_QDate[i]; // ถ้าใช้เป็น "dd-MM-yyyy HH:mm:ss:ms" จะไม่สามารถทำงานได้เนื่องจากการเขียนที่ถูกคือ "dd-MM-yyyy HH:mm:ss.zzz" ✅✅✅
                                                                        // เอาข้อมูลวันที่จาก Un_mergeData_chartData_Date_scaled_QDate มาเก็บไว้ใน Date_
        auto it = std::find(chartData_Date_scaled_QDate.begin(), chartData_Date_scaled_QDate.end(), Date_); // หาว่าวันที่นี้มีข้อมูลอยู่แล้วหรือยัง และเนื่องจากเราตัดส่วนของเวลา ออกไปแล้วข้อมูลที่ วันที่ตรงกันแต่เวลาไม่ตรงกันจะถือว่าเป็นข้อมูลเดียวกัน
        if (it == chartData_Date_scaled_QDate.end()) // ถ้าไม่มีจะ == chartData_Date_scaled_QDate.end() หรือตัวสุดท้าย+1
        {
            chartData_Date_scaled_QDate.push_back(Date_); // ทำการเก็บวันที่ลงไปใน chartData_Date_scaled_QDate
            chartData_income_scaled_QDate.push_back(0); // ทำการเก็บรายได้ลงไปใน chartData_income_scaled_QDate โดยเริ่มต้นเป็น 0
            chartData_expenses_scaled_QDate.push_back(0); // ทำการเก็บรายจ่ายลงไปใน chartData_expenses_scaled_QDate โดยเริ่มต้นเป็น 0

            int newindex = chartData_Date_scaled_QDate.size() - 1; // หา index ของวันที่ที่เพิ่มเข้ามาใหม่

            chartData_income_scaled_QDate[newindex] += abs(double(chartData_income[i])); // ทำการเก็บข้อมูลรายได้ไว้ใน chartData_income_scaled_QDate (รายรับ)
            chartData_expenses_scaled_QDate[newindex] += abs(double(chartData_expenses[i])); // ทำการเก็บข้อมูลรายจ่ายไว้ใน chartData_expenses_scaled_QDate (รายจ่าย)

        }
        else // ถ้าเจอว่ามีข้อมูลเวลานั้นอยู่แล้ว หรือเป็นวันเวลาที่เคยเก็บข้อมูลไปแล้ว ให้ทำตามคำสั่งนี้
        {
            int index = std::distance(chartData_Date_scaled_QDate.begin() , it); // หา index ของวันที่นั้น

            chartData_income_scaled_QDate[index] += abs(double(chartData_income[i])); // ทำการเก็บข้อมูลรายได้ไว้ใน chartData_income_scaled_QDate (รายรับ)
            chartData_expenses_scaled_QDate[index] += abs(double(chartData_expenses[i])); // ทำการเก็บข้อมูลรายจ่ายไว้ใน chartData_expenses_scaled_QDate (รายจ่าย)
        }
    }

    // for (unsigned int i = 0 ; i < chartData_Date_scaled_QDate.size() ; i++)
    // {
    //     qDebug() << "Date : "<< chartData_Date_scaled_QDate[i].toString("dd-MM-yyyy HH:mm:ss.zzz") << "Income : " << QString::number(chartData_income_scaled_QDate[i]) << "expenses : " << QString::number(chartData_expenses_scaled_QDate[i]);
    // }

    //===================================================================//

    ui->comboBox_search_mode->setCurrentIndex(5); // ตั้งให้ค่าเริ่มต้นของ search_mode เป็น search_mode ลำดับที่ 5 คือโหมด ALL //
}


void analysis::CloseUI() // เนื่องจากมีการ stack กันของข้อมูลเมื่อ ปิด เปิด UI Analysis จึงต้องมีการลบข้อมูลเก่าออกกทุกครั้งที่มีการปิดหน้า UI//
{
    Dishes_data.clear();
    Drinks_data.clear();

    all_Dishes_name.clear();
    all_Drinks_name.clear();

    chartData_income.clear();
    chartData_expenses.clear();
    chartData_Date.clear();

    chartData_income_scaled_QDate.clear();
    chartData_expenses_scaled_QDate.clear();
    chartData_Date_scaled_QDate.clear();

    week_dates.clear();
    month_dates.clear();
    year_dates.clear();

    delete chartView;
}


void analysis::Update_Selectable_and_Highlight_DateRange() // function สำหรับ limit วันมากสุดและน้อยสุดที่เลือกได้ และทำการ highlight วันที่อยู่ระหว่างวันที่เริ่มต้นและวันสิ้นสุด
{
    QDate startDate = ui->calendar_start_date->selectedDate(); // ดึงวันที่เริ่มต้นที่เลือกมา
    QDate endDate = ui->calendar_end_date->selectedDate(); // ดึงวันที่สิ้นสุดที่เลือกมา
    ui->calendar_start_date->setMaximumDate(endDate); // ตั้งให้วันที่เริ่มต้นสามารถเลือกได้มากสุดคือวันที่สิ้นสุด
    ui->calendar_end_date->setMinimumDate(startDate); // ตั้งให้วันที่สิ้นสุดสามารถเลือกได้น้อยสุดคือวันที่เริ่มต้น

    // ลบ format เก่าทั้งหมดออก
    ui->calendar_start_date->setDateTextFormat(QDate(), QTextCharFormat());
    ui->calendar_end_date->setDateTextFormat(QDate(), QTextCharFormat());

    // สร้าง Format สำหรับไฮไลต์
    QTextCharFormat Highlight_Range_Format;
    Highlight_Range_Format.setBackground(Qt::green); // ตั้งสีพื้นหลังเป็นสีเขียว
    Highlight_Range_Format.setForeground(Qt::black); // ตั้งสีตัวอักษรเป็นสีดำ

    for (QDate date = startDate.addDays(1) ; date <= endDate; date = date.addDays(1)) // วนลูปเพื่อทำการ highlight วันที่อยู่ระหว่างวันที่เริ่มต้นและวันที่สิ้นสุดทั้งสองปฏิทิน
    {
        ui->calendar_start_date->setDateTextFormat(date, Highlight_Range_Format); // ทำการ highlight วันที่อยู่ระหว่างวันที่เริ่มต้น+1 และวันที่สิ้นสุด
        ui->calendar_end_date->setDateTextFormat(date.addDays(-1), Highlight_Range_Format); // ทำการ highlight วันที่อยู่ระหว่างวันที่เริ่มต้น และวันที่สิ้นสุด-1
    }
}


void analysis::Update_Highlight_Week()
{
    week_dates = Get_Week_Dates(ui->calendar_start_date->selectedDate()); // จะได้ Vector ของวันที่ในสัปดาห์นั้นๆ มาทั้งหมด

    // ลบ format เก่าทั้งหมดออก
    ui->calendar_start_date->setDateTextFormat(QDate(), QTextCharFormat());
    ui->calendar_end_date->setDateTextFormat(QDate(), QTextCharFormat());

    // สร้าง Format สำหรับไฮไลต์
    QTextCharFormat Highlight_Format;
    Highlight_Format.setBackground(Qt::green); // ตั้งสีพื้นหลังเป็นสีเขียว
    Highlight_Format.setForeground(Qt::black); // ตั้งสีตัวอักษรเป็นสีดำ

    for (unsigned int i = 0 ; i < week_dates.size() ; i++) // วนลูปเพื่อทำการ highlight วันที่อยู่ในสัปดาห์นั้นๆ
    {
        if (week_dates[i] != ui->calendar_start_date->selectedDate()) ui->calendar_start_date->setDateTextFormat(week_dates[i], Highlight_Format); // ทำการ highlight วันที่อยู่ในสัปดาห์นั้นๆ โดยไม่ highlight วันที่เลือก
    }
}


void analysis::Update_Highlight_Month()
{
    month_dates = Get_Month_Dates(ui->calendar_start_date->selectedDate()); // จะได้ Vector ของวันที่ในเดือนนั้นๆ มาทั้งหมด

    // ลบ format เก่าทั้งหมดออก
    ui->calendar_start_date->setDateTextFormat(QDate(), QTextCharFormat());
    ui->calendar_end_date->setDateTextFormat(QDate(), QTextCharFormat());

    // สร้าง Format สำหรับไฮไลต์
    QTextCharFormat Highlight_Format;
    Highlight_Format.setBackground(Qt::green); // ตั้งสีพื้นหลังเป็นสีเขียว
    Highlight_Format.setForeground(Qt::black); // ตั้งสีตัวอักษรเป็นสีดำ

    for (unsigned long int i = 0 ; i < month_dates.size() ; i++) // วนลูปเพื่อทำการ highlight วันที่อยู่ในเดือนนั้นๆ
    {
        if (month_dates[i] != ui->calendar_start_date->selectedDate()) ui->calendar_start_date->setDateTextFormat(month_dates[i], Highlight_Format); // ทำการ highlight วันที่อยู่ในเดือนนั้นๆ โดยไม่ highlight วันที่เลือก
    }

    // for (int i = 0 ; i < month_dates.size() ; i++)
    // {
    //     qDebug() << i+1 << ". " << month_dates[i].toString("dd-MM-yyyy");
    // }
}


void analysis::Update_Highlight_Year()
{
    year_dates = Get_Year_Dates(ui->calendar_start_date->selectedDate()); // จะได้ Vector ของวันที่ในปีนั้นๆ มาทั้งหมด

    // ลบ format เก่าทั้งหมดออก
    ui->calendar_start_date->setDateTextFormat(QDate(), QTextCharFormat());
    ui->calendar_end_date->setDateTextFormat(QDate(), QTextCharFormat());

    // สร้าง Format สำหรับไฮไลต์
    QTextCharFormat Highlight_Format;
    Highlight_Format.setBackground(Qt::green); // ตั้งสีพื้นหลังเป็นสีเขียว
    Highlight_Format.setForeground(Qt::black); // ตั้งสีตัวอักษรเป็นสีดำ

    for (unsigned long int i = 0 ; i < year_dates.size() ; i++) // วนลูปเพื่อทำการ highlight วันที่อยู่ในปีนั้นๆ
    {
        if (year_dates[i] != ui->calendar_start_date->selectedDate()) ui->calendar_start_date->setDateTextFormat(year_dates[i], Highlight_Format); // ทำการ highlight วันที่อยู่ในปีนั้นๆ โดยไม่ highlight วันที่เลือก
    }

    // for (int i = 0 ; i < year_dates.size() ; i++)
    // {
    //     qDebug() << i+1 << ". " << year_dates[i].toString("dd-MM-yyyy");
    // }
}


void analysis::on_calendar_start_date_selectionChanged() // เมื่อปฏิทินวันที่เริ่มต้น(ปฏิทินด้านซ้ายน) มีการเลือกวันที่ใหม่
{
    int searchmode = ui->comboBox_search_mode->currentIndex(); // ดึงค่าของ search_mode ที่เลือกมา
    if (searchmode == 0) // ถ้า search_mode ที่เลือกคือ 0 คือ โหมด range
    {
        QDate startDate = ui->calendar_start_date->selectedDate(); // ดึงวันที่เริ่มต้นที่เลือกในปฏิทิน startDate มา
        QDate endDate = ui->calendar_end_date->selectedDate(); // ดึงวันที่สิ้นสุดที่เลือกในปฏิทิน endDate มา
        if (startDate > endDate) ui->calendar_end_date->setSelectedDate(startDate); // ถ้าวันที่เริ่มต้นมากกว่าวันที่สิ้นสุดให้ทำการเปลี่ยนวันที่สิ้นสุดให้เป็นวันที่เริ่มต้น
        Update_Selectable_and_Highlight_DateRange(); // ทำการเรียก function Update_Selectable_and_Highlight_DateRange() เพื่อทำการ limit วันมากสุดและน้อยสุดที่เลือกได้ และทำการ highlight วันที่อยู่ระหว่างวันที่เริ่มต้นและวันสิ้นสุด
    }
    else if (searchmode == 2) // ถ้า search_mode ที่เลือกคือ 2 คือ โหมด Week
    {
        Update_Highlight_Week(); // ทำการเรียก function Update_Highlight_Week() เพื่อทำการ highlight วันที่อยู่ในสัปดาห์นั้นๆ
    }
    else if (searchmode == 3) // ถ้า search_mode ที่เลือกคือ 3 คือ โหมด Month
    {
        Update_Highlight_Month(); // ทำการเรียก function Update_Highlight_Month() เพื่อทำการ highlight วันที่อยู่ในเดือนนั้นๆ
    }
    else if (searchmode == 4) // ถ้า search_mode ที่เลือกคือ 4 คือ โหมด Year
    {
        Update_Highlight_Year(); // ทำการเรียก function Update_Highlight_Year() เพื่อทำการ highlight วันที่อยู่ในปีนั้นๆ
    }

    Show_Chart(); // ทำการเรียก function Show_Chart() เพื่อทำการแสดงกราฟ
}


void analysis::on_calendar_end_date_selectionChanged() // เมื่อปฏิทินวันที่สิ้นสุด(ปฏิทินด้านขวา) มีการเลือกวันที่ใหม่ // ที่ปฏิทินวันที่สิ้นสุด(ปฏิทินด้านขวา) มีเงื่อนไขสำหรับโหมดเดียวเพราะ ปฏิทินด้านขวาจะกดเลือกไม่ได้เลยถ้าหากไม่ได้อยู่ในโหมด range
{
    int searchmode = ui->comboBox_search_mode->currentIndex(); // ดึงค่าของ search_mode ที่เลือกมา
    if (searchmode == 0) // ถ้า search_mode ที่เลือกคือ 0 คือ โหมด range
    {
        QDate startDate = ui->calendar_start_date->selectedDate(); // ดึงวันที่เริ่มต้นที่เลือกในปฏิทิน startDate มา
        QDate endDate = ui->calendar_end_date->selectedDate(); // ดึงวันที่สิ้นสุดที่เลือกในปฏิทิน endDate มา
        if (startDate > endDate) ui->calendar_start_date->setSelectedDate(endDate); // ถ้าวันที่เริ่มต้นมากกว่าวันที่สิ้นสุดให้ทำการเปลี่ยนวันที่เริ่มต้นให้เป็นวันที่สิ้นสุด
        Update_Selectable_and_Highlight_DateRange(); // ทำการเรียก function Update_Selectable_and_Highlight_DateRange() เพื่อทำการ limit วันมากสุดและน้อยสุดที่เลือกได้ และทำการ highlight วันที่อยู่ระหว่างวันที่เริ่มต้นและวันสิ้นสุด
        Show_Chart(); // ทำการเรียก function Show_Chart() เพื่อทำการแสดงกราฟ
    }
}


void analysis::on_Refresh_clicked() // เมื่อมีการกดปุ่ม Refresh
{
    playButtonSound(); // เล่นเสียงปุ่ม
    Refresh_calendar();
    Refresh_calendar(); // ต้องมีสองอันเนื่องจากเมื่ออันแรกเปลี่ยนหน้าแล้วถ้าให้ mode : moth และ year อยู่จะทำให้ on_calendar_start_date_currentPageChanged(int year, int month)
                        // ทำงานและเปลี่ยนเป็นวันที่ 1 จึงต้อง Refresh_calendar(); อีกครั้งเพื่อให้วันที่ถูดต้อง

    int searchmode = ui->comboBox_search_mode->currentIndex();
    switch (searchmode) {
    case 0:
    case 1:
        Show_Chart(); // ทำการเรียก function Show_Chart() เพื่อทำการแสดงกราฟ
        break;
    case 2:
        Update_Highlight_Week(); // ทำการเรียก function Update_Highlight_Week() เพื่อทำการ highlight วันที่อยู่ในสัปดาห์นั้นๆ
        Show_Chart();
        break;
    case 3:
        Update_Highlight_Month(); // ทำการเรียก function Update_Highlight_Month() เพื่อทำการ highlight วันที่อยู่ในเดือนนั้นๆ
        Show_Chart();
        break;
    case 4:
        Update_Highlight_Year(); // ทำการเรียก function Update_Highlight_Year() เพื่อทำการ highlight วันที่อยู่ในปีนั้นๆ
        Show_Chart();
        break;
    case 5:
        Show_Chart();
        break;
    }
}


void analysis::Refresh_calendar()
{
    ui->calendar_start_date->setSelectedDate(QDate::currentDate()); // ทำการเซ็ตวันที่เริ่มต้นให้เป็นวันปัจจุบัน
    ui->calendar_end_date->setSelectedDate(QDate::currentDate()); // ทำการเซ็ตวันที่สิ้นสุดให้เป็นวันปัจจุบัน

    ui->calendar_start_date->setMaximumDate(QDate::currentDate().addYears(1000)); // ทำการเซ็ตวันที่เริ่มต้นให้สามารถเลือกได้มากสุดคือวันปัจจุบัน+1000 ปี
    ui->calendar_end_date->setMinimumDate(QDate::currentDate().addYears(-1000)); // ทำการเซ็ตวันที่สิ้นสุดให้สามารถเลือกได้น้อยสุดคือวันปัจจุบัน-1000 ปี

    // ลบ format ทั้งหมดออก
    ui->calendar_start_date->setDateTextFormat(QDate(), QTextCharFormat());
    ui->calendar_end_date->setDateTextFormat(QDate(), QTextCharFormat());
}


void analysis::on_comboBox_search_mode_currentIndexChanged(int searchmode) // function ที่จะทำงานเมื่อมีการเลือกโหมดการค้นหาใหม่
{
    //Refresh_calendar // แต่ถ้าเป็นการเปลี่ยนโหมดจะไม่มีการเปลี่ยนวันที่ที่เลือกไปเป็นวันปัจจุบัน
    // ลบ format ทั้งหมดออก
    ui->calendar_start_date->setDateTextFormat(QDate(), QTextCharFormat());
    ui->calendar_end_date->setDateTextFormat(QDate(), QTextCharFormat());

    switch (searchmode)
    {
    case 0:
        ui->calendar_end_date->setSelectedDate(ui->calendar_start_date->selectedDate()); // ทำการเซ็ตวันที่สิ้นสุดให้เป็นวันที่เริ่มต้น

        ui->calendar_start_date->setEnabled(true); // ตั้งค่าให้ปฏิทินวันที่เริ่มต้นเป็นสามารถใช้งานได้
        ui->calendar_end_date->setEnabled(true);  // ตั้งค่าให้ปฏิทินวันที่สิ้นสุดเป็นสามารถใช้งานได้
        break;
    case 1:
        ui->calendar_start_date->setEnabled(true); // ตั้งค่าให้ปฏิทินวันที่เริ่มต้นเป็นสามารถใช้งานได้
        ui->calendar_end_date->setEnabled(false); // ตั้งค่าให้ปฏิทินวันที่สิ้นสุดเป็นไม่สามารถใช้งานได้
        break;
    case 2:
        ui->calendar_start_date->setEnabled(true); // ตั้งค่าให้ปฏิทินวันที่เริ่มต้นเป็นสามารถใช้งานได้
        ui->calendar_end_date->setEnabled(false); // ตั้งค่าให้ปฏิทินวันที่สิ้นสุดเป็นไม่สามารถใช้งานได้
        Update_Highlight_Week(); // ทำการเรียก function Update_Highlight_Week() เพื่อทำการ highlight วันที่อยู่ในสัปดาห์นั้นๆ
        break;
    case 3:
        ui->calendar_start_date->setEnabled(true); // ตั้งค่าให้ปฏิทินวันที่เริ่มต้นเป็นสามารถใช้งานได้
        ui->calendar_end_date->setEnabled(false); // ตั้งค่าให้ปฏิทินวันที่สิ้นสุดเป็นไม่สามารถใช้งานได้
        Update_Highlight_Month(); // ทำการเรียก function Update_Highlight_Month() เพื่อทำการ highlight วันที่อยู่ในเดือนนั้นๆ
        break;
    case 4:
        ui->calendar_start_date->setEnabled(true); // ตั้งค่าให้ปฏิทินวันที่เริ่มต้นเป็นสามารถใช้งานได้
        ui->calendar_end_date->setEnabled(false); // ตั้งค่าให้ปฏิทินวันที่สิ้นสุดเป็นไม่สามารถใช้งานได้
        Update_Highlight_Year(); // ทำการเรียก function Update_Highlight_Year() เพื่อทำการ highlight วันที่อยู่ในปีนั้นๆ
        break;
    case 5:
        ui->calendar_start_date->setEnabled(false); // ตั้งค่าให้ปฏิทินวันที่เริ่มต้นเป็นไม่สามารถใช้งานได้
        ui->calendar_end_date->setEnabled(false); // ตั้งค่าให้ปฏิทินวันที่สิ้นสุดเป็นไม่สามารถใช้งานได้
        break;
    }

    ui->calendar_start_date->setMaximumDate(QDate::currentDate().addYears(1000)); // ทำการเซ็ตวันที่เริ่มต้นให้สามารถเลือกได้มากสุดคือวันปัจจุบัน+1000 ปี
    ui->calendar_end_date->setMinimumDate(QDate::currentDate().addYears(-1000)); // ทำการเซ็ตวันที่สิ้นสุดให้สามารถเลือกได้น้อยสุดคือวันปัจจุบัน-1000 ปี


    Show_Chart();
}


vector<QDate> analysis::Get_Week_Dates(QDate selected_Date)
{
    vector<QDate> week_Dates; // สร้าง vector ของ QDate ชื่อ week_Dates

    // หาววันแรกของสัปดา
    int day_Of_Week = selected_Date.dayOfWeek();  // 1 = จันทร์, 7 = อาทิตย์
    QDate day_start_Of_Week = selected_Date.addDays(-(day_Of_Week - 1)); // หาวันที่เริ่มต้นของสัปดาห์

    // เพิ่มวันทั้งอาทิตย์เข้า vector
    for (int i = 0; i < 7; ++i) // วนลูป 7 รอบเพื่อเพิ่มวันทั้งสัปดาห์เข้า vector
    {
        week_Dates.push_back(day_start_Of_Week.addDays(i)); // เพิ่มวันที่เริ่มต้นของสัปดาห์ + i วันเข้า vector
    }

    return week_Dates; // ส่งค่า vector ของวันที่ในสัปดาห์นั้นๆ ออกไป
}


vector<QDate> analysis::Get_Month_Dates(QDate selected_Date)
{
    vector<QDate> month_Dates; // สร้าง vector ของ QDate ชื่อ month_Dates

    // หาววันแรกและวันสุดท้ายของเดือน
    QDate start_Of_Month(selected_Date.year(), selected_Date.month(), 1); // วันที่ 1 ของเดือนที่เลือก
    QDate end_Of_Month = start_Of_Month.addMonths(1).addDays(-1); // จะได้วันที่ 1 ของเดือนถัดไป และลบด้วย 1 วัน จะได้วันสุดท้ายของเดือนที่เลือก

     // เพิ่มวันทั้งเดือนเข้า vector
    for (QDate date = start_Of_Month; date <= end_Of_Month; date = date.addDays(1)) // วนลูปจนกว่าวันที่จะเป็นวันสุดท้ายของเดือน และเพิ่มวันทั้งเดือนเข้า vector
    {
        month_Dates.push_back(date); // เพิ่มวันที่เป็นวันทั้งเดือนเข้า vector
    }

    return month_Dates; // ส่งค่า vector ของวันที่ในเดือนนั้นๆ ออกไป
}


vector<QDate> analysis::Get_Year_Dates(QDate selected_Date)
{
    vector<QDate> year_Dates; // สร้าง vector ของ QDate ชื่อ year_Dates

    // หาววันแรกและวันสุดท้ายของปี
    QDate start_Of_year(selected_Date.year(), 1, 1); // วันที่ 1 ของปีที่เลือก
    QDate end_Of_year = start_Of_year.addYears(1).addDays(-1); // จะได้วันที่ 1 ของปีถัดไป และลบด้วย 1 วันจะได้วันสุดท้ายของของปีที่เลือก

    // เพิ่มวันทั้งเดือนเข้า vector
    for (QDate date = start_Of_year; date <= end_Of_year; date = date.addDays(1)) // วนลูปจนกว่าวันที่จะเป็นวันสุดท้ายของปี และเพิ่มวันทั้งปีเข้า vector
    {
        year_Dates.push_back(date); // เพิ่มวันที่เป็นวันทั้งปีเข้า vector
    }

    return year_Dates; // ส่งค่า vector ของวันที่ในปีนั้นๆ ออกไป
}


void analysis::on_calendar_start_date_currentPageChanged(int year, int month) // ทำงานเมื่อหน้าปฏิทินเปลี่ยน
{
    int searchmode = ui->comboBox_search_mode->currentIndex(); // ดึงค่าของ search_mode ที่เลือกมา
    switch (searchmode) // ทำงานเมื่อ search_mode ที่เลือกมีค่าเป็น 3 , 4 คือ โหมด Month และ Year
    {
    case 3:
    case 4:
        ui->calendar_start_date->setSelectedDate(QDate(year , month , 1)); // ทำการเซ็ตวันที่เริ่มต้นให้เป็นวันที่ 1 ของเดือนที่เลือก // โดย function นี้จะช่วยให้ทุกครั้งที่ scroll เปลี่ยนหน้าปฏิทินจะตั้งเป็นวันที่ 1 ของเดือนนั้นๆ ให้เองเลย
        Show_Chart(); // ทำการเรียก function Show_Chart() เพื่อทำการแสดงกราฟ
        break;
    }

    // Show_Chart(); //ถ้าให้โชว์ตรงนี้แล้วยังไม่เคยกดเข้า mode week แล้วจากเดือนหรือปีเปลี่ยนไปโหมด week Show_Chart(); จะถูดสั่งให้แสดงใน mode ของ week ก่อนที่จะมีการเก็บค่า week_dates ทำให้แสดงผลไม่ได้และ crashed
}


vector<QDate> analysis::Date_range_now()
{
    vector<QDate> Date_range(2); // สร้าง vector ของ QDate ชื่อ Date_range มีขนาด 2 ช่อง
    int searchmode = ui->comboBox_search_mode->currentIndex(); // ดึงค่าของ search_mode ที่เลือกมา
    switch (searchmode)
    {
    case 0: // ถ้า search_mode ที่เลือกคือ 0 คือ โหมด range
        Date_range[0] = ui->calendar_start_date->selectedDate(); // วันที่เริ่มต้นที่เลือกคือวันในปฏิทินด้านซ้าย
        Date_range[1] = ui->calendar_end_date->selectedDate(); // วันที่สิ้นสุดที่เลือกคือวันในปฏิทินด้านขวา
        break;
    case 1: // ถ้า search_mode ที่เลือกคือ 1 คือ โหมด day
        Date_range[0] = ui->calendar_start_date->selectedDate(); // วันที่เริ่มต้นที่เลือกคือวันในปฏิทินด้านซ้าย
        Date_range[1] = Date_range[0]; // วันที่สิ้นสุดที่เลือกคือวันที่เริ่มต้น
        break;
    case 2: // ถ้า search_mode ที่เลือกคือ 2 คือ โหมด week
        Date_range[0] = week_dates[0]; // วันที่เริ่มต้นที่เลือกคือวันแรกของสัปดาห์
        Date_range[1] = week_dates[week_dates.size()-1]; // วันที่สิ้นสุดที่เลือกคือวันสุดท้ายของสัปดาห์
        break;
    case 3: // ถ้า search_mode ที่เลือกคือ 3 คือ โหมด month
        Date_range[0] = month_dates[0]; // วันที่เริ่มต้นที่เลือกคือวันแรกของเดือน
        Date_range[1] = month_dates[month_dates.size()-1]; // วันที่สิ้นสุดที่เลือกคือวันสุดท้ายของเดือน
        break;
    case 4: // ถ้า search_mode ที่เลือกคือ 4 คือ โหมด year
        Date_range[0] = year_dates[0]; // วันที่เริ่มต้นที่เลือกคือวันแรกของปี
        Date_range[1] = year_dates[year_dates.size()-1]; // วันที่สิ้นสุดที่เลือกคือวันสุดท้ายของปี
        break;
    case 5: // ถ้า search_mode ที่เลือกคือ 5 คือ โหมด all
        Date_range[0] = QDate::currentDate().addYears(-1000); // วันที่เริ่มต้นที่เลือกคือวันปัจจุบัน-1000 ปี
        Date_range[1] = QDate::currentDate().addYears(1000); // วันที่สิ้นสุดที่เลือกคือวันปัจจุบัน+1000 ปี
        break;
    }

    return Date_range; // ส่งค่า vector ของวันที่เริ่มต้นและสิ้นสุดออกไป
}


void analysis::Show_Chart()
{
    ui->NO_DATA->hide(); // ซ่อน label ที่บอกว่าไม่มีข้อมูล

    //==================================================================================//

    Total_Income_in_selected_range = 0; // ตั้งค่าให้ Total_Income_in_selected_range เป็น 0 สำหรับนับรวมรายรับในช่วงที่เลือก
    Total_Expenses_in_selected_range = 0; // ตั้งค่าให้ Total_Expenses_in_selected_range เป็น 0 สำหรับนับรวมรายจ่ายในช่วงที่เลือก

    //==================================================================================//

    if (chartView != 0) // ถ้ามี chartView อยู่แล้ว
    {
        ui->Chart_Layout->removeWidget(chartView); // ทำการลบ chartView ออกจาก layout
        delete chartView; // ทำการลบ chartView
        chartView = 0; // ทำการเซ็ตค่า pointer chartView เป็น 0
    }

    //==================================================================================//

    vector<QDate> Date_Range_now = Date_range_now(); // ดึงวันที่เริ่มต้นและสิ้นสุดของช่วงที่เลือกมา
    QDateTime minDate_range = Date_Range_now[0].startOfDay(); // ทำการเซ็ตเวลาเริ่มต้นของวันที่เริ่มต้นให้เป็นเวลา 00:00:00 เพื่อแปลงจาก QDate เป็น QDateTime
    QDateTime maxDate_range = Date_Range_now[1].endOfDay(); // ทำการเซ็ตเวลาสิ้นสุดของวันที่สิ้นสุดให้เป็นเวลา 23:59:59  เพื่อแปลงจาก QDate เป็น QDateTime

    int count_Day_in_range = Date_Range_now[0].daysTo(Date_Range_now[1]); // นับจำนวนวันในช่วงที่เลือก

    //==================================================================================//

    vector<QDateTime> chartData_Date_For_CreateChart; // สร้าง vector ของ QDateTime ชื่อ chartData_Date_For_CreateChart เพื่อเก็บวันที่ที่จะนำมาสร้างกราฟ
    vector<double> chartData_income_For_CreateChart; // สร้าง vector ของ double ชื่อ chartData_income_For_CreateChart เพื่อเก็บรายรับที่จะนำมาสร้างกราฟ
    vector<double> chartData_expenses_For_CreateChart; // สร้าง vector ของ double ชื่อ chartData_expenses_For_CreateChart เพื่อเก็บรายจ่ายที่จะนำมาสร้างกราฟ

    bool Scale_logic; // สร้างตัวแปร Scale_logic เพื่อเก็บค่าว่าจะทำการ scale หรือไม่
    switch (ui->comboBox_Scale_mode->currentIndex()) // ดึงค่าของ Scale_mode ที่เลือกมา
    {
        case 0: // ถ้า Scale_mode ที่เลือกคือ 0 คือ โหมด auto
            Scale_logic = count_Day_in_range >= 6; // ถ้าจำนวนวันในช่วงที่เลือกมามากกว่าหรือเท่ากับ 7 วัน จะทำการ scale
            break;
        case 1:
            Scale_logic = true; // ถ้า Scale_mode ที่เลือกคือ 1 คือ โหมด scale จะทำการ scale
            break;
        case 2:
            Scale_logic = false; // ถ้า Scale_mode ที่เลือกคือ 2 คือ โหมด un-scale จะไม่ทำการ scale
            break;
    }

    if (Scale_logic) // ถ้า Scale_logic เป็น true จะทำการ scale
    {
        chartData_Date_For_CreateChart = chartData_Date_scaled_QDate; // ทำการเซ็ตค่าของ chartData_Date_For_CreateChart ให้เป็น chartData_Date_scaled_QDate ที่เป็น vector ของ QDateTime ที่ scale แล้ว
        chartData_income_For_CreateChart = chartData_income_scaled_QDate; // ทำการเซ็ตค่าของ chartData_income_For_CreateChart ให้เป็น chartData_income_scaled_QDate ที่เป็น vector ของ double ที่ scale แล้ว
        chartData_expenses_For_CreateChart = chartData_expenses_scaled_QDate; // ทำการเซ็ตค่าของ chartData_expenses_For_CreateChart ให้เป็น chartData_expenses_scaled_QDate ที่เป็น vector ของ double ที่ scale แล้ว
    }
    else
    {
        chartData_Date_For_CreateChart = chartData_Date; // ทำการเซ็ตค่าของ chartData_Date_For_CreateChart ให้เป็น chartData_Date ที่เป็น vector ของ QDateTime ที่ยังไม่ได้ scale
        chartData_income_For_CreateChart = chartData_income; // ทำการเซ็ตค่าของ chartData_income_For_CreateChart ให้เป็น chartData_income ที่เป็น vector ของ double ที่ยังไม่ได้ scale
        chartData_expenses_For_CreateChart = chartData_expenses; // ทำการเซ็ตค่าของ chartData_expenses_For_CreateChart ให้เป็น chartData_expenses ที่เป็น vector ของ double ที่ยังไม่ได้ scale
    }

    //==================================================================================//

    // Create a line series
    QLineSeries *series_income = new QLineSeries; // กราฟเส้น รายรับ
    QLineSeries *series_expenses = new QLineSeries; // กราฟเส้น รายจ่าย
    QScatterSeries *Dot_series_income = new QScatterSeries; // กราฟจุด รายรับ
    QScatterSeries *Dot_series_expenses = new QScatterSeries; // กราฟจุด รายจ่าย

    int add_count = 0; // ตัวแปรนับจำนวนวันที่เพิ่มเข้าไปในกราฟ
    for (unsigned int i = 0; i < chartData_Date_For_CreateChart.size(); i++) // วนลูปเพื่อเพิ่มข้อมูลในกราฟ
    {
        if (chartData_Date_For_CreateChart[i] < minDate_range or chartData_Date_For_CreateChart[i] > maxDate_range) continue; // ถ้าวันที่ไม่อยู่ในช่วงที่เลือก จะข้ามไป

        //---------------total-----------------//
        Total_Income_in_selected_range += chartData_income_For_CreateChart[i]; // นับรวมรายรับรวมในช่วงที่เลือก
        Total_Expenses_in_selected_range += chartData_expenses_For_CreateChart[i]; // นับรวมรายจ่ายรวมในช่วงที่เลือก
        //---------------total-----------------//

        qint64 xValue = chartData_Date_For_CreateChart[i].toMSecsSinceEpoch(); // แปลง QDateTime เป็น qint64 เพื่อให้สามารถใช้เป็นแกน x ในกราฟได้ แปลงให้อยู่ในรูปของ millisecond จุดเริ่มต้นคือ 1 มกราคม 1970 00:00:00 UTC

        //---------------line series-----------------//
        series_income->append(xValue, chartData_income_For_CreateChart[i]); // เพิ่มข้อมูลรายรับลง series_income โดย series จะเป็นข้อมูลแบบคู่อันดับ x y โดยแกน x คือเวลาและแกน y คือ จำนวนเงิน
        series_expenses->append(xValue, chartData_expenses_For_CreateChart[i]); // เพิ่มข้อมูลรายจ่ายลง series_expenses โดย series จะเป็นข้อมูลแบบคู่อันดับ x y โดยแกน x คือเวลาและแกน y คือ จำนวนเงิน
        //---------------line series-----------------//

        //---------------Dot series-----------------//
        if (chartData_income_For_CreateChart[i] != 0) Dot_series_income->append(xValue, chartData_income_For_CreateChart[i]); // ถ้ารายรับไม่เท่ากับ 0 จะเพิ่มจุดลงใน Dot_series_income โดย series จะเป็นข้อมูลแบบคู่อันดับ x y โดยแกน x คือเวลาและแกน y คือ จำนวนเงินแต่อันนี้เป็นข้อมูลสำหรับกราฟแบบจุด
        if (chartData_expenses_For_CreateChart[i] != 0) Dot_series_expenses->append(xValue, chartData_expenses_For_CreateChart[i]); // ถ้ารายจ่ายไม่เท่ากับ 0 จะเพิ่มจุดลงใน Dot_series_expenses โดย series จะเป็นข้อมูลแบบคู่อันดับ x y โดยแกน x คือเวลาและแกน y คือ จำนวนเงินแต่อันนี้เป็นข้อมูลสำหรับกราฟแบบจุด
        //---------------Dot series-----------------//
        
        add_count++; // นับจำนวนวันที่เพิ่มเข้าไปในกราฟ
    }

    //==================================================================================//
    // qDebug() << add_count;

    QPointF income_ONE_point , expenses_ONE_point; // สร้างตัวแปร QPointF ชื่อ income_ONE_point , expenses_ONE_point เพื่อเก็บจุดของรายรับและรายจ่ายในวันที่เดียวกัน QPointF เป็นตัวแปรที่เก็บค่าคู่อันดับ x และ y ไว้ในตัวแปรเดียวกัน
    if (add_count == 1) // ถ้ามีวันที่เพิ่มเข้าไปในกราฟเพียง 1 วัน (มีข้อมูลเพียงวันเดียว)
    {
        income_ONE_point = series_income->at(0); // นำจุดของรายรับในวันที่เดียวกันมาเก็บไว้ใน income_ONE_point
        expenses_ONE_point = series_expenses->at(0); // นำจุดของรายจ่ายในวันที่เดียวกันมาเก็บไว้ใน expenses_ONE_point

        series_income->clear(); // ลบข้อมูลใน series_income ทั้งหมด
        series_income->append(QDateTime::fromMSecsSinceEpoch(income_ONE_point.x()).date().startOfDay().toMSecsSinceEpoch()  , 0); // เพิ่มจุดเริ่มต้นของกราฟรายรับให้แกน y เป็น 0
        series_income->append(income_ONE_point.x() , income_ONE_point.y()); // เพิ่มจุดของรายรับให้แกน y เป็นจำนวนเงิน
        series_income->append(QDateTime::fromMSecsSinceEpoch(income_ONE_point.x()).date().endOfDay().toMSecsSinceEpoch() , 0); // เพิ่มจุดสุดท้ายของกราฟรายรับให้แกน y เป็น 0

        series_expenses->clear(); // ลบข้อมูลใน series_expenses ทั้งหมด
        series_expenses->append(QDateTime::fromMSecsSinceEpoch(expenses_ONE_point.x()).date().startOfDay().toMSecsSinceEpoch() , 0); // เพิ่มจุดเริ่มต้นของกราฟรายจ่ายให้แกน y เป็น 0
        series_expenses->append(expenses_ONE_point.x() , expenses_ONE_point.y()); // เพิ่มจุดของรายจ่ายให้แกน y เป็นจำนวนเงิน
        series_expenses->append(QDateTime::fromMSecsSinceEpoch(expenses_ONE_point.x()).date().endOfDay().toMSecsSinceEpoch() , 0); // เพิ่มจุดสุดท้ายของกราฟรายจ่ายให้แกน y เป็น 0
    }

    // ที่ต้องเพิ่มข้อมูลแบบนี้เพราะถ้ามีข้อมูลเพียงวันเดียวใน series จะไม่สามารถสร้างกราฟได้

    //==========Line===========//
    //==================================================================================//

    QPen pen_income(Qt::green); //ตั้งค่าสีให้กับเส้นกราฟ
    pen_income.setWidth(2);  // ตั้งค่าความหนาของเส้นกราฟ
    series_income->setName("Income"); // ชื่อให้กับเส้นกราฟ
    series_income->setPen(pen_income); //ตั้งค่าเส้นของกราฟ income ให้เป็น pen_income ตามที่ตั้งค่าก่อนหน้า

    QPen pen_expenses(Qt::red); //ตั้งค่าสีให้กับเส้นกราฟ
    pen_expenses.setWidth(2);  // ตั้งค่าความหนาของเส้นกราฟ
    series_expenses->setName("Expenses"); // ชื่อให้กับเส้นกราฟ
    series_expenses->setPen(pen_expenses); //ตั้งค่าเส้นของกราฟ income ให้เป็น pen_income ตามที่ตั้งค่าก่อนหน้า

    //==========Dot===========//
    //==================================================================================//

    Dot_series_income->setMarkerSize(10); // ตั้งค่าขนาดของจุด
    Dot_series_income->setColor(Qt::darkGreen); // ตั้งค่าสีของจุด
    Dot_series_income->setName("Dot-Income"); // ชื่อให้กับจุด

    Dot_series_expenses->setMarkerSize(10); // ตั้งค่าขนาดของจุด
    Dot_series_expenses->setColor(Qt::darkRed); // ตั้งค่าสีของจุด
    Dot_series_expenses->setName("Dot-Expenses"); // ชื่อให้กับจุด

    //==================================================================================//

    // Create a chart
    QChart *chart = new QChart;
    chart->addSeries(series_income); //เพิ่มเส้นกราฟ series_income ลงไปใน chart
    chart->addSeries(series_expenses); //เพิ่มเส้นกราฟ series_expenses ลงไปใน chart
    chart->setTitle("Analysis"); //ตั้งชื่อ chart

    chart->addSeries(Dot_series_income); // เพิ่มจุดของ Dot_series_income ลงไปใน chart
    chart->addSeries(Dot_series_expenses); // พิ่มจุดของ Dot_series_expenses ลงไปใน chart

    //==================================================================================//

    // เชื่อมต่อกับจุด Dot_series_income บนกราฟ และแสดงข้อมูลเมื่อนำเมาส์ไปวางที่จุด โดยเก็บข้อมูลไว้ในตัวแปร point เก็บสถานะว่าเมาส์วางหรือไม่ในตัวแปร state
    connect(Dot_series_income, &QScatterSeries::hovered, this, [=](const QPointF &point, bool state) 
        {
            if (state) // ถ้าเมาส์วางที่จุด จะแสดงข้อมูลของจุดนั้น
            {
                QString Income_at_the_cursor_position = QString::number(point.y()); // x คือเวลาและ y คือ จำนวนเงิน นำจำนวนเงินมาเก็บไว้ใน Income_at_the_cursor_position
                QString DateTime_at_the_cursor_position; // สร้างตัวแปร DateTime_at_the_cursor_position เพื่อเก็บวันที่ที่จุดนั้นอยู่
                if (Scale_logic) DateTime_at_the_cursor_position = QDateTime::fromMSecsSinceEpoch(point.x()).toString("dd-MM-yyyy"); // นำข้อมูลใน x มาแปลงเป็นวันที่และเก็บไว้ใน DateTime_at_the_cursor_position ถ้า Scale_logic เป็น true จะแสดงเฉพาะวันที่
                else DateTime_at_the_cursor_position = QDateTime::fromMSecsSinceEpoch(point.x()).toString("dd-MM-yyyy HH:mm:ss");  // นำข้อมูลใน x มาแปลงเป็นวันที่และเก็บไว้ใน DateTime_at_the_cursor_position ถ้า Scale_logic เป็น false จะแสดงวันที่และเวลา

                QString Text_Show_at_the_cursor_position_income = "📅Date: " + DateTime_at_the_cursor_position + "  📈Income: " + Income_at_the_cursor_position; // ข้อมูลแกน x ที่เป็นเวลาและ y ที่เป็นจำนวนเงิน มา format เป็นข้อความสำหรับแสดงผล
                QToolTip::showText(QCursor::pos(), Text_Show_at_the_cursor_position_income); // แสดงข้อความที่จุดนั้นๆ ที่เมาส์ชี้ไป
            }
            else // ถ้าเมาส์ไม่วางที่จุด จะซ่อนข้อความที่แสดง
            {
                QToolTip::hideText(); // ซ่อนข้อความที่แสดง
            }
        }
    );

    // เชื่อมต่อกับจุด Dot_series_expenses บนกราฟ และแสดงข้อมูลเมื่อนำเมาส์ไปวางที่จุด โดยเก็บข้อมูลไว้ในตัวแปร point เก็บสถานะว่าเมาส์วางหรือไม่ในตัวแปร state
    connect(Dot_series_expenses, &QScatterSeries::hovered, this, [=](const QPointF &point, bool state)
        {
            if (state) // ถ้าเมาส์วางที่จุด จะแสดงข้อมูลของจุดนั้น
            {
                QString Expenses_at_the_cursor_position = QString::number(point.y()); // x คือเวลาและ y คือ จำนวนเงิน นำจำนวนเงินมาเก็บไว้ใน Expenses_at_the_cursor_position
                QString DateTime_at_the_cursor_position; // สร้างตัวแปร DateTime_at_the_cursor_position เพื่อเก็บวันที่ที่จุดนั้นอยู่
                if (Scale_logic) DateTime_at_the_cursor_position = QDateTime::fromMSecsSinceEpoch(point.x()).toString("dd-MM-yyyy"); // นำข้อมูลใน x มาแปลงเป็นวันที่และเก็บไว้ใน DateTime_at_the_cursor_position ถ้า Scale_logic เป็น true จะแสดงเฉพาะวันที่
                else DateTime_at_the_cursor_position = QDateTime::fromMSecsSinceEpoch(point.x()).toString("dd-MM-yyyy HH:mm:ss"); // นำข้อมูลใน x มาแปลงเป็นวันที่และเก็บไว้ใน DateTime_at_the_cursor_position ถ้า Scale_logic เป็น false จะแสดงวันที่และเวลา


                QString Text_Show_at_the_cursor_position_Expenses = "📅Date: " + DateTime_at_the_cursor_position + " 📉Expense: " + Expenses_at_the_cursor_position; // ข้อมูลแกน x ที่เป็นเวลาและ y ที่เป็นจำนวนเงิน มา format เป็นข้อความสำหรับแสดงผล
                QToolTip::showText(QCursor::pos(), Text_Show_at_the_cursor_position_Expenses); // แสดงข้อความที่จุดนั้นๆ ที่เมาส์ชี้ไป
            }
            else // ถ้าเมาส์ไม่วางที่จุด จะซ่อนข้อความที่แสดง
            {
                QToolTip::hideText(); // ซ่อนข้อความที่แสดง
            }
        }
    );

    //==================================================================================//

    //ตั้งค่าแต่ละแกนของกราฟ
    //chart->createDefaultAxes(); //เป็นฟังก์ชันใน Qt Charts ที่ใช้ สร้างแกน X และแกน Y โดยอัตโนมัติ ตามข้อมูลที่เพิ่มเข้าไปในกราฟ
    // ใช้ QDateTimeAxis สำหรับแกน X
    QDateTimeAxis *axisX = new QDateTimeAxis; // สร้าง QDateTimeAxis ชื่อ axisX เพื่อใช้เป็นแกน X
    axisX->setTitleText("Date"); // ตั้งค่าชื่อแกน X ว่า Date
    if (Scale_logic) // ถ้า Scale_logic เป็น true จะทำการ scale และจะมีการตั้งค่าแกน x ตามนี้
    {
        axisX->setFormat("dd MMM yy"); // ตั้งค่าการแสดงผลวันที่
        if (add_count >= 10) axisX->setTickCount(10); // มีข้อมูลมากกว่าหรือเท่ากับ 10 วัน แกน x จะแบ่งเป็น 10 ช่วง
        else if (add_count < 10 and add_count > 1) axisX->setTickCount(add_count); // มีข้อมูลน้อยกว่า 10 วัน แกน x จะแบ่งเท่ากับจำนวนข้อมูล
        else if (add_count == 1) // ถ้ามีข้อมูลเพียงวันเดียว
        {
            axisX->setRange(QDateTime::fromMSecsSinceEpoch(income_ONE_point.x()).date().startOfDay() , QDateTime::fromMSecsSinceEpoch(income_ONE_point.x()).date().endOfDay()); // ตั้งค่าช่วงของแกน x ให้เป็นวันที่เดียว ตั้งแต่เวลา 00:00:00 ถึง 23:59:59
            axisX->setTickCount(2); // แกน x จะแบ่งเป็น 2 ช่วง
        }
        else // ถ้าไม่มีข้อมูลเลย
        {
            axisX->setRange(minDate_range , maxDate_range); // ตั้งค่าช่วงของแกน x ให้เป็นช่วงที่เลือกในปฏิทิน
            axisX->setTickCount(2); // แกน x จะแบ่งเป็น 2 ช่วง
        }

        // ถ้ามีข้อมูลกราฟจะแสดงข้อมูลตั้งแต่วันแรกที่มีข้อมูลถึงวันสุดท้ายที่มีข้อมูล
        // ถ้าไม่มีข้อมูลจะแสดงวันที่เริ่มต้นและสิ้นสุดของช่วงที่เลือก
    }
    else // ถ้า Scale_logic เป็น false จะไม่ทำการ scale และจะมีการตั้งค่าแกน x ตามนี้
    {
        axisX->setFormat("dd MMM yy HH:mm"); // ตั้งค่าการแสดงผลวันที่และเวลา
        if (add_count >= 7) axisX->setTickCount(7); // มีข้อมูลมากกว่าหรือเท่ากับ 7 วัน แกน x จะแบ่งเป็น 7 ช่วง
        else if (add_count < 7 and add_count > 1) axisX->setTickCount(add_count); // มีข้อมูลน้อยกว่า 7 วัน แกน x จะแบ่งเท่ากับจำนวนข้อมูล
        else if (add_count == 1) // ถ้ามีข้อมูลเพียงวันเดียว
        {
            axisX->setRange(QDateTime::fromMSecsSinceEpoch(income_ONE_point.x()).date().startOfDay() , QDateTime::fromMSecsSinceEpoch(income_ONE_point.x()).date().endOfDay()); // ตั้งค่าช่วงของแกน x ให้เป็นวันที่เดียว ตั้งแต่เวลา 00:00:00 ถึง 23:59:59
            axisX->setTickCount(2); // แกน x จะแบ่งเป็น 2 ช่วง
        }
        else
        {
            axisX->setRange(minDate_range , maxDate_range); // ตั้งค่าช่วงของแกน x ให้เป็นช่วงที่เลือกในปฏิทิน
            axisX->setTickCount(2); // แกน x จะแบ่งเป็น 2 ช่วง
        }

        // ถ้ามีข้อมูลกราฟจะแสดงข้อมูลตั้งแต่วันแรกที่มีข้อมูลถึงวันสุดท้ายที่มีข้อมูล
        // ถ้าไม่มีข้อมูลจะแสดงวันที่เริ่มต้นและสิ้นสุดของช่วงที่เลือก
    }



    // ตั้งค่าแกน Y
    vector<long double> max_y(2); // สร้าง vector ของ long double ชื่อ max_y เพื่อเก็บค่าสูงสุดของแกน y ของกราฟ

    if (chartData_income_For_CreateChart.size() <= 0) max_y.push_back(0); // ถ้าไม่มีข้อมูลรายรับ จะเพิ่ม 0 ลงใน max_y
    else max_y.push_back(*max_element(chartData_income_For_CreateChart.begin() , chartData_income_For_CreateChart.end())); // ถ้ามีข้อมูลรายรับ จะเพิ่มค่าสูงสุดของรายรับลงใน max_y

    if (chartData_expenses_For_CreateChart.size() <= 0) max_y.push_back(0); // ถ้าไม่มีข้อมูลรายจ่าย จะเพิ่ม 0 ลงใน max_y
    else max_y.push_back(*max_element(chartData_expenses_For_CreateChart.begin() , chartData_expenses_For_CreateChart.end())); // ถ้ามีข้อมูลรายจ่าย จะเพิ่มค่าสูงสุดของรายจ่ายลงใน max_y

    // max_y.push_back(*max_element(chartData_income_For_CreateChart.begin() , chartData_income_For_CreateChart.end()));
    // max_y.push_back(*max_element(chartData_expenses_For_CreateChart.begin() , chartData_expenses_For_CreateChart.end()));

    QValueAxis *axisY = new QValueAxis; // สร้าง QValueAxis ชื่อ axisY เพื่อใช้ตั้งค่าแกน Y
    axisY->setRange(0, *max_element(max_y.begin() , max_y.end())); // ตั้งค่าช่วงของแกน Y ให้เริ่มต้นที่ 0 ถึงค่าสูงสุดของข้อมูลทั้งหมด
    axisY->setTickCount(10); // แกน Y จะแบ่งเป็น 10 ช่วง
    axisY->setLabelFormat("%.2f"); // ตั้งค่าการแสดงผลของแกน Y ให้แสดงเป็นทศนิยม 2 ตำแหน่ง
    axisY->setTitleText("Amount (THB)"); // ตั้งค่าชื่อแกน Y ว่า Amount (THB)

    chart->addAxis(axisX, Qt::AlignBottom); // ตั้งค่าแกน X ให้อยู่ด้านล่างของกราฟ
    chart->addAxis(axisY, Qt::AlignLeft); // ตั้งค่าแกน Y ให้อยู่ด้านซ้ายของกราฟ
    series_income->attachAxis(axisX); // ตั้งค่าให้ series_income แสดงผลบนแกน X ตามที่ตั้งค่าไว้
    series_income->attachAxis(axisY); // ตั้งค่าให้ series_income แสดงผลบนแกน Y ตามที่ตั้งค่าไว้
    series_expenses->attachAxis(axisX); // ตั้งค่าให้ series_expenses แสดงผลบนแกน X ตามที่ตั้งค่าไว้
    series_expenses->attachAxis(axisY); // ตั้งค่าให้ series_expenses แสดงผลบนแกน Y ตามที่ตั้งค่าไว้ 
    Dot_series_expenses->attachAxis(axisX); // ตั้งค่าให้ Dot_series_expenses แสดงผลบนแกน X ตามที่ตั้งค่าไว้
    Dot_series_expenses->attachAxis(axisY); // ตั้งค่าให้ Dot_series_expenses แสดงผลบนแกน Y ตามที่ตั้งค่าไว้
    Dot_series_income->attachAxis(axisX); // ตั้งค่าให้ Dot_series_income แสดงผลบนแกน X ตามที่ตั้งค่าไว้
    Dot_series_income->attachAxis(axisY); // ตั้งค่าให้ Dot_series_income แสดงผลบนแกน Y ตามที่ตั้งค่าไว้

    //==================================================================================//

    // Create chart view
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing); //ทำให้เส้นคมชัดขึ้น

    //==================================================================================//

    // Add chart view to layout
    ui->Chart_Layout->addWidget(chartView); // เพิ่ม chartView ลงใน layout ของ ui

    //==================================================================================//

    if (add_count == 0) // ถ้าไม่มีข้อมูลที่เพิ่มเข้ามา
    {
        ui->NO_DATA->show(); // แสดงข้อความว่าไม่มีข้อมูล
    }

    //😲😲😲😲😲😲//

    Summary(); // หลังจากสร้างกราฟเสร็จ จะทำการแสดงผลข้อมูลสรุป
}


void analysis::on_accept_clicked() // function ทำงานเมื่อ click ปุ่ม accept
{
    playAcceptSound(); // เล่นเสียงเมื่อ click ปุ่ม accept
    Show_Chart(); // แสดงกราฟ
    Summary(); // แสดงข้อมูลสรุป

    double total = 0; // สร้างตัวแปร total เพื่อเก็บผลรวมของรายจ่ายทั้งหมด
    for (unsigned int i = 0 ; i < statement.size() ; i++) // วนลูปเพื่อหาผลรวมของรายจ่ายทั้งหมด
    {
        if (statement[i][2] > 0) total += double(statement[i][2]); // ถ้า statement[i][2] มากกว่า 0 จะเพิ่มค่าของ statement[i][2] ลงใน total
    }

    qDebug() << total; // แสดงผลรวมของรายจ่ายทั้งหมด
}


void analysis::Summary() // function สำหรับสรุปข้อมูลและแสดงข้อมูลอยู่ทางด้านขวาของ กราฟ
{
    //======================================================================================//

    vector<QDate> Date_Range_now = Date_range_now(); // นำข้อมูลวันที่ที่เลือกในปฏิทินมาเก็บไว้ใน Date_Range_now
    QDateTime minDate_range = Date_Range_now[0].startOfDay(); // นำวันที่เริ่มต้นของช่วงที่เลือกมาเก็บไว้ใน minDate_range
    QDateTime maxDate_range = Date_Range_now[1].endOfDay(); // นำวันที่สิ้นสุดของช่วงที่เลือกมาเก็บไว้ใน maxDate_range

    //======================================================================================//

    vector<Dishes> Dishes_data_in_range; // สร้าง vector ของ Dishes ชื่อ Dishes_data_in_range เพื่อเก็บข้อมูลเมนูอาหารในช่วงที่เลือก
    vector<Drinks> Drinks_data_in_range; // สร้าง vector ของ Drinks ชื่อ Drinks_data_in_range เพื่อเก็บข้อมูลเครื่องดื่มในช่วงที่เลือก
    vector<QDateTime> Date_in_range; // สร้าง vector ของ QDateTime ชื่อ Date_in_range เพื่อเก็บข้อมูลวันที่ในช่วงที่เลือก

    for (unsigned int i = 0; i < chartData_Date.size(); i++) // วนลูปเพื่อเก็บข้อมูลเมนูอาหาร ข้อมูลเครื่องดื่ม และข้อมูลวันที่ในช่วงที่เลือก
    {
        if (chartData_Date[i] < minDate_range or chartData_Date[i] > maxDate_range) continue; // ถ้าข้อมูลวันที่ไม่อยู่ในช่วงที่เลือก จะข้ามข้อมูลนั้น

        Date_in_range.push_back(chartData_Date[i]); // เก็บข้อมูลวันที่ในช่วงที่เลือก
        Dishes_data_in_range.push_back(Dishes_data[i]); // เก็บข้อมูลเมนูอาหารในช่วงที่เลือก
        Drinks_data_in_range.push_back(Drinks_data[i]); // เก็บข้อมูลเครื่องดื่มในช่วงที่เลือก
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

    Dishes Total_Dishes_data_in_range = template_Dishes; // สร้าง Dishes ชื่อ Total_Dishes_data_in_range โดยให้มีข้อมูลเป็น template_Dishes ที่มีข้อมูลเมนูอาหารทั้งหมด และมีจำนวนรวมแต่ละแมนูเป็น 0
    Drinks Total_Drinks_data_in_range = template_Drinks; // สร้าง Drinks ชื่อ Total_Drinks_data_in_range โดยให้มีข้อมูลเป็น template_Drinks ที่มีข้อมูลเครื่องดื่มทั้งหมด และมีจำนวนรวมแต่ละเครื่องดื่มเป็น 0

    for (unsigned int i = 0 ; i < Date_in_range.size() ; i++) // วนลูปเท่ากับจำนวนวันที่ในช่วงที่เลือก
    {
        for (unsigned int j = 0 ; j < Dishes_data_in_range[i].name.size() ; j++) // วนลูปเท่ากับจำนวนเมนูอาหารทั้งหมด
        {
            string name_menu = Dishes_data_in_range[i].name[j]; // เก็บชื่อเมนูอาหารไว้ใน name_menu
            int amount = Dishes_data_in_range[i].amount[j]; // เก็บจำนวนเมนูอาหารไว้ใน amount

            int index_in_count_Dishes = std::distance(Total_Dishes_data_in_range.name.begin() , std::find(Total_Dishes_data_in_range.name.begin() , Total_Dishes_data_in_range.name.end() , name_menu)); // หา index ของ name_menu ใน vector ของ name ของ Total_Dishes_data_in_range
            Total_Dishes_data_in_range.amount[index_in_count_Dishes] += amount; // เพิ่มจำนวนเมนูอาหารใน index ที่เก็บ name_menu ใน vector ของ amount ของ Total_Dishes_data_in_range
        }

        for (unsigned int j = 0 ; j < Drinks_data_in_range[i].name.size() ; j++) // วนลูปเท่ากับจำนวนเครื่องดื่มทั้งหมด
        {
            string name_menu = Drinks_data_in_range[i].name[j]; // เก็บชื่อเครื่องดื่มไว้ใน name_menu
            int amount = Drinks_data_in_range[i].amount[j]; // เก็บจำนวนเครื่องดื่มไว้ใน amount

            int index_in_count_Drinks = std::distance(Total_Drinks_data_in_range.name.begin() , std::find(Total_Drinks_data_in_range.name.begin() , Total_Drinks_data_in_range.name.end() , name_menu)); // หา index ของ name_menu ใน vector ของ name ของ Total_Drinks_data_in_range
            Total_Drinks_data_in_range.amount[index_in_count_Drinks] += amount; // เพิ่มจำนวนเครื่องดื่มใน index ที่เก็บ name_menu ใน vector ของ amount ของ Total_Drinks_data_in_range
        }
    }

    //======================================================================================//

    // qDebug() << "\n\ncount_in_range";
    // qDebug() << "Dished";
    // for (int i = 0 ; i < Total_Dishes_data_in_range.name.size() ; i++)
    // {
    //     qDebug() << "Name : " << Total_Dishes_data_in_range.name[i] << " Amount : " << Total_Dishes_data_in_range.amount[i];
    // }

    // qDebug() << "Drinks";
    // for (int i = 0 ; i < Total_Drinks_data_in_range.name.size() ; i++)
    // {
    //     qDebug() << "Name : " << Total_Drinks_data_in_range.name[i] << " Amount : " << Total_Drinks_data_in_range.amount[i];
    // }

    //======================================================================================//
    // Sort count_data_in_range // Selection sort

    for(unsigned int i = 0 ; i < Total_Dishes_data_in_range.name.size() ; i++) // วนลูปเท่ากับจำนวนเมนูอาหารทั้งหมด
    {
        int max = Total_Dishes_data_in_range.amount[i]; // สร้างตัวแปร max เพื่อเก็บจำนวนเมนูอาหารที่มากที่สุด
        int index_max = i; // สร้างตัวแปร index_max เพื่อเก็บ index ของจำนวนเมนูอาหารที่มากที่สุด
        for (unsigned int j = i ; j < Total_Dishes_data_in_range.name.size() ; j++) // วนลูปเท่ากับจำนวนเมนูอาหารทั้งหมด
        {
            if (Total_Dishes_data_in_range.amount[j] > max) // ถ้าจำนวนเมนูอาหารที่ j มากกว่า max
            {
                max = Total_Dishes_data_in_range.amount[j]; // ให้ max เป็นจำนวนเมนูอาหารที่ j
                index_max = j; // ให้ index_max เป็น index ของจำนวนเมนูอาหารที่ j
            }
        }

        // swap
        int amount1 = Total_Dishes_data_in_range.amount[i]; // สร้างตัวแปร amount1 เพื่อเก็บจำนวนเมนูอาหารที่ i
        int amount2 = max; // สร้างตัวแปร amount2 เพื่อเก็บจำนวนเมนูอาหารที่ max
        Total_Dishes_data_in_range.amount[i] = amount2; // ให้จำนวนเมนูอาหารที่ i เป็นจำนวนเมนูอาหารที่ max
        Total_Dishes_data_in_range.amount[index_max] = amount1; // ให้จำนวนเมนูอาหารที่ max เป็นจำนวนเมนูอาหารที่ i

        string name1 = Total_Dishes_data_in_range.name[i]; // สร้างตัวแปร name1 เพื่อเก็บชื่อเมนูอาหารที่ i
        string name2 = Total_Dishes_data_in_range.name[index_max]; // สร้างตัวแปร name2 เพื่อเก็บชื่อเมนูอาหารที่ max
        Total_Dishes_data_in_range.name[i] = name2; // ให้ชื่อเมนูอาหารที่ i เป็นชื่อเมนูอาหารที่ max
        Total_Dishes_data_in_range.name[index_max] = name1; // ให้ชื่อเมนูอาหารที่ max เป็นชื่อเมนูอาหารที่ i


    }

    for(unsigned int i = 0 ; i < Total_Drinks_data_in_range.name.size() ; i++) // วนลูปเท่ากับจำนวนเครื่องดื่มทั้งหมด // เหมือนกับด้านบนแต่เป็นการ sort ของ Drinks
    {
        int max = Total_Drinks_data_in_range.amount[i]; // สร้างตัวแปร max เพื่อเก็บจำนวนเครื่องดื่มที่มากที่สุด
        int index_max = i; // สร้างตัวแปร index_max เพื่อเก็บ index ของจำนวนเครื่องดื่มที่มากที่สุด
        for (unsigned int j = i ; j < Total_Drinks_data_in_range.name.size() ; j++) // วนลูปเท่ากับจำนวนเครื่องดื่มทั้งหมด
        {
            if (Total_Drinks_data_in_range.amount[j] > max) // ถ้าจำนวนเครื่องดื่มที่ j มากกว่า max
            {
                max = Total_Drinks_data_in_range.amount[j]; // ให้ max เป็นจำนวนเครื่องดื่มที่ j
                index_max = j; // ให้ index_max เป็น index ของจำนวนเครื่องดื่มที่ j
            }
        }

        // swap
        int amount1 = Total_Drinks_data_in_range.amount[i]; // สร้างตัวแปร amount1 เพื่อเก็บจำนวนเครื่องดื่มที่ i
        int amount2 = max; // สร้างตัวแปร amount2 เพื่อเก็บจำนวนเครื่องดื่มที่ max
        Total_Drinks_data_in_range.amount[i] = amount2; // ให้จำนวนเครื่องดื่มที่ i เป็นจำนวนเครื่องดื่มที่ max
        Total_Drinks_data_in_range.amount[index_max] = amount1; // ให้จำนวนเครื่องดื่มที่ max เป็นจำนวนเครื่องดื่มที่ i

        string name1 = Total_Drinks_data_in_range.name[i]; // สร้างตัวแปร name1 เพื่อเก็บชื่อเครื่องดื่มที่ i
        string name2 = Total_Drinks_data_in_range.name[index_max]; // สร้างตัวแปร name2 เพื่อเก็บชื่อเครื่องดื่มที่ max
        Total_Drinks_data_in_range.name[i] = name2; // ให้ชื่อเครื่องดื่มที่ i เป็นชื่อเครื่องดื่มที่ max
        Total_Drinks_data_in_range.name[index_max] = name1; // ให้ชื่อเครื่องดื่มที่ max เป็นชื่อเครื่องดื่มที่ i


    }

    //======================================================================================//

    // qDebug() << "\n\nTotal_in_range";
    // qDebug() << "Dished";
    // for (int i = 0 ; i < Total_Dishes_data_in_range.name.size() ; i++)
    // {
    //     qDebug() << "Name : " << Total_Dishes_data_in_range.name[i] << " Amount : " << Total_Dishes_data_in_range.amount[i];
    // }

    // qDebug() << "Drinks";
    // for (int i = 0 ; i < Total_Drinks_data_in_range.name.size() ; i++)
    // {
    //     qDebug() << "Name : " << Total_Drinks_data_in_range.name[i] << " Amount : " << Total_Drinks_data_in_range.amount[i];
    // }

    //======================================================================================//

    ui->Top_Dished->clearContents(); // ลบข้อมูลในตาราง Top_Dished ไม่ได้ลบแถวแต่ลบข้อมูลในแต่ละเซลล์
    for (int i = ui->Top_Dished->rowCount()-1 ; i >= 0  ; i--) ui->Top_Dished->removeRow(i); // ลบแถวในตาราง Top_Dished ทั้งหมดโดยเริ่มจากแถวสุดท้าย ไปจนถึงแถวแรก

    ui->Top_Drinks->clearContents(); // ลบข้อมูลในตาราง Top_Drinks ไม่ได้ลบแถวแต่ลบข้อมูลในแต่ละเซลล์
    for (int i = ui->Top_Drinks->rowCount()-1 ; i >= 0  ; i--) ui->Top_Drinks->removeRow(i); // ลบแถวในตาราง Top_Drinks ทั้งหมดโดยเริมจากแถวสุดท้าย ไปจนถึงแถวแรก

    //--------total_order----------//
    int total_order = 0; // สร้างตัวแปร total_order เพื่อเก็บจำนวนรวมของรายการทั้งหมด
    //--------total_order----------//

    for (unsigned int i = 0 ; i < Total_Dishes_data_in_range.name.size() ; i++) // วนลูปเท่ากับจำนวนเมนูอาหารทั้งหมด
    {
        ui->Top_Dished->insertRow(ui->Top_Dished->rowCount()); // เพิ่มแถวในตาราง Top_Dished
        ui->Top_Dished->setItem(i , 0 , new QTableWidgetItem(QString::fromStdString(Total_Dishes_data_in_range.name[i]))); // ใส่ชื่อเมนูอาหารลงในเซลล์ที่ 0 ของแถวที่ i ในตาราง Top_Dished
        ui->Top_Dished->setItem(i , 1 , new QTableWidgetItem(QString::number(Total_Dishes_data_in_range.amount[i]))); // ใส่จำนวนเมนูอาหารลงในเซลล์ที่ 1 ของแถวที่ i ในตาราง Top_Dished

        //--------total_order----------//
        total_order += Total_Dishes_data_in_range.amount[i]; // เพิ่มจำนวนเมนูอาหารใน index ที่ i ใน vector ของ amount ของ Total_Dishes_data_in_range ลงใน total_order
        //--------total_order----------//
    }

    for (unsigned int i = 0 ; i < Total_Drinks_data_in_range.name.size() ; i++) // วนลูปเท่ากับจำนวนเครื่องดื่มทั้งหมด // เหมือนกับด้านบนแต่เป็นการแสดงข้อมูลของ Drinks
    {
        ui->Top_Drinks->insertRow(ui->Top_Drinks->rowCount()); // เพิ่มแถวในตาราง Top_Drinks
        ui->Top_Drinks->setItem(i , 0 , new QTableWidgetItem(QString::fromStdString(Total_Drinks_data_in_range.name[i]))); // ใส่ชื่อเครื่องดื่มลงในเซลล์ที่ 0 ของแถวที่ i ในตาราง Top_Drinks
        ui->Top_Drinks->setItem(i , 1 , new QTableWidgetItem(QString::number(Total_Drinks_data_in_range.amount[i]))); // ใส่จำนวนเครื่องดื่มลงในเซลล์ที่ 1 ของแถวที่ i ในตาราง Top_Drinks

        //--------total_order----------//
        total_order += Total_Drinks_data_in_range.amount[i]; // เพิ่มจำนวนเครื่องดื่มใน index ที่ i ใน vector ของ amount ของ Total_Drinks_data_in_range ลงใน total_order
        //--------total_order----------//
    }

    //00000000000000000000000000000000000000000000000000000000000000000000000000000000000000//
    //00000000000000000000000000000000000000000000000000000000000000000000000000000000000000//

    QFont Format_Fonnt_Bold_Center_16; // สร้าง QFont ชื่อ Format_Fonnt_Bold_Center_16 เพื่อใช้ตั้งค่า font ให้เป็น Bold และ Center ขนาด 16
    Format_Fonnt_Bold_Center_16.setBold(true); // ตั้งค่าให้ font เป็น Bold
    Format_Fonnt_Bold_Center_16.setPointSize(16); // ตั้งค่าขนาด font เป็น 16

    ui->Income_in_range->setText(QString::number(Total_Income_in_selected_range , 'f' ,2)); // แสดงรายรับทั้งหมดในช่วงที่เลือก
    ui->Income_in_range->setFont(Format_Fonnt_Bold_Center_16); // ตั้งค่า font ของ Income_in_range ให้เป็น Format_Fonnt_Bold_Center_16
    ui->Income_in_range->setAlignment(Qt::AlignCenter); // ตั้งค่าการจัดวางของข้อความใน Income_in_range ให้เป็น Center

    ui->Expenses_in_range->setText(QString::number(Total_Expenses_in_selected_range , 'f' ,2)); // แสดงรายจ่ายทั้งหมดในช่วงที่เลือก
    ui->Expenses_in_range->setFont(Format_Fonnt_Bold_Center_16); // ตั้งค่า font ของ Expenses_in_range ให้เป็น Format_Fonnt_Bold_Center_16
    ui->Expenses_in_range->setAlignment(Qt::AlignCenter); // ตั้งค่าการจัดวางของข้อความใน Expenses_in_range ให้เป็น Center

    //00000000000000000000000000000000000000000000000000000000000000000000000000000000000000//
    //00000000000000000000000000000000000000000000000000000000000000000000000000000000000000//


    ui->Total_Order_in_range->setText(QString::number(total_order)); // แสดงจำนวนรายการทั้งหมดในช่วงที่เลือก
    ui->Total_Order_in_range->setFont(Format_Fonnt_Bold_Center_16); // ตั้งค่า font ของ Total_Order_in_range ให้เป็น Format_Fonnt_Bold_Center_16
    ui->Total_Order_in_range->setAlignment(Qt::AlignCenter); // ตั้งค่าการจัดวางของข้อความใน Total_Order_in_range ให้เป็น Center


}


void analysis::on_comboBox_Scale_mode_currentIndexChanged(int scale_mode)
{
    Show_Chart();
}

