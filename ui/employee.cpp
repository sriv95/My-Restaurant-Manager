#include "employee.h"
#include "ui_employee.h"
#include <QDebug>
#include <QDir>
#include <header/json.h>
#include <string>
#include <cstdlib>
#include <QMessageBox>
#include <algorithm>

using namespace std;

const string Employee_Keys[] = {"Name" , "Age" , "Gender" , "Salary" , "Job"}; // ชื่อ key ของข้อมูลของ Employee
const int len_Employee_Keys = 5; // จำนวน key ของข้อมูลของ Employee

employee::employee(QWidget *parent) 
    : QDialog(parent)
    , ui(new Ui::employee)
{
    ui->setupUi(this);
    this->setWindowTitle("Employee");
    Refresh_TableData(); // ทำการ Refresh ข้อมูลในตาราง
    ui->Table->clearSelection(); // ตั้งค่าไม่ให้เลือกแถวใดๆ ในตาราง

    ui->Table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); // ทำให้ตาราง column 0 ขยายตามช่องว่างที่เหลือให้เต็มโดยแบ่งกับ colum ที่ 3 4
    ui->Table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // ทำให้ตาราง column 1 มีขนาดพอดีกับข้อความ
    ui->Table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // ทำให้ตาราง column 2 มีขนาดพอดีกับข้อความ
    ui->Table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch); // ทำให้ตาราง column 3 ขยายตามช่องว่างที่เหลือให้เต็มโดยแบ่งกับ colum ที่ 0 4
    ui->Table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch); // ทำให้ตาราง column 4 ขยายตามช่องว่างที่เหลือให้เต็มโดยแบ่งกับ colum ที่ 0 3

    //=================== sound =====================//
    buttonSound = new QMediaPlayer(this);
    buttonAudio = new QAudioOutput(this);
    buttonSound->setAudioOutput(buttonAudio);
    buttonSound->setSource(QUrl("qrc:/Sounds/Button.mp3"));
    //=================== sound =====================//
}

employee::~employee()
{
    delete ui;
}

//=================== sound =====================//
void employee::playButtonSound()
{
    if (buttonSound->playbackState() == QMediaPlayer::PlayingState) {
        buttonSound->setPosition(0);
    } else {
        buttonSound->play();
    }
}
//=================== sound =====================//

void employee::Refresh_TableData() 
{
    json employee; // สร้างตัวแปร json ชื่อ employee
    getData(employee , "Employee"); // ดึงข้อมูลจากไฟล์ json ที่มี key ชื่อ Employee มาเก็บไว้ในตัวแปร employee
    int number_of_employee = lenData(employee); // นับจำนวนข้อมูลทั้งหมดใน employee

    ui->Table->setRowCount(number_of_employee); // กำหนดจำนวนแถวในตารางเท่ากับจำนวนข้อมูลทั้งหมดใน employee

    for (int i = 0 ; i < number_of_employee ; i++) // วนลูปเพื่อเก็บข้อมูลลงในตาราง
    {
        QString name = QString::fromStdString(employee[i]["Name"]); // สร้างตัวแปร name เพื่อเก็บชื่อของพนักงานที่ i
        QString Age = QString::number(int(employee[i]["Age"])); // สร้างตัวแปร Age เพื่อเก็บอายุของพนักงานที่ i
        QString Gender = QString::fromStdString(employee[i]["Gender"]); // สร้างตัวแปรชื่อ Gender เพื่อเก็บเพศของพนักงานที่ i
        QString Salary = QString::number(double(employee[i]["Salary"]) , 'f' , 2); // สร้างตัวแปร Salary เพื่อเก็บเงินเดือนของพนักงานที่ i
        QString Job = QString::fromStdString(employee[i]["Job"]); // สร้างตัวแปร Job เพื่อเก็บงานของพนักงานที่ i

        ui->Table->setItem(i , 0 , new QTableWidgetItem(name)); // ใส่ชื่อของพนักงานลงในเซลล์ที่ 0 ของแถวที่ i ในตาราง
        ui->Table->setItem(i , 1 , new QTableWidgetItem(Age)); // ใส่อายุของพนักงานลงในเซลล์ที่ 1 ของแถวที่ i ในตาราง
        ui->Table->setItem(i , 2 , new QTableWidgetItem(Gender)); // ใส่เพศของพนักงานลงในเซลล์ที่ 2 ของแถวที่ i ในตาราง
        ui->Table->setItem(i , 3 , new QTableWidgetItem(Salary)); // ใส่เงินเดือนของพนักงานลงในเซลล์ที่ 3 ของแถวที่ i ในตาราง
        ui->Table->setItem(i , 4 , new QTableWidgetItem(Job)); // ใส่งานของพนักงานลงในเซลล์ที่ 4 ของแถวที่ i ในตาราง
    }
}

