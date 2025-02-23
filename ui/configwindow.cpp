#include "configwindow.h"
#include "header/json.h"
#include "ui_configwindow.h"

configwindow::configwindow(QWidget *parent,QMap<QString,QPalette> PalettesMap)
    : QDialog(parent)
    , ui(new Ui::configwindow)
    , networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    //Set Initital Version
    ui->version->setText("Version: " + currentVersion);

    // Connect network reply signal
    connect(networkManager, &QNetworkAccessManager::finished, this, &configwindow::onVersionCheckFinished);

    QUrl apiUrl("https://api.github.com/repos/sriv95/My-Restaurant-Manager/releases/latest"); //Repo URL
    QNetworkRequest request(apiUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader, "QtApp"); // GitHub requires a User-Agent
    networkManager->get(request);
    
}

void configwindow::onVersionCheckFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        json jsonResponse = json::parse(reply->readAll());
        QString latestVersion = QString::fromStdString(jsonResponse["tag_name"]);
        if (!latestVersion.isEmpty() && currentVersion < latestVersion) ui->version->setText("Version: " + currentVersion + " (Outdated! " + latestVersion + "was released)");
        else if(currentVersion == latestVersion) ui->version->setText("Version: " + currentVersion + " (Up To Date)");
        else if(currentVersion > latestVersion) ui->version->setText("Version: " + currentVersion + " (Beta/Unreleased)");
    }
    reply->deleteLater();
}

configwindow::~configwindow()
{
    delete ui;
}
