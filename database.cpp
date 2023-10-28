#include "database.h"

database::database(QObject *parent)
    : QObject{parent}
{
    DB = new QSqlDatabase();
    *DB = QSqlDatabase::addDatabase(POSTGRE_DRIVER, DB_NAME);

    qModel = new QSqlQueryModel(this);

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
    qModel->setQuery("SELECT airport_name->>'ru' as \"airportName\", airport_code as \"airportCode\" FROM bookings.airports_data", *DB);

    QMap<QString, QString> _airports;

    for(uint8_t i = 0; i < 104; ++i){

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
    qModel->setQuery("SELECT DATE(MIN(scheduled_departure)) as \"firstDay\", DATE(MAX(scheduled_departure)) as \"lastDay\" FROM flights", *DB);

    qDebug() << qModel->record().value("firstDay").toString();
    qDebug() << qModel->record().value("lastDay").toString();

    if (qModel->lastError().isValid()){

        QSqlError err = qModel->lastError();
        emit sig_sendQueryError(err);
    }

    //    emit sig_sendDayRange(firstDay, lastDay);
}

void database::aircraftsRequest(QString date, QString airport, direction dir)
{
    if(dir == direction::arrival)
    {
        qModel->setQuery("SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' as \"Name\" "
                         "FROM bookings.flights f "
                         "JOIN bookings.airports_data ad ON ad.airport_code = f.departure_airport "
                         "WHERE f.arrival_airport = " + airport +
                         " AND DATE(f.scheduled_departure) = '" + date + "'" +
                         " ORDER BY f.scheduled_arrival ASC;", *DB);
    }
    else if(dir == direction::departure)
    {
        qModel->setQuery("SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' as \"Name\" "
                     "FROM bookings.flights f "
                     "JOIN bookings.airports_data ad ON ad.airport_code = f.arrival_airport "
                     "WHERE f.departure_airport = " + airport  +
                     "AND DATE(f.scheduled_departure) = '" + date + "'" +
                     "ORDER BY f.scheduled_departure ASC;", *DB);
    }

    qModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Номер рейса"));
    qModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Запланированное прибытие"));
    qModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Аэропорт"));

    if (qModel->lastError().isValid()){

        QSqlError err = qModel->lastError();
        emit sig_sendQueryError(err);
    }
    else
    {
        view->setModel(qModel);
        view->show();
    }

}
