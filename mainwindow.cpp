#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->showMessage("Подключение...");
    ui->pushB_ok->setToolTip("Отобразить результат по выбранным параметрам");
    ui->pushB_ok->setToolTipDuration(5000);

    msgBox = new QMessageBox(this);

    demoDB = new database(this);
    connectToDB();
    demoDB->bindView(ui->tableView);

    connect(demoDB, &database::sig_sendQueryError, this, &MainWindow::queryErrorWorker);

    connect(demoDB, &database::sig_sendStatusConnection, this, &MainWindow::connStatusProcessing);

    connect(demoDB, &database::sig_sendAirportsList, this, &MainWindow::slot_setAirportsList);

    connect(demoDB, &database::sig_sendDayRange, this, &MainWindow::slot_setDateTimeRange);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectToDB(unsigned long secs)
{
    auto connection = [this, secs]{
                                QThread::sleep(secs);
                                demoDB->connectToDB();
                            };
    QtConcurrent::run(connection);
}

void MainWindow::connStatusProcessing(bool status)
{
    if(status == active){
        ui->statusBar->setStyleSheet("color:green");
        ui->statusBar->showMessage("Активно");

        minimumDataParser();
    }
    else if(status == disabled){
        ui->statusBar->setStyleSheet("color:red");
        ui->statusBar->showMessage("Отключено");

        msgBox->setIcon(QMessageBox::Critical);
        msgBox->setText(demoDB->lastError().text());
        msgBox->exec();

        demoDB->disconnectFromDB();
        connectToDB(5);
    }
}

void MainWindow::minimumDataParser()
{
    auto airportsRequest = [this]{ demoDB->airportsRequest();};

//    auto dateRangeRequests = [this]{ demoDB->dateRangeRequests();};

    QtConcurrent::run(airportsRequest);

    QDate min;
    min.setDate(2016, 8, 15);
    QDate max;
    max.setDate(2017, 9, 15);
    ui->datEd_departureDay->setDateRange(min, max);

    ui->comBox_headings->addItem("Прилет");
    ui->comBox_headings->addItem("Вылет");
}

void MainWindow::queryErrorWorker(QSqlError err)
{
    msgBox->setText(err.text());
    msgBox->exec();
}

void MainWindow::slot_setAirportsList(QMap<QString, QString> _airports)
{
    airports = _airports;

    for(auto it = airports.cbegin(); it != airports.cend(); ++it){
        ui->comBox_airports->addItem(it.key());
    }
}

void MainWindow::slot_setDateTimeRange(QDateTime min, QDateTime max)
{
    ui->datEd_departureDay->setDateTimeRange(min, max);
}


void MainWindow::on_pushB_ok_clicked()
{
    QString date = ui->datEd_departureDay->date().toString("yyyy-MM-dd");

    QString airport = "'" + airports[ui->comBox_airports->currentText()] + "'";

    database::direction dir = static_cast<database::direction>(ui->comBox_headings->currentIndex());

    demoDB->aircraftsRequest(date, airport, dir);
}

