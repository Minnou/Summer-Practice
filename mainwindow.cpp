#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cstdlib>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QJsonObject>
#include <QTimer>

//название жисона из которого читаем
QString FILE_NAME = "test.json";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableDevicesWidget->setColumnCount(3);
    QStringList headers;
    headers.append("Название");
    headers.append("Серийный номер");
    headers.append("uuid");
    ui->tableDevicesWidget->setHorizontalHeaderLabels(headers);
    ui->tableDevicesWidget->horizontalHeader()->setStretchLastSection(true);
    MainWindow::ReadJson();
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));
    interval = 1000;
    timer->start(interval);
}


MainWindow::~MainWindow()
{
    QString command = "rm /tmp/" + FILE_NAME;
    system(command.toLocal8Bit());
    delete ui;
}

//Как выглядит json
//{
//   "blockdevices": [
//      {"name": "sda", "serial": null, "uuid": null,
//         "children": [
//            {"name": "sda1", "serial": null, "uuid": "f583e202-a8af-468f-8ba0-a247969119dc"},
//            {"name": "sda2", "serial": null, "uuid": null},
//            {"name": "sda5", "serial": null, "uuid": "38cd2fff-b608-4291-babc-8ac0c48cc9e1"},
//            {"name": "sda6", "serial": null, "uuid": "9928712e-afcc-47ff-933f-b70a8ba2c0af"}
//         ]
//      },
//      {"name": "sdb", "serial": "60A44C4139D4BFB0E99500C4", "uuid": null,
//         "children": [
//            {"name": "sdb1", "serial": null, "uuid": "3EC4-4836"}
//         ]
//      },
//      {"name": "sr0", "serial": "10000000000000000001", "uuid": "2022-07-23-07-54-49-00"}
//   ]
//}

QString createReadFile(){
    QString command = "lsblk -o name,serial,uuid --json > /tmp/" + FILE_NAME;
    system(command.toLocal8Bit());
    QString val;
    QFile file("/tmp/" + FILE_NAME);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();
    return val;
}

void MainWindow::ReadJson(){
    QString val = createReadFile();
    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jObject = document.object();
    QJsonArray jArr = jObject["blockdevices"].toArray();
    int i = 0;
    //очистка таблицы
    ui->tableDevicesWidget->clearContents();
    ui->tableDevicesWidget->setRowCount(0);
    //заполнение таблицы
    foreach (const QJsonValue & value, jArr) {
        QJsonObject obj = value.toObject();
        ui->tableDevicesWidget->insertRow(i);
        ui->tableDevicesWidget->setItem(i, 0, new QTableWidgetItem(obj["name"].toString()));
        ui->tableDevicesWidget->setItem(i, 1, new QTableWidgetItem(obj["serial"].toString()));
        ui->tableDevicesWidget->setItem(i, 2, new QTableWidgetItem(obj["uuid"].toString()));
        QJsonArray childrenArr = obj["children"].toArray();
        //достаём ДЕТЕЙ из основного объекта
        foreach (const QJsonValue & childValue, childrenArr) {
            i++;
            QJsonObject obj = childValue.toObject();
            ui->tableDevicesWidget->insertRow(i);
            ui->tableDevicesWidget->setItem(i, 0, new QTableWidgetItem(obj["name"].toString()));
            ui->tableDevicesWidget->setItem(i, 1, new QTableWidgetItem(obj["serial"].toString()));
            ui->tableDevicesWidget->setItem(i, 2, new QTableWidgetItem(obj["uuid"].toString()));
        }
        i++;
    }
}
void MainWindow::slotTimerAlarm()
{
    MainWindow::ReadJson();
}


void MainWindow::on_intervalPushButton_clicked()
{
    QString s = ui->intervalLineEdit->text();
    int newInterval = s.toInt();
    if(newInterval > 0){
        interval = newInterval;
        timer->stop();
        timer->start(interval);
    }
}
