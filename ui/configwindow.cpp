#include "configwindow.h"
#include "header/json.h"
#include "ui_configwindow.h"

QSettings *settings = nullptr;

QString currentTheme;

configwindow::configwindow(QWidget *parent,QMap<QString,QPalette> Map)
    : QDialog(parent)
    , ui(new Ui::configwindow)
    , networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    //Initial Settings
    currentVersion = settings->value("version").toString();
    currentTheme = settings->value("theme").toString();

    //Set Initital Version
    ui->version->setText("Version: " + currentVersion);
    ui->updateBtn->hide();

    // Connect network reply signal
    connect(networkManager, &QNetworkAccessManager::finished, this, &configwindow::onVersionCheckFinished);

    QUrl apiUrl("https://api.github.com/repos/sriv95/My-Restaurant-Manager/releases/latest"); //Repo URL
    QNetworkRequest request(apiUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader, "QtApp"); // GitHub requires a User-Agent
    networkManager->get(request);
    
    PalettesMap = Map;
    for (const QString &key : PalettesMap.keys()) {
        ui->ThemeSelect->addItem(key);
    }
    ui->ThemeSelect->setCurrentText(settings->value("theme").toString());
}

void configwindow::onVersionCheckFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        json jsonResponse = json::parse(reply->readAll());
        QString latestVersion = QString::fromStdString(jsonResponse["tag_name"]);
        if (!latestVersion.isEmpty() && currentVersion < latestVersion) {
            ui->version->setText("Version: " + currentVersion + " (Outdated! " + latestVersion + " is released)");
            ui->updateBtn->show();
        }
        else if(currentVersion == latestVersion) ui->version->setText("Version: " + currentVersion + " (Up To Date)");
        else if(currentVersion > latestVersion) ui->version->setText("Version: " + currentVersion + " (Beta/Unreleased)");
    }
    reply->deleteLater();
}

configwindow::~configwindow()
{
    delete ui;
}

void configwindow::on_ThemeSelect_currentTextChanged(const QString &arg1)
{
    QApplication::setPalette(PalettesMap[arg1]);
    currentTheme=arg1;
}

void configwindow::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button->text()=="Save") {
        settings->setValue("theme",currentTheme);
        settings->setValue("font",QApplication::font());
        emit returnValue(ui->Title->text());
    }
    this->close();
}

void configwindow::on_updateBtn_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/sriv95/My-Restaurant-Manager/releases"));
}


void configwindow::on_fontComboBox_currentFontChanged(const QFont &f)
{
    QApplication::setFont(f);
}


void configwindow::on_Reset_clicked()
{
    ui->Title->setText("My Restaurant Manager");
    ui->ThemeSelect->setCurrentText("Dark");
    ui->fontComboBox->setCurrentText("Segoe UI");
}


void configwindow::on_comboBox_currentIndexChanged(int index)
{
    settings->setValue("gui_scale",index);
}

