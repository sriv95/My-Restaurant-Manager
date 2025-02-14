#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include <ui/opentabledialog.h>

class RestuarantManagement : public QMainWindow
{
    Q_OBJECT

public:
    RestuarantManagement(QWidget *parent = nullptr);
    ~RestuarantManagement();

    // void on_TableBtn_clicked();

private slots:

    void on_RefreshBtn_clicked();

    void SetSelectingTable(QString);

    int GetSelectingTableNo();

    void Changeseats(); //added by fong

    void on_TableBtn_clicked();

    void on_CheckBills_clicked();

    void setMainBtnVisible(bool);

    void on_OpenTableBtn_clicked();

    void onTableReturnValue(const QString &);

    void on_ReserveBtn_clicked();

    void getData();
    void updateTablesStatus();

    // void onReserveReturnValue(const QString &);

    void on_Employee_clicked();
    void on_Stocks_clicked();

    void on_OrderFoodBtn_clicked() ;

private:

    Ui::RestuarantManagementClass ui;
    OpenTableDialog *openTableDialog; // Declare OpenTableDialog as a member

};
