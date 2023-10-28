#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtConcurrent>

#include "database.h"

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

    void connectToDB(unsigned long secs = 0);
    void connStatusProcessing(bool status);
    void minimumDataParser();

private:
    Ui::MainWindow *ui;
    QMessageBox* msgBox;
    database* demoDB;

    QMap<QString, QString> airports;

signals:

private slots:
    void queryErrorWorker(QSqlError err);
    void slot_setAirportsList(QMap<QString, QString> airports);
    void slot_setDateTimeRange(QDateTime min, QDateTime max);
    void on_pushB_ok_clicked();
};
#endif // MAINWINDOW_H
