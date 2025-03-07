#include "mainwindow.h"
#include "func/tables.h"
#include <QMessageBox>
#include <QTextStream>
#include <QDateTime>
#include <header/json.h>
#include <QTimer>
#include <QDateTime>
#include <QMediaPlayer>
#include <QAudioOutput>

#include <ui/reserve.h>
#include <ui/employee.h>
#include <ui/opentabledialog.h>
#include <ui/reserve.h>
#include <ui/stockwindow.h>
#include <ui/statement.h>
#include <ui/editmenu.h>
#include <ui/orderfood.h>
#include <ui/analysis.h>
#include "ui/jsoncheck.h"
#include "ui/orderstock.h"
#include "ui/configwindow.h"

QMap <QString,QPalette> PalettesMap;

json restaurantData;

int Table_Count = 9;

bool isDarkMode() {
    QSettings WindowsSettings(R"(HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)", QSettings::NativeFormat);
    bool isDarkMode = WindowsSettings.value("AppsUseLightTheme", 1).toInt() == 0;  // ถ้าเป็น 0 แสดงว่าใช้ Dark Mode
    // qDebug()<<"isDarkMode = "<<isDarkMode<<")";
    return isDarkMode;
}

void AddPalettesMap(QString ThemeName,QString Window,QString WindowText,QString Base,QString Text,QString Button,QString ButtonText,QString Highlight,QString HighlightedText,QString Dark,QString Mid,QString AlternateBase){
    QPalette Palette;
    Palette.setColor(QPalette::Window, QColor(Window));
    Palette.setColor(QPalette::WindowText, QColor(WindowText)); //label
    Palette.setColor(QPalette::Base, QColor(Base));
    Palette.setColor(QPalette::Text, QColor(Text)); // สีตัวหนังสือทั่วไปใน Base
    Palette.setColor(QPalette::Button, QColor(Button));
    Palette.setColor(QPalette::ButtonText, QColor(ButtonText));
    Palette.setColor(QPalette::Highlight, QColor(Highlight));
    Palette.setColor(QPalette::HighlightedText, QColor(HighlightedText));
    Palette.setColor(QPalette::Dark, QColor(Dark)); // ตัวหนังสืออ่อนของปฎิทิน
    Palette.setColor(QPalette::Mid, QColor(Mid)); // สีขอบ
    Palette.setColor(QPalette::AlternateBase, QColor(AlternateBase)); //หัวแถวปฎิทิน
    PalettesMap[ThemeName] = Palette;
}

void SetPalette(bool darkmode_on){



// AddPalettesMap("","#Window","#WindowText","#Base","#Text","#Button","#ButtonText","#Highlight","#HighlightedText","#Dark","#Mid","#AlternateBase");

    AddPalettesMap("Light","#f0f0f0","#000000","#ffffff","#000000","#dfdfdf","#000000","#0078d7","#ffffff","#a0a0a0","#a0a0a0","#f5f5f5");

    AddPalettesMap("Dark","#1e1e1e","#ffffff","#2d2d2d","#ffffff","#3c3c3c","#ffffff","#0078d7","#ffffff","#a0a0a0","#a0a0a0","#3c3c3c");

    AddPalettesMap("Purple","#716cbf","#000000","#ccd6d8","#000000","#faab1d","#000000","#3ec387","#ffffff","#a0a0a0","#3e4096","#ffffff");

    AddPalettesMap("Red","#990000","#ffffff","#252728","#ffffff","#d98d16","#ffffff","#2a2979","#eba4a7","#a0a0a0","#a0a0a0","#3c3c3c");

    AddPalettesMap("Green","#00b39b","#ffffff","#263331","#ffffff","#394241","#00b39b","#4e3b99","#ffffff","#a0a0a0","#00e696","#3c3c3c");

    AddPalettesMap("Mule","#943D2C","#000000","#dfd0bb","#474344","#CC7952","#E5BD77","#474344","#CC7952","#a0a0a0","#a0a0a0","#ffffff");

    //AddPalettesMap("Red2","#350a0e","#e9c5b5","#3c3c3c","#ffffff","#e1bb3e","#350a0e","#ffffff","#000000","#a0a0a0","#a0a0a0","#3c3c3c");

    // AddPalettesMap("","#","#","#","#","#","#","#","#");

    QString theme;
    if(settings->value("theme").toString()!="") theme = settings->value("theme").toString();
    else {
        if(darkmode_on == false) theme="Light"; //Default Light mode
        else theme="Dark"; //Default Dark mode
    }

    settings->setValue("theme",theme); //save to config
    QApplication::setPalette(PalettesMap[theme]);
}



