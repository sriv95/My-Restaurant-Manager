#include "orderfood.h"
#include <QMessageBox>

OrderFoodDialog::OrderFoodDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Order Food");

    menuComboBox = new QComboBox();
    priceLabel = new QLabel("Price: 0 Baht");
    quantitySpinBox = new QSpinBox();
    extraPriceLineEdit = new QLineEdit();
    confirmButton = new QPushButton("Add to Bill");

    quantitySpinBox->setMinimum(1);
    quantitySpinBox->setValue(1);
    extraPriceLineEdit->setPlaceholderText("Extra Price (optional)");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(menuComboBox);
    layout->addWidget(priceLabel);
    layout->addWidget(quantitySpinBox);
    layout->addWidget(extraPriceLineEdit);
    layout->addWidget(confirmButton);
    setLayout(layout);

    connect(menuComboBox, &QComboBox::currentTextChanged, this, &OrderFoodDialog::updatePrice);
    connect(confirmButton, &QPushButton::clicked, this, [](){
        //play sound
    });
    connect(confirmButton, &QPushButton::clicked, this, &QDialog::accept);
}

QString OrderFoodDialog::getSelectedFood() {
    return menuComboBox->currentText();
}

int OrderFoodDialog::getQuantity() {
    return quantitySpinBox->value();
}

int OrderFoodDialog::getExtraPrice() {
    return extraPriceLineEdit->text().toInt();
}

void OrderFoodDialog::loadMenu(const json &menuData) {
    menuComboBox->clear();
    menuPrices.clear();

    for (const auto &menu : menuData) {
        QString name = QString::fromStdString(menu[0]);
        int price = menu[1];
        menuPrices[name] = price;
        menuComboBox->addItem(name);
    }
}

void OrderFoodDialog::updatePrice() {
    QString selectedFood = menuComboBox->currentText();
    int price = menuPrices[selectedFood];
    priceLabel->setText(QString("Price: %1 Baht").arg(price));
}
