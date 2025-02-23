#include "../header/json.h"
#include <fstream>
#include <iomanip>
#include "ui/jsoncheck.h"
#include <QDir>
#include <QMessageBox>

string PATH = (QDir::currentPath() + "/data.json").toStdString();

void getData(json &Data,string key){
    if(!checkData()){
        QMessageBox Error;
        Error.setText(".json file is not found. Please go set a new one at  \"Open File...\" 😃");
        Error.setIcon(QMessageBox::Warning);
        Error.setWindowTitle("Error!");
        Error.exec();
        return;
    }
    ifstream file(PATH);
    json j = json::parse(file);
    file.close();

    Data = j[key];
}

void getAllData(json &Data){
    if(!checkData()){
        QMessageBox Error;
        Error.setText(".json file is not found. Please go set a new one at  \"Open File...\" 😃");
        Error.setIcon(QMessageBox::Warning);
        Error.setWindowTitle("Error!");
        Error.exec();
        return;
    }

    ifstream file(PATH);
    json j = json::parse(file);
    file.close();
    
    Data = j;
}

void setData(json &Data,string key){
    if(!checkData()){
        QMessageBox Error;
        Error.setText(".json file is not found. Please go set a new one at  \"Open File...\" 😃");
        Error.setIcon(QMessageBox::Warning);
        Error.setWindowTitle("Error!");
        Error.exec();
        return;
    }

    json all;
    getAllData(all);
    ofstream file(PATH);
    all[key]=Data;

    file<<setw(4)<<all;
}

void setAllData(json &Data){
    if(!checkData()){
        QMessageBox Error;
        Error.setText(".json file is not found. Please go set a new one at  \"Open File...\" 😃");
        Error.setIcon(QMessageBox::Warning);
        Error.setWindowTitle("Error!");
        Error.exec();
        return;
    }

    ofstream file(PATH);
    file<<setw(4)<<Data;
}

int lenData(json &Data){
    if(!checkData()){
        QMessageBox Error;
        Error.setText(".json file is not found. Please go set a new one at  \"Open File...\" 😃");
        Error.setIcon(QMessageBox::Warning);
        Error.setWindowTitle("Error!");
        Error.exec();
        return 0;
    }

    int i = 0;
    for (auto it = Data.begin(); it != Data.end(); ++it) {
        i++;
    }
    return i;
}

string updateFilePath(const string& newpath){
    PATH = newpath;

    return newpath;
}

bool checkData(){
    ifstream file(PATH);
    if (file.is_open()) {
        return true;
    }else {

        return false;
    }
}


void newData() {

    json templatedata = json::object();

    templatedata["Employee"] = json::array();
    templatedata["Menus"] = json::array();
    templatedata["Reservation"] = json::array();
    templatedata["Statement"] = json::array();
    templatedata["Stocks"] = json::array();

    json tables = json::array();
    for (int i = 1; i <= 9; ++i) {
        json table = json::object();
        json bills = json::array();
        json menu = json::array();

        menu.push_back("");
        bills.push_back(menu);
        bills.push_back(json::array());
        bills.push_back(json::array());

        table["Bills"] = bills;
        table["No"] = i;
        table["Reserved"] = "";
        table["Seats"] = 0;

        tables.push_back(table);
    }
    templatedata["Tables"] = tables;


    ofstream file("data.json"); //กันเขียนทับ data.json ใน example ตอนใช้ไฟล์อันนั้น
    updateFilePath("data.json"); //ให้ไปอ่านที่ data.json ไม่งั้นจะค้างที่ path ของ open(ถ้าใช้อยู่)
    file<<setw(4)<<templatedata;
}


string getPATH(){
    return PATH;
}

bool forcecloseTriggered = false;

// void makecrash(){ //เพื่อบังคับปิดโปรแกรมทั้งหมดทันที
//     ifstream file("");
//     json j = json::parse(file);
//     file.close();
// }