void RestuarantManagement::showError(QString text){
    QMessageBox Error;
    Error.setText(text);
    Error.setIcon(QMessageBox::Warning);
    Error.setWindowTitle("Error!");
    Error.exec();
}

void RestuarantManagement::setMainBtnVisible(bool tf){
    ui.CheckBills->setVisible(tf);
    ui.ReserveBtn->setVisible(tf);
    ui.OpenTableBtn->setVisible(tf);
}

RestuarantManagement::RestuarantManagement(QWidget *parent)
    : QMainWindow(parent)

{
    ui.setupUi(this);
    SetPalette(isDarkMode());
    QApplication::setFont(settings->value("font").value<QFont>());
    on_backtosetup_clicked();

    if(!checkData()){
        return;
    }

    updateTablesStatus();
    for(int i=1;i<=Table_Count;++i){
        QString btnName = QString("Table_").append(QString::number(i));
        QPushButton *button = this->findChild<QPushButton *>(btnName);

        if(button) connect(button, &QPushButton::clicked, this, &RestuarantManagement::on_TableBtn_clicked);
        else  qDebug()<<"Error: Button Not Found (Button Name: "<<btnName<<")";
    }

    ui.Receipt->hide();
    ui.OrderFoodBtn->hide();
    setMainBtnVisible(false);

    // ตั้งค่าเสียงปุ่ม
    buttonSound = new QMediaPlayer(this);
    buttonAudio = new QAudioOutput(this);
    buttonSound->setAudioOutput(buttonAudio);
    buttonSound->setSource(QUrl("qrc:/Sounds/Button.mp3"));

    // ตั้งค่าเสียง check bill
    checkbillSound = new QMediaPlayer(this);
    checkbillAudio = new QAudioOutput(this);
    checkbillSound->setAudioOutput(checkbillAudio);
    checkbillSound->setSource(QUrl("qrc:/Sounds/checkBill.mp3"));

    QTimer *timer=new QTimer(this);
    connect (timer ,SIGNAL(timeout()),this,SLOT(showTime()));
    timer->start();
}

void RestuarantManagement::playButtonSound()
{
    if (buttonSound->playbackState() == QMediaPlayer::PlayingState) {
        buttonSound->setPosition(0);
    } else {
        buttonSound->play();
    }
}

void RestuarantManagement::playCheckbillSound()
{
    if (checkbillSound->playbackState() == QMediaPlayer::PlayingState) {
        checkbillSound->setPosition(0);
    } else {
        checkbillSound->play();
    }
}


void  RestuarantManagement::showTime()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeText = dateTime.toString("dd/MM/yyyy HH:mm:ss");

    ui.Digital_clock->setText(dateTimeText);
}


RestuarantManagement::~RestuarantManagement()
{}

int RestuarantManagement::GetSelectingTableNo(){
    return ui.SelectingTable->text().toInt();
}