void employee::on_Refresh_Button_clicked() // เมื่อกานคลิกที่ปุ่ม Refresh
{
    playButtonSound(); // เล่นเสียงเมื่อมีการคลิกที่ปุ่ม
    Refresh_TableData(); // ทำการ Refresh ข้อมูลในตาราง
}

void employee::on_Add_Button_clicked() // เมื่อกานคลิกที่ปุ่ม Add
{
    playButtonSound(); // เล่นเสียงเมื่อมีการคลิกที่ปุ่ม
    ui->Table->insertRow(ui->Table->rowCount()); // เพิ่มแถวในตาราง
}

void employee::on_Delete_Button_clicked() // เมื่อกานคลิกที่ปุ่ม Delete
{
    playButtonSound(); // เล่นเสียงเมื่อมีการคลิกที่ปุ่ม
    QModelIndexList selected_row_list = ui->Table->selectionModel()->selectedRows(); // สร้างตัวแปร selected_row_list เพื่อเก็บแถวที่ถูกเลือก
    int selected_size = selected_row_list.size(); // นับจำนวนแถวที่ถูกเลือก

    if (selected_size <= 0) // ถ้าไม่มีแถวไหนถูกเลือก
    {
        QMessageBox::about(this , "❗warning❗" , "❗Delete failed.❗Please select the row you want to delete by clicking on the first column of the table (the column with row numbers)."); // แสดงข้อความแจ้งเตือน
    }
    else
    {
        vector<int> seleted_row_vector; // สร้าง vector เพื่อเก็บแถวที่ถูกเลือก
        for (int i = 0 ; i < selected_size ; i++) // วนลูปเพื่อเก็บแถวที่ถูกเลือก
        {
            seleted_row_vector.push_back(int(selected_row_list[i].row())); // เก็บแถวที่ถูกเลือกที่อยู่ในตัวแปรประเภท QModelIndex ไปใน vector
        }

        sort(seleted_row_vector.begin(),seleted_row_vector.end()); // เรียงลำดับแถวที่ถูกเลือกจากน้อยไปมากโดยใช้ function sort ของ algorithm

        for (int i = seleted_row_vector.size() - 1 ; i >= 0  ; i--) // วนลูปเพื่อลบแถวที่ถูกเลือกจากแถวสุดท้ายไปจนถึงแถวแรก
        {
            ui->Table->removeRow(seleted_row_vector[i]); // ลบแถวที่อยู่ใน vector ที่ i
        }
    }
}

bool employee::Check_empty_cell_in_table() // return True ถ้ามีช่องไหนว่าง | return false ถ้าไม่มีช่องไหนว่าง
{
    int Number_of_employee_OnTable = ui->Table->rowCount(); // นับจำนวนแถวทั้งหมดในตาราง
    for (int i = 0 ; i < Number_of_employee_OnTable ; i++) // วนลูปเพื่อตรวจสอบช่องว่างในแต่ละแถว
    {
        for (int j = 0 ; j < 5 ; j++) // วนลูปเพื่อตรวจสอบช่องว่างในแต่ละเซลล์
        {
            if (ui->Table->item(i,j) == nullptr or ui->Table->item(i,j)->text().isEmpty()) // ถ้าเซลล์ที่ i,j ว่าง
            {
                QMessageBox::about(this , "❗warning❗" , "❗Save failed.❗There is an empty cell in row " + QString::number(i+1) + "."); // แสดงข้อความแจ้งเตือน
                return true; // คืนค่า True
            }
        }
    }
    return false; // ถ้าไม่มีช่องว่างคืนค่า False
}

bool employee::Check_Correct_DataType_in_cell() // return True ถ้า DataType ทุกช่องสามารถใช้งานได้ | return false ถ้ามี cell ที่ DataType ไม่เหมาะสม
{
    bool test_value; // สร้างตัวแปร test_value ประเภท bool เพื่อเก็บค่าทดสอบว่าข้อมูลในเซลล์เป็นชนิดที่ถูกต้องหรือไม่

    int Number_of_employee_OnTable = ui->Table->rowCount(); // นับจำนวนแถวทั้งหมดในตาราง
    for (int i = 0 ; i < Number_of_employee_OnTable ; i++) // วนลูปเพื่อตรวจสอบ DataType ในแต่ละเซลล์
    {
        for (int j = 1 ; j <=3 ; j = j+2) // วนลูปเพื่อตรวจสอบ DataType ในเซลล์ที่ 1 และ 3
        {
            switch (j) {
            case 1: // กรณีที่ต้องการตรวจสอบ DataType ในเซลล์ที่ 1 (Age)
                ui->Table->item(i,j)->text().toInt(&test_value); // ทดสอบว่าข้อมูลในเซลล์เป็นชนิด int หรือไม่
                if (test_value == false) // ถ้าข้อมูลในเซลล์ไม่ใช่ int
                {
                    QMessageBox::about(this , "❗warning❗" , "❗Save failed.❗ Invalid data type in row " + QString::number(i+1) + ", column " + QString::number(j+1) + "(Age). It should be an integer." ); // แสดงข้อความแจ้งเตือน
                    return false; // คืนค่า False
                }
                break; 
            case 3: // กรณีที่ต้องการตรวจสอบ DataType ในเซลล์ที่ 3 (Salary)
                ui->Table->item(i,j)->text().toDouble(&test_value); // ทดสอบว่าข้อมูลในเซลล์เป็นชนิด double หรือไม่
                if (test_value == false) // ถ้าข้อมูลในเซลล์ไม่ใช่ double
                {
                    QMessageBox::about(this , "❗warning❗" , "❗Save failed.❗ Invalid data type in row " + QString::number(i+1) + ", column " + QString::number(j+1) + "(Salary). It should be a number." ); // แสดงข้อความแจ้งเตือน
                    return false; // คืนค่า False
                }
                break;
            }
        }
    }

    return true; // ถ้า DataType ทุกช่องสามารถใช้งานได้ คืนค่า True
}

