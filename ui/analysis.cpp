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


void analysis::on_calendar_end_date_selectionChanged() // เมื่อปฏิทินวันที่สิ้นสุด(ปฏิทินด้านขวา) มีการเลือกวันที่ใหม่ // ที่เมื่อปฏิทินวันที่สิ้นสุด(ปฏิทินด้านขวา) มีเงื่อนไขสำหรับโหมดเดียวเพราะ ปฏิทินด้านขวาจะกดเลือกไม่ได้เลยถ้าหากไม่ได้อยู่ในโหมด range
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


void analysis::on_comboBox_search_mode_currentIndexChanged(int searchmode)
{
    //Refresh_calendar // แต่ถ้าเป็นการเปลี่ยนโหมดจะไม่มีการเปลี่ยนวันที่ที่เลือกไปเป็นวันปัจจุบัน
    // ลบ format ทั้งหมดออก
    ui->calendar_start_date->setDateTextFormat(QDate(), QTextCharFormat());
    ui->calendar_end_date->setDateTextFormat(QDate(), QTextCharFormat());

    switch (searchmode)
    {
    case 0:
        ui->calendar_end_date->setSelectedDate(ui->calendar_start_date->selectedDate()); // ทำการเซ็ตวันที่สิ้นสุดให้เป็นวันที่เริ่มต้น

        ui->calendar_start_date->setEnabled(true);
        ui->calendar_end_date->setEnabled(true);
        break;
    case 1:
        ui->calendar_start_date->setEnabled(true);
        ui->calendar_end_date->setEnabled(false);
        break;
    case 2:
        ui->calendar_start_date->setEnabled(true);
        ui->calendar_end_date->setEnabled(false);
        Update_Highlight_Week();
        break;
    case 3:
        ui->calendar_start_date->setEnabled(true);
        ui->calendar_end_date->setEnabled(false);
        Update_Highlight_Month();
        break;
    case 4:
        ui->calendar_start_date->setEnabled(true);
        ui->calendar_end_date->setEnabled(false);
        Update_Highlight_Year();
        break;
    case 5:
        ui->calendar_start_date->setEnabled(false);
        ui->calendar_end_date->setEnabled(false);
        break;
    }

    ui->calendar_start_date->setMaximumDate(QDate::currentDate().addYears(1000)); // ทำการเซ็ตวันที่เริ่มต้นให้สามารถเลือกได้มากสุดคือวันปัจจุบัน+1000 ปี
    ui->calendar_end_date->setMinimumDate(QDate::currentDate().addYears(-1000)); // ทำการเซ็ตวันที่สิ้นสุดให้สามารถเลือกได้น้อยสุดคือวันปัจจุบัน-1000 ปี


    Show_Chart();
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

    Total_Income_in_selected_range = 0;
    Total_Expenses_in_selected_range = 0;

    //==================================================================================//

    if (chartView != 0)
    {
        ui->Chart_Layout->removeWidget(chartView);
        delete chartView;
        chartView = 0;
    }

    //==================================================================================//

    vector<QDate> Date_Range_now = Date_range_now();
    QDateTime minDate_range = Date_Range_now[0].startOfDay();
    QDateTime maxDate_range = Date_Range_now[1].endOfDay();

    int count_Day_in_range = Date_Range_now[0].daysTo(Date_Range_now[1]);

    //==================================================================================//

    vector<QDateTime> chartData_Date_For_CreateChart;
    vector<double> chartData_income_For_CreateChart;
    vector<double> chartData_expenses_For_CreateChart;

    bool Scale_logic;
    switch (ui->comboBox_Scale_mode->currentIndex())
    {
        case 0:
            Scale_logic = count_Day_in_range >= 6;
            break;
        case 1:
            Scale_logic = true;
            break;
        case 2:
            Scale_logic = false;
            break;
    }

    if (Scale_logic)
    {
        chartData_Date_For_CreateChart = chartData_Date_scaled_QDate;
        chartData_income_For_CreateChart = chartData_income_scaled_QDate;
        chartData_expenses_For_CreateChart = chartData_expenses_scaled_QDate;
    }
    else
    {
        chartData_Date_For_CreateChart = chartData_Date;
        chartData_income_For_CreateChart = chartData_income;
        chartData_expenses_For_CreateChart = chartData_expenses;
    }

    //==================================================================================//

    // Create a line series
    QLineSeries *series_income = new QLineSeries; // กราฟเส้น
    QLineSeries *series_expenses = new QLineSeries;
    QScatterSeries *Dot_series_income = new QScatterSeries; // กราฟจุด
    QScatterSeries *Dot_series_expenses = new QScatterSeries;

    int add_count = 0;
    for (unsigned int i = 0; i < chartData_Date_For_CreateChart.size(); i++)
    {
        if (chartData_Date_For_CreateChart[i] < minDate_range or chartData_Date_For_CreateChart[i] > maxDate_range) continue;

        //---------------total-----------------//
        Total_Income_in_selected_range += chartData_income_For_CreateChart[i];
        Total_Expenses_in_selected_range += chartData_expenses_For_CreateChart[i];
        //---------------total-----------------//

        qint64 xValue = chartData_Date_For_CreateChart[i].toMSecsSinceEpoch();

        //---------------line series-----------------//
        series_income->append(xValue, chartData_income_For_CreateChart[i]);
        series_expenses->append(xValue, chartData_expenses_For_CreateChart[i]);
        //---------------line series-----------------//

        //---------------Dot series-----------------//
        if (chartData_income_For_CreateChart[i] != 0) Dot_series_income->append(xValue, chartData_income_For_CreateChart[i]);
        if (chartData_expenses_For_CreateChart[i] != 0) Dot_series_expenses->append(xValue, chartData_expenses_For_CreateChart[i]);
        //---------------Dot series-----------------//
        add_count++;
    }

    //==================================================================================//
    // qDebug() << add_count;

    QPointF income_ONE_point , expenses_ONE_point;
    if (add_count == 1)
    {
        income_ONE_point = series_income->at(0);
        expenses_ONE_point = series_expenses->at(0);

        series_income->clear();
        series_income->append(QDateTime::fromMSecsSinceEpoch(income_ONE_point.x()).date().startOfDay().toMSecsSinceEpoch()  , 0);
        series_income->append(income_ONE_point.x() , income_ONE_point.y());
        series_income->append(QDateTime::fromMSecsSinceEpoch(income_ONE_point.x()).date().endOfDay().toMSecsSinceEpoch() , 0);

        series_expenses->clear();
        series_expenses->append(QDateTime::fromMSecsSinceEpoch(expenses_ONE_point.x()).date().startOfDay().toMSecsSinceEpoch() , 0);
        series_expenses->append(expenses_ONE_point.x() , expenses_ONE_point.y());
        series_expenses->append(QDateTime::fromMSecsSinceEpoch(expenses_ONE_point.x()).date().endOfDay().toMSecsSinceEpoch() , 0);
    }

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
    Dot_series_income->setName("Dot-Income");

    Dot_series_expenses->setMarkerSize(10); // ตั้งค่าขนาดของจุด
    Dot_series_expenses->setColor(Qt::darkRed); // ตั้งค่าสีของจุด
    Dot_series_expenses->setName("Dot-Expenses");

    //==================================================================================//

    // Create a chart
    QChart *chart = new QChart;
    chart->addSeries(series_income); //เพิ่มเส้นกราฟ series_income ลงไปใน chart
    chart->addSeries(series_expenses); //เพิ่มเส้นกราฟ series_expenses ลงไปใน chart
    chart->setTitle("Analysis"); //ตั้งชื่อ chart

    chart->addSeries(Dot_series_income); // เพิ่มจุดของ Dot_series_income ลงไปใน chart
    chart->addSeries(Dot_series_expenses); // พิ่มจุดของ Dot_series_expenses ลงไปใน chart

    //==================================================================================//

    connect(Dot_series_income, &QScatterSeries::hovered, this, [=](const QPointF &point, bool state)
        {
            if (state)
            {
                QString Income_at_the_cursor_position = QString::number(point.y());
                QString DateTime_at_the_cursor_position;
                if (Scale_logic) DateTime_at_the_cursor_position = QDateTime::fromMSecsSinceEpoch(point.x()).toString("dd-MM-yyyy");
                else DateTime_at_the_cursor_position = QDateTime::fromMSecsSinceEpoch(point.x()).toString("dd-MM-yyyy HH:mm:ss");

                QString Text_Show_at_the_cursor_position_income = "📅Date: " + DateTime_at_the_cursor_position + "  📈Income: " + Income_at_the_cursor_position;
                QToolTip::showText(QCursor::pos(), Text_Show_at_the_cursor_position_income);
            }
            else
            {
                QToolTip::hideText();
            }
        }
    );

    connect(Dot_series_expenses, &QScatterSeries::hovered, this, [=](const QPointF &point, bool state)
        {
            if (state)
            {
                QString Expenses_at_the_cursor_position = QString::number(point.y());
                QString DateTime_at_the_cursor_position;
                if (Scale_logic) DateTime_at_the_cursor_position = QDateTime::fromMSecsSinceEpoch(point.x()).toString("dd-MM-yyyy");
                else DateTime_at_the_cursor_position = QDateTime::fromMSecsSinceEpoch(point.x()).toString("dd-MM-yyyy HH:mm:ss");


                QString Text_Show_at_the_cursor_position_Expenses = "📅Date: " + DateTime_at_the_cursor_position + " 📉Expense: " + Expenses_at_the_cursor_position;
                QToolTip::showText(QCursor::pos(), Text_Show_at_the_cursor_position_Expenses);
            }
            else
            {
                QToolTip::hideText();
            }
        }
    );

    //==================================================================================//

    //ตั้งค่าแต่ละแกนของกราฟ
    //chart->createDefaultAxes(); //เป็นฟังก์ชันใน Qt Charts ที่ใช้ สร้างแกน X และแกน Y โดยอัตโนมัติ ตามข้อมูลที่เพิ่มเข้าไปในกราฟ
    // ใช้ QDateTimeAxis สำหรับแกน X
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTitleText("Date");
    if (Scale_logic)
    {
        axisX->setFormat("dd MMM yy"); // ตั้งค่าการแสดงผลวันที่
        if (add_count >= 10) axisX->setTickCount(10);
        else if (add_count < 10 and add_count > 1) axisX->setTickCount(add_count);
        else if (add_count == 1)
        {
            axisX->setRange(QDateTime::fromMSecsSinceEpoch(income_ONE_point.x()).date().startOfDay() , QDateTime::fromMSecsSinceEpoch(income_ONE_point.x()).date().endOfDay());
            axisX->setTickCount(2);
        }
        else
        {
            axisX->setRange(minDate_range , maxDate_range);
            axisX->setTickCount(2);
        }
    }
    else
    {
        axisX->setFormat("dd MMM yy HH:mm");
        if (add_count >= 7) axisX->setTickCount(7);
        else if (add_count < 7 and add_count > 1) axisX->setTickCount(add_count);
        else if (add_count == 1)
        {
            axisX->setRange(QDateTime::fromMSecsSinceEpoch(income_ONE_point.x()).date().startOfDay() , QDateTime::fromMSecsSinceEpoch(income_ONE_point.x()).date().endOfDay());
            axisX->setTickCount(2);
        }
        else
        {
            axisX->setRange(minDate_range , maxDate_range);
            axisX->setTickCount(2);
        }
    }



    \
    vector<long double> max_y(2);

    if (chartData_income_For_CreateChart.size() <= 0) max_y.push_back(0);
    else max_y.push_back(*max_element(chartData_income_For_CreateChart.begin() , chartData_income_For_CreateChart.end()));

    if (chartData_expenses_For_CreateChart.size() <= 0) max_y.push_back(0);
    else max_y.push_back(*max_element(chartData_expenses_For_CreateChart.begin() , chartData_expenses_For_CreateChart.end()));

    // max_y.push_back(*max_element(chartData_income_For_CreateChart.begin() , chartData_income_For_CreateChart.end()));
    // max_y.push_back(*max_element(chartData_expenses_For_CreateChart.begin() , chartData_expenses_For_CreateChart.end()));

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, *max_element(max_y.begin() , max_y.end()));
    axisY->setTickCount(10);
    axisY->setLabelFormat("%.2f");
    axisY->setTitleText("Amount (THB)");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series_income->attachAxis(axisX);
    series_income->attachAxis(axisY);
    series_expenses->attachAxis(axisX);
    series_expenses->attachAxis(axisY);
    Dot_series_expenses->attachAxis(axisX);
    Dot_series_expenses->attachAxis(axisY);
    Dot_series_income->attachAxis(axisX);
    Dot_series_income->attachAxis(axisY);

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

    //😲😲😲😲😲😲//
    Summary();
}


