#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setDarkStyle();

    ui->setupUi(this);
    ui->statusBar->showMessage("Подключение...");

    ui->comBox_headings->addItem("Прилет");
    ui->comBox_headings->addItem("Вылет");

    ui->pushB_ok->setEnabled(false);
    ui->pushB_ok->setToolTip("Отобразить результат по выбранным параметрам");
    ui->pushB_ok->setToolTipDuration(5000);

    ui->pushB_stat->setEnabled(false);
    ui->pushB_stat->setToolTip("Открыть статистику по выбранному аэропорту");
    ui->pushB_stat->setToolTipDuration(5000);

    msgBox = new QMessageBox(this);
    statistics = new congestion(this);
    demoDB = new database(this);

    connectToDB();
    demoDB->bindView(ui->tableView);

    connect(demoDB, &database::sig_sendQueryError, this, &MainWindow::slot_queryErrorWorker);

    connect(demoDB, &database::sig_sendStatusConnection, this, &MainWindow::connStatusProcessing);

    connect(demoDB, &database::sig_sendAirportsList, this, &MainWindow::slot_setAirportsList);

    connect(demoDB, &database::sig_sendDayRange, this, &MainWindow::slot_setDateTimeRange);

    connect(demoDB, &database::sig_sendYearStatistics, statistics, &congestion::slot_setYearStatistics);

    connect(demoDB, &database::sig_sendMonthStatistics, statistics, &congestion::slot_setMonthStatistics);
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

void MainWindow::setDarkStyle()
{
    QPalette darkPalette;

    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(10, 140, 70));
    darkPalette.setColor(QPalette::Highlight, QColor(10, 140, 70));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    QFont font;

    font.setFamily("Agency FB");
    font.setBold(true);

    qApp->setPalette(darkPalette);
    qApp->setFont(font);
}

void MainWindow::slot_queryErrorWorker(const QSqlError err)
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

    ui->pushB_ok->setEnabled(true);
    ui->pushB_stat->setEnabled(true);
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

    auto requestYear = [this, airport]{ demoDB->yearStatRequest(airport); };
    auto requestMonths = [this, airport] { demoDB->monthStatRequest(airport); };
    QtConcurrent::run(requestYear).then(requestMonths);

    statistics->setWindowModality(Qt::ApplicationModal);
    statistics->show();
}

