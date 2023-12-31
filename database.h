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
    void aircraftsRequest(const QString date, const QString airport, const direction dir);
    void yearStatRequest(const QString airport);
    void monthStatRequest(const QString airport);

private:
    QSqlDatabase* DB;
    QSqlQueryModel* qModel;
    QSqlQueryModel* qStatModel;
    QTableView* view;

signals:
    void sig_sendStatusConnection(bool);
    void sig_sendQueryError(QSqlError err);
    void sig_sendAirportsList(QMap<QString, QString> _airports);
    void sig_sendDayRange(QDate min, QDate max);
    void sig_sendYearStatistics(QVector<int> yearStatistics);
    void sig_sendMonthStatistics(QVector<QVector<int>> monthStatistics);

};

#endif // DATABASE_H
