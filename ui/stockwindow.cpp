#include "stockwindow.h"
#include "ui_stockwindow.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QSet>
#include <QList>


StockWindow::StockWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StockWindow)
{
    ui->setupUi(this);

    ui->tableStocks->verticalHeader()->setVisible(false);
    ui->tableStocks->horizontalHeader()->setSortIndicatorShown(false);


    ui->tableStocks->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableStocks->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableStocks->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    connect(ui->tableStocks, &QTableWidget::itemChanged, this, &StockWindow::on_tableWidget_itemChanged);
    loadfromjson();

    buttonSound = new QMediaPlayer(this);
    buttonAudio = new QAudioOutput(this);
    buttonSound->setAudioOutput(buttonAudio);
    buttonSound->setSource(QUrl("qrc:/Sounds/Button.mp3"));
}

StockWindow::~StockWindow()
{
    delete ui;
}

void StockWindow::playButtonSound()
{
    if (buttonSound->playbackState() == QMediaPlayer::PlayingState) {
        buttonSound->setPosition(0);
    } else {
        buttonSound->play();
    }
}

void StockWindow::loadfromjson()
{
    getAllData(stockJson);
    refreshTable();
}



void StockWindow::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    int row = item->row();
    int col = item->column();

    if (col == 1) {  // Name Column
        stockJson["Stocks"][row][0] = item->text().toStdString();
    }
    else if (col == 2) {  // Quantity Column
        stockJson["Stocks"][row][1] = item->text().toDouble();
    }
}



void StockWindow::refreshTable()
{
    ui->tableStocks->clearContents();
    ui->tableStocks->setRowCount(stockJson["Stocks"].size());
    for (unsigned long long int i = 0; i < stockJson["Stocks"].size(); ++i) {
        // No. Column (Auto Numbering) - Read Only
        QTableWidgetItem *numItem = new QTableWidgetItem(QString::number(i + 1));
        numItem->setFlags(numItem->flags() & ~Qt::ItemIsEditable);
        ui->tableStocks->setItem(i, 0, numItem);

        // Name Column - Editable
        QTableWidgetItem *nameItem = new QTableWidgetItem(QString::fromStdString(stockJson["Stocks"][i][0]));
        ui->tableStocks->setItem(i, 1, nameItem);

        // Quantity Column - Editable
        QTableWidgetItem *qtyItem = new QTableWidgetItem(QString::number(stockJson["Stocks"][i][1].get<double>()));
        qtyItem->setTextAlignment(Qt::AlignRight);  // Right-align quantity
        ui->tableStocks->setItem(i, 2, qtyItem);
    }
}



void StockWindow::on_AddBtn_clicked()
{
    playButtonSound();
    stockJson["Stocks"].push_back({"New Stock", 0});
    refreshTable();
}



void StockWindow::on_DeleteBtn_clicked()
{
    playButtonSound();
    QList<QTableWidgetItem*> selectedItems = ui->tableStocks->selectedItems();

    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "Delete Error", "No item selected.");
        return;
    }

    QSet<int> rowsToDelete;

    for (QTableWidgetItem* item : selectedItems) {
        rowsToDelete.insert(item->row());
    }

    QList<int> rowsList = QList<int>::fromVector(rowsToDelete.values());
    std::sort(rowsList.begin(), rowsList.end(), std::greater<int>());

    for (int row : rowsList) {
        if (row >= 0 && row < stockJson["Stocks"].size()) {
            stockJson["Stocks"].erase(stockJson["Stocks"].begin() + row);
        }
    }

    refreshTable();
}



void StockWindow::savetojson()
{
    setAllData(stockJson);

    // Create a QMessageBox object
    QMessageBox Save_Message;
    Save_Message.setWindowTitle("Save Successful");
    Save_Message.setText("Data has been successfully saved to the file.");
    Save_Message.setIcon(QMessageBox::Information);
    Save_Message.setWindowFlags(Qt::Popup);  // Set as a popup window
    Save_Message.exec();  // Execute the message box
}



void StockWindow::on_SaveBtn_clicked()
{
    playButtonSound();
    savetojson();
}
