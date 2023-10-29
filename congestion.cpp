#include "congestion.h"
#include "ui_congestion.h"

congestion::congestion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::congestion)
{
    ui->setupUi(this);

    graph = new QCPGraph(ui->chart->xAxis, ui->chart->yAxis);
    bars = new QCPBars(ui->chart->xAxis, ui->chart->yAxis);
}

congestion::~congestion()
{
    delete ui;
    delete graph;
}

void congestion::slot_displayChart(QMap<int, int> statisticData)
{
    bars->setName("Cтатистика за год");

    QVector<double> key;
    QVector<double> value;

    for(uint8_t i = 0, j = 8; i < 12; ++i, ++j){
        key << i+1;
        value << statisticData[j];

        if(j == 11){
            j = 0;
        }
    }

    bars->setData(key, value);
    ui->chart->rescaleAxes();
    ui->chart->replot();

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

//    ui->chart->addGraph();

//    QVector<double> x(12);
//    QVector<double> y(12);

//    auto max = [](QMap<int, int> arr){
//        int max_num = 0;
//        for(int i = 0; i < arr.size(); ++i)
//        {
//            if(arr[i] > max_num) max_num = arr[i];
//        }

//        return max_num;
//    };

//    for(uint8_t i = 0, j = 8; i < 12; ++i, ++j){
//        x[i] = i+1;
//        y[i] = statisticData[j];

//        if(j == 11){
//            j = 0;
//        }

//        graph->addData(x[i], y[i]);
//    }

//    ui->chart->xAxis->setRange(0, 12);
//    ui->chart->yAxis->setRange(0, max(statisticData));
//    ui->chart->rescaleAxes();
//    ui->chart->replot();
}
