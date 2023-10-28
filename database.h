#ifndef DATABASE_H
#define DATABASE_H

#include <QtDebug>

#include <QObject>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QTableView>

#define POSTGRE_DRIVER "QPSQL"
#define DB_NAME "demoDB"

class database : public QObject
{
    Q_OBJECT
public:
    enum class direction {
        arrival,
        departure
    };

    explicit database(QObject *parent = nullptr);
    ~database();

    void connectToDB();
    void disconnectFromDB();
    QSqlError lastError();
    void bindView(QTableView* view_);
    void airportsRequest();
    void dateRangeRequests();
    void aircraftsRequest(QString date, QString airport, direction dir);

private:
    QSqlDatabase* DB;
    QSqlQueryModel* qModel;
    QTableView* view;

signals:
    void sig_sendStatusConnection(bool);
    void sig_sendQueryError(QSqlError err);
    void sig_sendAirportsList(QMap<QString, QString> _airports);
    void sig_sendDayRange(QDateTime min, QDateTime max);

};

#endif // DATABASE_H