void RestuarantManagement::SetSelectingTable(QString no){
    for(int i =1 ; i <=Table_Count ; i++)
    {
        QString btnName = QString("Table_").append(QString::number(i));
        QPushButton *button = this->findChild<QPushButton *>(btnName);

        QPalette palette = QApplication::palette();
        QColor buttonColor = palette.color(QPalette::Button);

        palette.setColor(QPalette::Button, buttonColor);
        button->setPalette(palette);
    }
    if (ui.Receipt->isVisible() && ui.CheckBills->text() == "Confirm Payment") {
        ui.Receipt->hide();
        ui.CheckBills->setText("Check Bills");
    }

    if(ui.SelectingTable->text()!=no) {
        ui.Receipt->show();
        ui.SelectingTable->setText(QString(no));
        setMainBtnVisible(true);
        ui.OrderFoodBtn->show();
        updateReserveButtonText(no.toInt()); //TT
        QString btnName = QString("Table_").append(no);
        QPushButton *button = this->findChild<QPushButton *>(btnName);

        QPalette palette = QApplication::palette();
        QColor buttonColor = palette.color(QPalette::Button);
        QColor buttonColorpressed = buttonColor.darker(140);

        palette.setColor(QPalette::Button, buttonColorpressed);
        button->setPalette(palette);

        // อัปเดตสถานะปุ่ม ReserveBtn
        json Tables;
        ::getData(Tables, "Tables");

        int tableNo = no.toInt();
        if (tableNo > 0 && Tables[tableNo - 1]["Seats"].get<int>() == 0) {
            ui.ReserveBtn->show();  // ถ้าโต๊ะว่าง = แสดงปุ่ม
        } else {
            ui.ReserveBtn->hide();  // ถ้ามีคนอยู่ที่โต๊ะ = ซ่อนปุ่ม
        }
    }
    else {
        ui.SelectingTable->setText(QString('0'));
        ui.OrderFoodBtn->hide();
        setMainBtnVisible(false);
    }
}

void RestuarantManagement::on_TableBtn_clicked()
{
    playButtonSound();
    if (ui.Receipt->isVisible() && ui.CheckBills->text() == "Confirm Payment") {
        ui.Receipt->hide();
        ui.CheckBills->setText("Check Bills");
    }

    QPushButton *buttonSender = qobject_cast<QPushButton *>(sender());
    QString buttonName = buttonSender->objectName();
    QString table_no = buttonName.split("_").last();

    SetSelectingTable(table_no);
    on_CheckBills_clicked();
}
void RestuarantManagement::getData()
{
    getAllData(restaurantData);
    Changeseats();

}

void RestuarantManagement::updateTablesStatus()
{
    json Tables;
    ::getData(Tables, "Tables");

    for (int i = 1; i <= Table_Count; ++i) {
        int seat = Tables[i - 1]["Seats"];
        QString Reserved = QString::fromStdString(Tables[i - 1]["Reserved"]);
        QString btnName = QString("Table_").append(QString::number(i));
        QPushButton *button = this->findChild<QPushButton *>(btnName);

        if (button) {
            if (seat != 0) {
                button->setText("Table#" + QString::number(i) + "\n" + QString::number(seat) + "👤");
            } else if (Reserved != "") {
                json Reservation;
                ::getData(Reservation, "Reservation");
                for (auto item : Reservation) {
                    if (item[0] == i && item[1] == Reserved.toStdString()) {
                        button->setText("Table#" + QString::number(i) + "\nReserved: " + Reserved + "\n" +
                                        QString::fromStdString(item[2]) + "\n" + QString::fromStdString(item[3]));
                        break;
                    }
                }
            } else {
                button->setText("Table#" + QString::number(i) + "\nAvailable");
            }
        } else {
            qDebug() << "Error: Button Not Found (Button Name: " << btnName << ")";
        }
    }

    // แสดงผลของปุ่ม ReserveBtn
    int selectedTable = GetSelectingTableNo();
    if (selectedTable > 0 && Tables[selectedTable - 1]["Seats"].get<int>() == 0) {
        ui.ReserveBtn->show();
    } else {
        ui.ReserveBtn->hide();
    }
}




void RestuarantManagement::on_RefreshBtn_clicked()
{
    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender()); // retrieve the button you have clicked
    if(buttonSender->text()=="Refresh") playButtonSound();
    ui.SelectingTable->setText(QString('0'));
    QString table_no = "0";
    ui.Receipt->hide();
    ui.CheckBills->setText("Check Bills");
    SetSelectingTable(table_no);
    updateTablesStatus(); //
}

