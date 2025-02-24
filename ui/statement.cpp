#include "statement.h"
#include "ui_statement.h"
#include "header/json.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QDateTime>

json statementData;

Statement::Statement(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Statement)
{
    ui->setupUi(this);

    //All Tab Setup
    ui->Statement_Table->verticalHeader()->setVisible(false);
    ui->Statement_Table->horizontalHeader()->setSortIndicatorShown(false);
    ui->Statement_Table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);

    //Income Tab Setup
    ui->Income_Table->verticalHeader()->setVisible(false);
    ui->Income_Table->horizontalHeader()->setSortIndicatorShown(false);
    ui->Income_Table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);

    //Expense Tab Setup
    ui->Expense_Table->verticalHeader()->setVisible(false);
    ui->Expense_Table->horizontalHeader()->setSortIndicatorShown(false);
    ui->Expense_Table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);

    //Add Year Input
    QComboBox *Year_Input = ui->Year_Input;
    getData(statementData, "Statement");
    QSet<int> yearSet;
    for (auto item : statementData) {
        QDate date = QDate::fromString(QString::fromStdString(item[3]), "dd-MM-yyyy");
        yearSet.insert(date.year());
    }
    QList<int> yearList = yearSet.values(); // Convert QSet to QList
    std::sort(yearList.begin(), yearList.end()); //Sort
    // Add to Combobox
    for (int year : yearList) {
        Year_Input->addItem(QString::number(year));
    }
    //Initial Input
    Year_Input->setCurrentText(QString::number(QDate::currentDate().year())); //Year
    ui->Month_ComboBox->setCurrentIndex(QDate::currentDate().month()-1); //Month
    //Connect Function
    connect(ui->Month_ComboBox, &QComboBox::currentTextChanged, this, &Statement::loadData);
    connect(Year_Input, &QComboBox::currentTextChanged, this, &Statement::loadData);

    loadData();
}

Statement::~Statement()
{
    delete ui;
}

void Statement::loadData()
{
    QString selectedMonth = QString::number(ui->Month_ComboBox->currentIndex()+1);
    QString selectedYear = ui->Year_Input->currentText();
    int IntMonth=ui->Month_ComboBox->currentIndex()+1;
    if(IntMonth < 10)selectedMonth="0"+QString::number(IntMonth);
    if (selectedMonth.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a month");
        return;
    }
    ui->Statement_Table->setRowCount(0);
    ui->Income_Table->setRowCount(0);
    ui->Expense_Table->setRowCount(0);
    double totalIncome = 0, totalExpense = 0;
    int row = 0;

    for (const auto& entry : statementData) {
        if (entry.size() < 5) continue;

        QString name = QString::fromStdString(entry[0]);
        int quantity = entry[1];
        double price = entry[2];
        QString date = QString::fromStdString(entry[3]);
        QString time = QTime::fromString(QString::fromStdString(entry[4]),"hh:mm:ss.zzz").toString("hh:mm");

        QStringList dateParts = date.split("-");
        if (dateParts.size() != 3) continue;

        if (dateParts[1] != selectedMonth) continue;
        if (!selectedYear.isEmpty() && dateParts[2] != selectedYear) continue;

        ui->Statement_Table->insertRow(row);

        ui->Statement_Table->setItem(row, 0, new QTableWidgetItem(date));
        ui->Statement_Table->setItem(row, 1, new QTableWidgetItem(time));
        ui->Statement_Table->setItem(row, 2, new QTableWidgetItem(name));
        ui->Statement_Table->setItem(row, 3, new QTableWidgetItem(QString::number(quantity)));
        QTableWidgetItem *priceItem = new QTableWidgetItem(QString::number(price, 'f', 2) + "฿");
        QTableWidgetItem *priceItem2 = new QTableWidgetItem(QString::number(price, 'f', 2) + "฿");

        if (price > 0) { //Income
            priceItem->setBackground(QColor(60, 179, 113)); // Green for income
            priceItem2->setBackground(QColor(60, 179, 113));
            totalIncome += price;

            int rowCount = ui->Income_Table->rowCount();
            ui->Income_Table->insertRow(rowCount);
            ui->Income_Table->setItem(rowCount, 0, new QTableWidgetItem(date));
            ui->Income_Table->setItem(rowCount, 1, new QTableWidgetItem(time));
            ui->Income_Table->setItem(rowCount, 2, new QTableWidgetItem(name));
            ui->Income_Table->setItem(rowCount, 3, new QTableWidgetItem(QString::number(quantity)));
            ui->Income_Table->setItem(rowCount, 4, priceItem2);
        } else { //Expense
            priceItem->setBackground(QColor(255, 99, 71)); // Red for expense
            priceItem2->setBackground(QColor(255, 99, 71));
            totalExpense += -price;

            int rowCount = ui->Expense_Table->rowCount();
            ui->Expense_Table->insertRow(rowCount);
            ui->Expense_Table->setItem(rowCount, 0, new QTableWidgetItem(date));
            ui->Expense_Table->setItem(rowCount, 1, new QTableWidgetItem(time));
            ui->Expense_Table->setItem(rowCount, 2, new QTableWidgetItem(name));
            ui->Expense_Table->setItem(rowCount, 3, new QTableWidgetItem(QString::number(quantity)));
            ui->Expense_Table->setItem(rowCount, 4, priceItem2);
        }

        ui->Statement_Table->setItem(row, 4, priceItem);
        row++;
    }

    double netProfit = totalIncome - totalExpense;

    // set text with comma separator
    ui->Total_Income->setText(QLocale(QLocale::English).toString(totalIncome, 'f', 2) + "฿");
    ui->Total_Expense->setText(QLocale(QLocale::English).toString(totalExpense, 'f', 2) + "฿");
    ui->Net_Balance->setText(QLocale(QLocale::English).toString(netProfit, 'f', 2) + "฿");
}
