#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->showMessage("Подключение...");

    ui->comBox_headings->addItem("Прилет");
    ui->comBox_headings->addItem("Вылет");

    ui->pushB_ok->setToolTip("Отобразить результат по выбранным параметрам");
    ui->pushB_ok->setToolTipDuration(5000);

    ui->pushB_stat->setToolTip("Открыть статистику по выбранному аэропорту");
    ui->pushB_stat->setToolTipDuration(5000);

    msgBox = new QMessageBox(this);

    statistics = new congestion(this);

    demoDB = new database(this);
    connectToDB();
    demoDB->bindView(ui->tableView);

    connect(demoDB, &database::sig_sendQueryError, this, &MainWindow::queryErrorWorker);

    connect(demoDB, &database::sig_sendStatusConnection, this, &MainWindow::connStatusProcessing);

    connect(demoDB, &database::sig_sendAirportsList, this, &MainWindow::slot_setAirportsList);

    connect(demoDB, &database::sig_sendDayRange, this, &MainWindow::slot_setDateTimeRange);

    connect(demoDB, &database::sig_sendYearStatistic, statistics, &congestion::slot_displayChart);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectToDB(const unsigned long secs)
{
    auto connection = [this, secs]{
                                QThread::sleep(secs);
                                demoDB->connectToDB();
                            };
    QtConcurrent::run(connection);
}

void MainWindow::connStatusProcessing(const bool status)
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

//    QtConcurrent::run(airportsRequest).then(dateRangeRequests);

    QtConcurrent::run(airportsRequest);

    QDate min;
    min.setDate(2016, 8, 15);
    QDate max;
    max.setDate(2017, 9, 15);
    ui->datEd_departureDay->setDateRange(min, max);
}

void MainWindow::queryErrorWorker(const QSqlError err)
{
    msgBox->setText(err.text());
    msgBox->exec();
}

void MainWindow::slot_setAirportsList(const QMap<QString, QString> _airports)
{
    airports = _airports;

    for(auto it = airports.cbegin(); it != airports.cend(); ++it){
        ui->comBox_airports->addItem(it.key());
    }
}

void MainWindow::slot_setDateTimeRange(const QDate firstDay, const QDate lastDay)
{
    QDateTime min;
    min.setDate(firstDay);
    QDateTime max;
    max.setDate(lastDay);
    ui->datEd_departureDay->setDateTimeRange(min, max);
}

void MainWindow::on_pushB_ok_clicked()
{
    QString date = ui->datEd_departureDay->date().toString("yyyy-MM-dd");

    QString airport = "'" + airports[ui->comBox_airports->currentText()] + "'";

    database::direction dir = static_cast<database::direction>(ui->comBox_headings->currentIndex());

    auto request = [this, date, airport, dir]{ demoDB->aircraftsRequest(date, airport, dir); };
    QtConcurrent::run(request);
}


void MainWindow::on_pushB_stat_clicked()
{
    QString airport = "'" + airports[ui->comBox_airports->currentText()] + "'";

    auto request = [this, airport]{ demoDB->congestionStatRequest(airport); };
    QtConcurrent::run(request);

    statistics->setWindowModality(Qt::ApplicationModal);
    statistics->show();
}

