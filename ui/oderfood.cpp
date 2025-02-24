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

}

void OrderFoodDialog::playButtonSound()
{
    if (buttonSound->playbackState() == QMediaPlayer::PlayingState) {
        buttonSound->setPosition(0);
    } else {
        buttonSound->play();
    }
}

void OrderFoodDialog::playSelectSound()
{
    if (SelectSound->playbackState() == QMediaPlayer::PlayingState) {
        SelectSound->setPosition(0);
    } else {
        SelectSound->play();
    }
}

void OrderFoodDialog::playOpenMenuSound()
{
    if (OpenMenuSound->playbackState() == QMediaPlayer::PlayingState) {
        OpenMenuSound->setPosition(0);
    } else {
        OpenMenuSound->play();
    }
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
