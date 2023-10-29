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

    QMap<int, int> yearStatistic;

public slots:
    void slot_displayChart(QMap<int, int> statisticData);
};

#endif // CONGESTION_H
