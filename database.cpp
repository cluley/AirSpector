#include "database.h"

database::database(QObject *parent)
    : QObject{parent}
{
    DB = new QSqlDatabase();
    *DB = QSqlDatabase::addDatabase(POSTGRE_DRIVER, DB_NAME);

    qModel = new QSqlQueryModel(this);
    qStatModel = new QSqlQueryModel(this);

}

database::~database()
{
    delete DB;
}

void database::connectToDB()
{
    DB->setHostName("981757-ca08998.tmweb.ru");
    DB->setDatabaseName("demo");
    DB->setUserName("netology_usr_cpp");
    DB->setPassword("CppNeto3");
    DB->setPort(5432);

    emit sig_sendStatusConnection(DB->open( ));
}

void database::disconnectFromDB()
{
    DB->close();
}

QSqlError database::lastError()
{
    return DB->lastError();
}

void database::bindView(QTableView *view_)
{
    view = view_;
}

void database::airportsRequest()
{
    qModel->setQuery("SELECT airport_name->>'ru' as \"airportName\", airport_code as \"airportCode\" FROM bookings.airports_data;", *DB);

    QMap<QString, QString> _airports;

    for(uint8_t i = 0; i < qModel->rowCount(); ++i){

        QString name = qModel->record(i).value("airportName").toString();
        QString code = qModel->record(i).value("airportCode").toString();
        _airports.insert(name, code);
    }

    if (qModel->lastError().isValid()){

        QSqlError err = qModel->lastError();
        emit sig_sendQueryError(err);
    }
    else{
        emit sig_sendAirportsList(_airports);
    }
}

void database::dateRangeRequests()
{
    qModel->setQuery("SELECT date_trunc('day', MIN(scheduled_departure)) as \"firstDay\", date_trunc('day', MAX(scheduled_departure)) as \"lastDay\" FROM bookings.flights;", *DB);

    QDate firstDay = qModel->record().value("firstDay").toDate();
    QDate lastDay = qModel->record().value("lastDay").toDate();

    qDebug() << firstDay;
    qDebug() << lastDay;

    if (qModel->lastError().isValid()){

        QSqlError err = qModel->lastError();
        emit sig_sendQueryError(err);
    }

    emit sig_sendDayRange(firstDay, lastDay);
}

void database::aircraftsRequest(const QString date, const QString airport, const direction dir)
{
    if(dir == direction::arrival)
    {
        qModel->setQuery("SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' as \"Name\" "
                         "FROM bookings.flights f "
                         "JOIN bookings.airports_data ad ON ad.airport_code = f.departure_airport "
                         "WHERE f.arrival_airport = " + airport +
                         " AND DATE(f.scheduled_departure) = '" + date + "'" +
                         " ORDER BY f.scheduled_arrival ASC;", *DB);

        qModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Аэропорт назначения"));
    }
    else if(dir == direction::departure)
    {
        qModel->setQuery("SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' as \"Name\" "
                     "FROM bookings.flights f "
                     "JOIN bookings.airports_data ad ON ad.airport_code = f.arrival_airport "
                     "WHERE f.departure_airport = " + airport  +
                     "AND DATE(f.scheduled_departure) = '" + date + "'" +
                     "ORDER BY f.scheduled_departure ASC;", *DB);

        qModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Аэропорт отправления"));
    }

    qModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Номер рейса"));
    qModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Время вылета"));

    if (qModel->lastError().isValid()){

        QSqlError err = qModel->lastError();
        emit sig_sendQueryError(err);
    }
    else
    {
        view->setModel(qModel);
        view->resizeColumnsToContents();
        view->resizeRowsToContents();
        view->show();
    }

}

void database::yearStatRequest(const QString airport)
{
    qStatModel->setQuery("SELECT count(flight_no), date_trunc('month', scheduled_departure) as \"Month\" from bookings.flights f "
                     "WHERE (scheduled_departure::date > date('2016-08-31') and scheduled_departure::date <= date('2017-08-31')) "
                     "AND ( departure_airport = " + airport + " or arrival_airport = " + airport + ") "
                     "GROUP BY \"Month\";", *DB);

    QVector<int> yearStatistics(12);

    for(uint8_t i = 0; i < yearStatistics.size(); ++i)
    {
        int amount = qStatModel->record(i).value("count").toInt();
        int month = (qStatModel->record(i).value("Month").toDate().toString("M").toInt()) - 1;

        yearStatistics[month] = amount;
    }

    if (qStatModel->lastError().isValid()){

        QSqlError err = qStatModel->lastError();
        emit sig_sendQueryError(err);
    }
    else
    {
        emit sig_sendYearStatistics(yearStatistics);
    }
}

void database::monthStatRequest(const QString airport)
{
    qStatModel->setQuery("SELECT count(flight_no), date_trunc('day', scheduled_departure) as \"Day\" from bookings.flights f "
                     "WHERE(scheduled_departure::date > date('2016-08-31') AND scheduled_departure::date <= date('2017-08-31')) "
                     "AND (departure_airport = " + airport + " or arrival_airport = " + airport + ") "
                     "GROUP BY \"Day\";", *DB);

    QVector<QVector<int>> monthStatistics(12);
    monthStatistics[0].resize(31);
    monthStatistics[1].resize(28);
    monthStatistics[2].resize(31);
    monthStatistics[3].resize(30);
    monthStatistics[4].resize(31);
    monthStatistics[5].resize(30);
    monthStatistics[6].resize(31);
    monthStatistics[7].resize(31);
    monthStatistics[8].resize(30);
    monthStatistics[9].resize(31);
    monthStatistics[10].resize(30);
    monthStatistics[11].resize(31);

    uint16_t cntr = 0;
    for(uint8_t i = 0, month = 8; i < 12; ++i, ++month)
    {
        if(month == 12){
            month = 0;
        }

        for(uint8_t day = 0; day < monthStatistics[month].size(); ++day, ++cntr)
        {
            monthStatistics[month][day] = qStatModel->record(cntr).value("count").toInt();
        }
    }

    if (qStatModel->lastError().isValid()){

        QSqlError err = qStatModel->lastError();
        emit sig_sendQueryError(err);
    }
    else
    {
        emit sig_sendMonthStatistics(monthStatistics);
    }
}