void employee::on_Save_Button_clicked() // เมื่อกานคลิกที่ปุ่ม Save
{
    playButtonSound(); // เล่นเสียงเมื่อมีการคลิกที่ปุ่ม

    int Number_of_employee_OnTable = ui->Table->rowCount(); // นับจำนวนแถวทั้งหมดในตาราง

    if (Check_empty_cell_in_table() == false) // ถ้าไม่มีช่องว่างในตาราง
    {
        if (Check_Correct_DataType_in_cell() == true) // ถ้า DataType ทุกช่องสามารถใช้งานได้
        {
            json employee; // สร้างตัวแปร json ชื่อ employee
            employee.clear(); // ลบข้อมูลใน employee ทั้งหมด
            setData(employee , "Employee"); // ลบข้อมูลในไฟล์ json ที่มี key ชื่อ Employee ทั้งหมด

            for (int i = 0 ; i < Number_of_employee_OnTable ; i++) // วนลูปเพื่อเก็บข้อมูลจากตารางลงในตัวแปร employee เพื่อทำการบันทึกลงในไฟล์ json
            {

                string Name = ui->Table->item(i , 0)->text().toStdString(); // สร้างตัวแปร Name เพื่อเก็บชื่อของพนักงานที่ i
                int Age = ui->Table->item(i , 1)->text().toInt(); // สร้างตัวแปร Age เพื่อเก็บอายุของพนักงานที่ i
                string Gender = ui->Table->item(i , 2)->text().toStdString(); // สร้างตัวแปรชื่อ Gender เพื่อเก็บเพศของพนักงานที่ i
                double Salary = ui->Table->item(i , 3)->text().toDouble(); // สร้างตัวแปร Salary เพื่อเก็บเงินเดือนของพนักงานที่ i
                string Job = ui->Table->item(i , 4)->text().toStdString(); // สร้างตัวแปร Job เพื่อเก็บงานของพนักงานที่ i

                employee[i]["Name"] = Name; // เก็บชื่อของพนักงานที่ i ลงในตัวแปร employee
                employee[i]["Age"] = Age; // เก็บอายุของพนักงานที่ i ลงในตัวแปร employee
                employee[i]["Gender"] = Gender; // เก็บเพศของพนักงานที่ i ลงในตัวแปร employee
                employee[i]["Salary"] = Salary; // เก็บเงินเดือนของพนักงานที่ i ลงในตัวแปร employee
                employee[i]["Job"] = Job; // เก็บงานของพนักงานที่ i ลงในตัวแปร employee
            }

            setData(employee , "Employee"); // ทำการบันทึกข้อมูลในตัวแปร employee ลงในไฟล์ json ที่มี key ชื่อ Employee

            QMessageBox Save_successful_Message; // สร้างตัวแปร Save_successful_Message ประเภท QMessageBox
            Save_successful_Message.setWindowTitle("🎉🥳✅✅🥳🎉"); // ตั้ง title ของ QMessageBox
            Save_successful_Message.setText("✅🟢🥳🎉-Save successful-🎉🥳🟢✅"); // บอกว่า Save สำเร็จ
            Save_successful_Message.setWindowFlags(Qt::Popup); // ตั้งค่าให้ขึ้นกล่องข้อความแบบ Popup คือกดตั้งไหนในหน้าจอก็ได้ ไม่ต้องกดกากบาทเพื่อปิดหน้าต่าง QMessageBox
            Save_successful_Message.exec(); // แสดง MessageBox

            // QMessageBox::about(this , "🎉🥳✅✅🥳🎉" , "✅🟢🥳🎉-Save successful-🎉🥳🟢✅");
        }
    }
}
