#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtConcurrent>

#include "database.h"
#include "congestion.h"

enum connStat{
    disabled,
    active
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void connectToDB(const unsigned long secs = 0);
    void connStatusProcessing(const bool status);
    void minimumDataParser();

private:
    Ui::MainWindow *ui;
    QMessageBox* msgBox;
    database* demoDB;
    congestion* statistics;

    QMap<QString, QString> airports;

signals:

private slots:
    void queryErrorWorker(const QSqlError err);
    void slot_setAirportsList(const QMap<QString, QString> airports);
    void slot_setDateTimeRange(const QDate min, const QDate max);
    void on_pushB_ok_clicked();
    void on_pushB_stat_clicked();
};
#endif // MAINWINDOW_H