void analysis::on_accept_clicked()
{
    playAcceptSound();
    Show_Chart();
    Summary();

    double total = 0;
    for (int i = 0 ; i < statement.size() ; i++)
    {
        if (statement[i][2] > 0) total += double(statement[i][2]);
    }

    qDebug() << total;
}


void analysis::Summary()
{
    //======================================================================================//

    vector<QDate> Date_Range_now = Date_range_now();
    QDateTime minDate_range = Date_Range_now[0].startOfDay();
    QDateTime maxDate_range = Date_Range_now[1].endOfDay();

    //======================================================================================//

    vector<Dishes> Dishes_data_in_range;
    vector<Drinks> Drinks_data_in_range;
    vector<QDateTime> Date_in_range;

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

    Dishes Total_Dishes_data_in_range = template_Dishes;
    Drinks Total_Drinks_data_in_range = template_Drinks;

    for (unsigned int i = 0 ; i < Date_in_range.size() ; i++)
    {
        for (unsigned int j = 0 ; j < Dishes_data_in_range[i].name.size() ; j++)
        {
            string name_menu = Dishes_data_in_range[i].name[j];
            int amount = Dishes_data_in_range[i].amount[j];

            int index_in_count_Dishes = std::distance(Total_Dishes_data_in_range.name.begin() , std::find(Total_Dishes_data_in_range.name.begin() , Total_Dishes_data_in_range.name.end() , name_menu));
            Total_Dishes_data_in_range.amount[index_in_count_Dishes] += amount;
        }

        for (unsigned int j = 0 ; j < Drinks_data_in_range[i].name.size() ; j++)
        {
            string name_menu = Drinks_data_in_range[i].name[j];
            int amount = Drinks_data_in_range[i].amount[j];

            int index_in_count_Drinks = std::distance(Total_Drinks_data_in_range.name.begin() , std::find(Total_Drinks_data_in_range.name.begin() , Total_Drinks_data_in_range.name.end() , name_menu));
            Total_Drinks_data_in_range.amount[index_in_count_Drinks] += amount;
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
    // Sort count_data_in_range //

    for(unsigned int i = 0 ; i < Total_Dishes_data_in_range.name.size() ; i++)
    {
        int max = Total_Dishes_data_in_range.amount[i];
        int index_max = i;
        for (unsigned int j = i ; j < Total_Dishes_data_in_range.name.size() ; j++)
        {
            if (Total_Dishes_data_in_range.amount[j] > max)
            {
                max = Total_Dishes_data_in_range.amount[j];
                index_max = j;
            }
        }

        // swap
        int amount1 = Total_Dishes_data_in_range.amount[i];
        int amount2 = max;
        Total_Dishes_data_in_range.amount[i] = amount2;
        Total_Dishes_data_in_range.amount[index_max] = amount1;

        string name1 = Total_Dishes_data_in_range.name[i];
        string name2 = Total_Dishes_data_in_range.name[index_max];
        Total_Dishes_data_in_range.name[i] = name2;
        Total_Dishes_data_in_range.name[index_max] = name1;


    }

    for(unsigned int i = 0 ; i < Total_Drinks_data_in_range.name.size() ; i++)
    {
        int max = Total_Drinks_data_in_range.amount[i];
        int index_max = i;
        for (unsigned int j = i ; j < Total_Drinks_data_in_range.name.size() ; j++)
        {
            if (Total_Drinks_data_in_range.amount[j] > max)
            {
                max = Total_Drinks_data_in_range.amount[j];
                index_max = j;
            }
        }

        // swap
        int amount1 = Total_Drinks_data_in_range.amount[i];
        int amount2 = max;
        Total_Drinks_data_in_range.amount[i] = amount2;
        Total_Drinks_data_in_range.amount[index_max] = amount1;

        string name1 = Total_Drinks_data_in_range.name[i];
        string name2 = Total_Drinks_data_in_range.name[index_max];
        Total_Drinks_data_in_range.name[i] = name2;
        Total_Drinks_data_in_range.name[index_max] = name1;


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

    ui->Top_Dished->clearContents();
    for (int i = ui->Top_Dished->rowCount()-1 ; i >= 0  ; i--) ui->Top_Dished->removeRow(i);

    ui->Top_Drinks->clearContents();
    for (int i = ui->Top_Drinks->rowCount()-1 ; i >= 0  ; i--) ui->Top_Drinks->removeRow(i);

    //--------total_order----------//
    int total_order = 0;
    //--------total_order----------//

    for (unsigned int i = 0 ; i < Total_Dishes_data_in_range.name.size() ; i++)
    {
        ui->Top_Dished->insertRow(ui->Top_Dished->rowCount());
        ui->Top_Dished->setItem(i , 0 , new QTableWidgetItem(QString::fromStdString(Total_Dishes_data_in_range.name[i])));
        ui->Top_Dished->setItem(i , 1 , new QTableWidgetItem(QString::number(Total_Dishes_data_in_range.amount[i])));

        //--------total_order----------//
        total_order += Total_Dishes_data_in_range.amount[i];
        //--------total_order----------//
    }

    for (unsigned int i = 0 ; i < Total_Drinks_data_in_range.name.size() ; i++)
    {
        ui->Top_Drinks->insertRow(ui->Top_Drinks->rowCount());
        ui->Top_Drinks->setItem(i , 0 , new QTableWidgetItem(QString::fromStdString(Total_Drinks_data_in_range.name[i])));
        ui->Top_Drinks->setItem(i , 1 , new QTableWidgetItem(QString::number(Total_Drinks_data_in_range.amount[i])));

        //--------total_order----------//
        total_order += Total_Drinks_data_in_range.amount[i];
        //--------total_order----------//
    }

    //00000000000000000000000000000000000000000000000000000000000000000000000000000000000000//
    //00000000000000000000000000000000000000000000000000000000000000000000000000000000000000//

    QFont Format_Fonnt_Bold_Center_16;
    Format_Fonnt_Bold_Center_16.setBold(true);
    Format_Fonnt_Bold_Center_16.setPointSize(16);

    ui->Income_in_range->setText(QString::number(Total_Income_in_selected_range , 'f' ,2));
    ui->Income_in_range->setFont(Format_Fonnt_Bold_Center_16);
    ui->Income_in_range->setAlignment(Qt::AlignCenter);

    ui->Expenses_in_range->setText(QString::number(Total_Expenses_in_selected_range , 'f' ,2));
    ui->Expenses_in_range->setFont(Format_Fonnt_Bold_Center_16);
    ui->Expenses_in_range->setAlignment(Qt::AlignCenter);

    //00000000000000000000000000000000000000000000000000000000000000000000000000000000000000//
    //00000000000000000000000000000000000000000000000000000000000000000000000000000000000000//


    ui->Total_Order_in_range->setText(QString::number(total_order));
    ui->Total_Order_in_range->setFont(Format_Fonnt_Bold_Center_16);
    ui->Total_Order_in_range->setAlignment(Qt::AlignCenter);


}


void analysis::on_comboBox_Scale_mode_currentIndexChanged(int scale_mode)
{
    Show_Chart();
}

