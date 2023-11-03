#ifndef CONGESTION_H
#define CONGESTION_H

#include <QDialog>

#include "qcustomplot.h"

namespace Ui {
class congestion;
}

class congestion : public QDialog
{
    Q_OBJECT

public:
    explicit congestion(QWidget *parent = nullptr);
    ~congestion();

private:
    Ui::congestion *ui;
    QCPGraph* graph;
    QCPBars* bars;

    QVector<QString> months;
    QVector<int> yearStatistics;
    QVector<QVector<int>> monthStatistics;

    void setDarkStyle(QCustomPlot* chart);
    void prepareYearChart();
    int ceiling(QVector<int> arr);
public slots:
    void slot_setYearStatistics(QVector<int> statisticData);
    void slot_setMonthStatistics(QVector<QVector<int>> statisticData);
    void slot_displayYearChart();
    void slot_displayMonthChart();
};

#endif // CONGESTION_H