void RestuarantManagement::on_CheckBills_clicked()
{
    // ประกาศตัวแปรก่อนใช้งาน
    QString currentText = ui.CheckBills->text();

    getData();

    ui.Receipt_DateTime->setText(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss"));

    int selectedTableNumber = GetSelectingTableNo();
    if (selectedTableNumber == 0) {
        ui.Receipt->hide();
        return;
    }

    json Bills = restaurantData["Tables"][selectedTableNumber - 1]["Bills"];
    json emptyBills = json::array({json::array({""}), json::array(), json::array()});

    if (ui.CheckBills->text() == "Confirm Payment") {
        if (Bills == emptyBills || Bills[0].empty()) {
            QMessageBox::warning(this, "No Orders", "No bills found for this table. Please add an order before confirming payment.");
            return;
        }
        playCheckbillSound();
        vector<string> billNames;
        vector<int> billPrices;
        getBills(Bills, restaurantData["Menus"], billNames, billPrices);

        QString currentDate = QDateTime::currentDateTime().toString("dd-MM-yyyy");
        QString currentTime = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");

        unordered_map<string, pair<int, int>> combinedBills;

       for (size_t i = 0; i < billNames.size(); ++i) {
            combinedBills[billNames[i]].first += 1;
            combinedBills[billNames[i]].second += billPrices[i];
        }

        for (const auto& bill : combinedBills) {
            json statementItem = json::array();
            statementItem.push_back(bill.first);
            statementItem.push_back(bill.second.first);
            statementItem.push_back(bill.second.second);
            statementItem.push_back(currentDate.toStdString());
            statementItem.push_back(currentTime.toStdString());
            restaurantData["Statement"].push_back(statementItem);
        }

        restaurantData["Tables"][selectedTableNumber - 1]["Seats"] = 0;
        restaurantData["Tables"][selectedTableNumber - 1]["Reserved"] = "";
        restaurantData["Tables"][selectedTableNumber - 1]["Bills"] = emptyBills;

        setAllData(restaurantData);

        ui.Receipt->hide();
        ui.CheckBills->setText("Check Bills");
        on_RefreshBtn_clicked();
        return;
    }

    bool Receipt_Status = !ui.Receipt->isVisible();
    ui.Receipt->setVisible(Receipt_Status);
    ui.Receipt_TableNo->setText(QString("Table#%1").arg(selectedTableNumber));

    if (Receipt_Status) {
        if (Bills == emptyBills) {
            ui.ReceiptListName->clear();
            ui.ReceiptListPrice->clear();
            ui.Receipt_Total->setText("0 Baht");
        } else {
            vector<string> billNames;
            vector<int> billPrices;
            getBills(Bills, restaurantData["Menus"], billNames, billPrices);

            ui.ReceiptListName->clear();
            ui.ReceiptListPrice->clear();
            int totalAmount = 0;

            for (size_t i = 0; i < billNames.size(); ++i) {
                QString menuName = QString::fromStdString(billNames[i]);
                int basePrice = 0;
                for (auto& menu : restaurantData["Menus"]) {
                    if (menu[0] == billNames[i]) {
                        basePrice = menu[1].get<int>();
                        break;
                    }
                }

                int quantity = 0, extra = 0;
                if (Bills.is_array() && Bills.size() == 3) {
                    if (i < Bills[1].size()) quantity = Bills[1][i].get<int>();
                    if (i < Bills[2].size()) extra = Bills[2][i].get<int>();
                }

                QString priceDetail = QString("%1 Baht x%2").arg(basePrice).arg(quantity);
                if (extra != 0) priceDetail += QString(" + %1 Baht").arg(extra);

                ui.ReceiptListName->addItem(menuName);

                //ราคาชิดขวา
                QListWidgetItem *priceItem = new QListWidgetItem(priceDetail);
                priceItem->setTextAlignment(Qt::AlignRight);
                ui.ReceiptListPrice->addItem(priceItem);

                totalAmount += (basePrice * quantity) + extra;
            }


            ui.Receipt_Total->setText(QString::number(totalAmount) + " Baht");
        }
        playButtonSound();
        ui.CheckBills->setText("Confirm Payment");
    } else {
        ui.CheckBills->setText("Check Bills");
    }
}

void RestuarantManagement::on_OpenTableBtn_clicked()
{
    playButtonSound();
    OpenTableDialog OpenTableDialog;
    OpenTableDialog.setModal(true);
    OpenTableDialog.setWindowTitle("Open Table");
    connect(&OpenTableDialog, &OpenTableDialog::returnValue, this, &RestuarantManagement::onTableReturnValue);
    OpenTableDialog.exec();
}

void RestuarantManagement::onTableReturnValue(const QString &data){
    json Tables;
    ::getData(Tables,"Tables");
    int No = GetSelectingTableNo() - 1 ;
    int seat = data.toInt(); //handle unused variable
    // if(seat > 4 or seat < 0)
    // {
    //     qDebug() << "Error: Number of seats. Enter the number of seats again.";
    //     seat = 0;
    // }
    //qDebug() << "Seats: " << seat << "TableNo: " <<GetSelectingTableNo(); //
    Tables [No]["Seats"] = seat;

    setData(Tables, "Tables");
    updateTablesStatus();

}
void RestuarantManagement::Changeseats() //แก้บัค
{
    json Tables;
    ::getData(Tables,"Tables");

    int No = GetSelectingTableNo() - 1 ;
    if(No >= 0)
    {
    if(Tables[No]["Seats"] != 0) ui.OpenTableBtn->setText("Change seat");
    else ui.OpenTableBtn->setText("Open Table");
    }
}


void RestuarantManagement::on_ReserveBtn_clicked()
{
    playButtonSound();
    int tableNo = GetSelectingTableNo();
    if (ui.ReserveBtn->text() == "Unreserve") {
        removeReservation(tableNo);
        ui.ReserveBtn->setText("Reserve");
    } else {
        reserve reserve(tableNo);
        reserve.setModal(true);
        reserve.setWindowTitle("Reserve");
        reserve.exec();
    }
    on_RefreshBtn_clicked();
}

bool RestuarantManagement::isTableReserved(int tableNo) {
    json reservations;
    ::getData(reservations, "Reservation");

    for (const auto &reservation : reservations) {
        if (reservation[0] == tableNo) {
            return true;
        }
    }
    return false;
}



void RestuarantManagement::updateReserveButtonText(int tableNo) {
    if (isTableReserved(tableNo)) {
        ui.ReserveBtn->setText("Unreserve");
    } else {
        ui.ReserveBtn->setText("Reserve");
    }
}

void RestuarantManagement::removeReservation(int tableNo) {
    json allData;
    getAllData(allData);

    json updatedReservations = json::array();
    for (const auto &reservation : allData["Reservation"]) {
        if (reservation[0] != tableNo) {
            updatedReservations.push_back(reservation);
        }
    }
    allData["Reservation"] = updatedReservations;

    for (auto &table : allData["Tables"]) {
        if (table["No"] == tableNo) {
            table["Reserved"] = "";
        }
    }

    setAllData(allData);

    qDebug() << "Reservation for table" << tableNo << "removed.";
}

// void RestuarantManagement::onReserveReturnValue(const QString &data){

// }

void RestuarantManagement::on_Employee_clicked()
{
    playButtonSound();
    employee employee(this);
    employee.exec();
}

void RestuarantManagement::on_Stocks_clicked()
{
    playButtonSound();
    StockWindow *stockWin = new StockWindow(this);


    stockWin->setWindowFlags(Qt::Window);


    stockWin->show();
    stockWin->raise();
    stockWin->activateWindow();
}

void RestuarantManagement::on_EditMenu_clicked()
{
    //playButtonSound();
    editmenu editmenu(this);
    editmenu.exec();
}


void RestuarantManagement::on_Statement_clicked()
{
    playButtonSound();
    Statement stateWin(this);
    stateWin.setWindowTitle("Statement");
    stateWin.exec();
}

void RestuarantManagement::on_OrderFoodBtn_clicked()
{
    playButtonSound();
    int tableNo = GetSelectingTableNo();

    json restaurantData;
    getAllData(restaurantData);

    
    json &tableData = restaurantData["Tables"][tableNo - 1];

    // added 15/2/25
    if (tableData["Seats"].get<int>() == 0) {
        QMessageBox::warning(this, "No customers",
                             "No customers are seated at this table.");
        return;
    }

    json &stocks = restaurantData["Stocks"];

    OrderFoodDialog orderDialog(this);

    json menuData;
    ::getData(menuData, "Menus");
    orderDialog.loadMenu(menuData);

    if (orderDialog.exec() == QDialog::Accepted) {
        QString food = orderDialog.getSelectedFood();
        int quantity = orderDialog.getQuantity();
        int extraPrice = orderDialog.getExtraPrice();

        for (const auto &menu : menuData) {
            if (menu[0] == food.toStdString()) {
                auto ingredients = menu[3];
                auto amounts = menu[4];
                bool hasEnoughStock = true;

                for (size_t i = 0; i < ingredients.size(); ++i) {
                    std::string ingredient = ingredients[i];
                    double amountNeeded = amounts[i].get<double>() * quantity;

                    for (auto &stock : stocks) {
                        if (stock[0] == ingredient) {
                            double remainingStock = stock[1].get<double>() - amountNeeded;
                            if (remainingStock < 0) {
                                hasEnoughStock = false;
                                QMessageBox::warning(this, "Insufficient Stock",
                                                     QString("Not enough stock for %1. Remain: %2 in stock")
                                                         .arg(QString::fromStdString(ingredient))
                                                         .arg(stock[1].get<double>()));
                                break;
                            }
                        }
                    }
                    if (!hasEnoughStock) break;
                }

                if (!hasEnoughStock) {
                    return;
                }

                for (size_t i = 0; i < ingredients.size(); ++i) {
                    std::string ingredient = ingredients[i];
                    double amountNeeded = amounts[i].get<double>() * quantity;

                    for (auto &stock : stocks) {
                        if (stock[0] == ingredient) {
                            double remainingStock = stock[1].get<double>() - amountNeeded;
                            stock[1] = std::max(0.0, remainingStock);

                            if (remainingStock < 10) {
                                QMessageBox::warning(this, "Low Stock",
                                                     QString("Low stock warning: %1 is running low (remaining: %2 in stock).")
                                                         .arg(QString::fromStdString(ingredient))
                                                         .arg(remainingStock));
                            }
                            break;
                        }
                    }
                }
                break;
            }
        }

        json &tableBills = tableData["Bills"];
        json emptyBills = json::array({json::array({""}), json::array(), json::array()});

        if (tableBills == emptyBills || (tableBills[0].size() == 1 && tableBills[0][0] == "")) {
            tableBills[0].clear();
        }

        tableBills[0].push_back(food.toStdString());
        tableBills[1].push_back(quantity);
        tableBills[2].push_back(extraPrice);

        setAllData(restaurantData);
        updateTablesStatus();
    }
}

void RestuarantManagement::on_Analysis_clicked()
{
    // json Data;
    // ::getAllData(Data);
    // if(Data["Statements"].size()<=0&&Data["Menus"].size()<=0) {
    //     showError(".json file data Statements or Menus is empty");
    //     return;}
    playButtonSound();
    analysis analysis(this);
    analysis.exec();
}

void RestuarantManagement::on_backtosetup_clicked()
{
    while(true){
        jsoncheck jsonCheck(this);
        jsonCheck.setWindowTitle("File Configuration");
        jsonCheck.exec();
        QPushButton *buttonSender = qobject_cast<QPushButton *>(sender());
        if(buttonSender!=NULL&&buttonSender->objectName()=="backtosetup") on_RefreshBtn_clicked();
        if(forcecloseTriggered){
            QTimer::singleShot(0,qApp,SLOT(quit()));
            return;
        }

        if(checkData()){
            break;
        }
    }
}

void RestuarantManagement::on_OrderStock_clicked()
{
    playButtonSound();
    OrderStock *orderstock = new OrderStock(this);
    orderstock->setWindowTitle("Order Stock");
    orderstock->exec();
}


void RestuarantManagement::on_Config_clicked()
{
    for(int i =1 ; i <=Table_Count ; i++)
    {
        QString btnName = QString("Table_").append(QString::number(i));
        QPushButton *button = this->findChild<QPushButton *>(btnName);

        button->setPalette(QPalette());
    }

    configwindow *config = new configwindow(this,PalettesMap);
    config->setWindowTitle("Configuration");
    connect(config, &configwindow::returnValue, this, &RestuarantManagement::on_Config_returnValue);
    updateTablesStatus();
    config->exec();
    QApplication::setPalette(PalettesMap[settings->value("theme").toString()]);
    QApplication::setFont(settings->value("font").value<QFont>());
}

void RestuarantManagement::on_Config_returnValue(const QString &data){
    ui.label->setText(data);
}
