#ifndef ORDERFOODDIALOG_H
#define ORDERFOODDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMap>
#include "header/json.h"

class OrderFoodDialog
    : public QDialog
{
    Q_OBJECT

public:
    explicit OrderFoodDialog(QWidget *parent = nullptr);
    QString getSelectedFood();
    int getQuantity();
    int getExtraPrice();

    void loadMenu(const json &menuData);

private slots:
    void updatePrice();

private:
    QComboBox *menuComboBox;
    QLabel *priceLabel;
    QSpinBox *quantitySpinBox;
    QLineEdit *extraPriceLineEdit;
    QPushButton *confirmButton;

    QMap<QString, int> menuPrices;
};

#endif
