#include "congestion.h"
#include "ui_congestion.h"

congestion::congestion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::congestion)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags()|Qt::WindowMaximizeButtonHint);

    months.resize(12);
    months[0] = "Январь";   months[1] = "Февраль";  months[2] = "Март";     months[3] = "Апрель";
    months[4] = "Май";      months[5] = "Июнь";     months[6] = "Июль";     months[7] = "Август";
    months[8] = "Сентябрь"; months[9] = "Октябрь";  months[10] = "Ноябрь";  months[11] = "Декабрь";

    graph = new QCPGraph(ui->monthChart->xAxis, ui->monthChart->yAxis);
    bars = new QCPBars(ui->yearChart->xAxis, ui->yearChart->yAxis);

    setDarkStyle(ui->yearChart);
    setDarkStyle(ui->monthChart);

    prepareYearChart();

    for(auto& month : months){
        ui->comBox_months->addItem(month);
    }

    connect(ui->comBox_months, &QComboBox::currentIndexChanged, this, &congestion::slot_displayMonthChart);
}

congestion::~congestion()
{
    delete ui;

    delete graph;
    delete bars;
}

void congestion::setDarkStyle(QCustomPlot *chart)
{
    chart->xAxis->setBasePen(QPen(Qt::white, 1));
    chart->yAxis->setBasePen(QPen(Qt::white, 1));
    chart->xAxis->setTickPen(QPen(Qt::white, 1));
    chart->yAxis->setTickPen(QPen(Qt::white, 1));
    chart->xAxis->setSubTickPen(QPen(Qt::white, 1));
    chart->yAxis->setSubTickPen(QPen(Qt::white, 1));
    chart->xAxis->setTickLabelColor(Qt::white);
    chart->yAxis->setTickLabelColor(Qt::white);
    chart->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    chart->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    chart->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    chart->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    chart->xAxis->grid()->setSubGridVisible(true);
    chart->yAxis->grid()->setSubGridVisible(true);
    chart->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    chart->yAxis->grid()->setZeroLinePen(Qt::NoPen);

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    chart->setBackground(plotGradient);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    chart->axisRect()->setBackground(axisRectGradient);
}

void congestion::prepareYearChart()
{
    ui->yearChart->xAxis->setRange(0, 13);
    QVector<double> ticks;
    QVector<QString> labels;
    ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    for(uint8_t i = 0, j = 8; i < 12; ++i, ++j){
        if(j == 12){
            j = 0;
        }

        labels << months[j];
    }
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    ui->yearChart->xAxis->setTicker(textTicker);
    ui->yearChart->xAxis->setSubTicks(false);
    ui->yearChart->xAxis->setTickLength(0, 4);
    ui->yearChart->xAxis->setBasePen(QPen(Qt::white));
    ui->yearChart->xAxis->setTickPen(QPen(Qt::white));
    ui->yearChart->xAxis->grid()->setVisible(true);
    ui->yearChart->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    ui->yearChart->xAxis->setTickLabelColor(Qt::white);
    ui->yearChart->xAxis->setLabelColor(Qt::white);

    ui->yearChart->axisRect()->addAxis(QCPAxis::atBottom)->setTickLabelColor(QColor(255, 255, 255));
    ui->yearChart->axisRect()->axis(QCPAxis::atBottom, 1)->setRange(0, 13);
    QVector<double> buttomTicks;
    QVector<QString> buttomLabels;
    buttomTicks << 0 << 5;
    buttomLabels << "2016г." << "2017г.";
    QSharedPointer<QCPAxisTickerText> buttomTicker(new QCPAxisTickerText);
    buttomTicker->addTicks(buttomTicks, buttomLabels);
    ui->yearChart->axisRect()->axis(QCPAxis::atBottom, 1)->setTicker(buttomTicker);
    ui->yearChart->axisRect()->axis(QCPAxis::atBottom, 1)->setSubTicks(false);
    ui->yearChart->axisRect()->axis(QCPAxis::atBottom, 1)->setTickLength(0, 4);
    ui->yearChart->axisRect()->axis(QCPAxis::atBottom, 1)->setBasePen(QPen(Qt::white));
    ui->yearChart->axisRect()->axis(QCPAxis::atBottom, 1)->setBasePen(QPen(Qt::white, 1));
    ui->yearChart->axisRect()->axis(QCPAxis::atBottom, 1)->setTickPen(QPen(Qt::white, 1));
    ui->yearChart->axisRect()->axis(QCPAxis::atBottom, 1)->setSubTickPen(QPen(Qt::white, 1));
    ui->yearChart->axisRect()->axis(QCPAxis::atBottom, 1)->setSubTicks(false);
    ui->yearChart->axisRect()->axis(QCPAxis::atBottom, 1)->setUpperEnding(QCPLineEnding::esSpikeArrow);

    ui->yearChart->rescaleAxes();
}

int congestion::ceiling(QVector<int> arr)
{
    int max_num = 0;
    for(auto num : arr)
    {
        if(num > max_num) max_num = num;
    }

    max_num += max_num / 10;

    return max_num;
}

void congestion::slot_displayYearChart()
{
    QVector<double> x(12), y(12);
    for(uint8_t i = 0, j = 8; i < 12; ++i, ++j){
        if(j == 12){
            j = 0;
        }

        x[i] = i+1;
        y[i] = yearStatistics[j];
    }

    bars->setName("Cтатистика за год");
    bars->setWidth(0.4);
    bars->setData(x, y);
    bars->setPen(QPen(QColor(10, 140, 70).lighter(160)));
    bars->setBrush(QColor(10, 100, 50, 160));

    ui->yearChart->yAxis->setRange(0, ceiling(yearStatistics));
    ui->yearChart->replot();
}

void congestion::slot_displayMonthChart()
{
    uint8_t monthIdx = ui->comBox_months->currentIndex();

    if(monthIdx >= 8){
        ui->lbl_yearHint->setText("2016 г.");
    }else{
        ui->lbl_yearHint->setText("2017 г.");
    }

    QVector<double> x(monthStatistics[monthIdx].size()), y(monthStatistics[monthIdx].size());

    for(uint8_t i = 0; i < monthStatistics[monthIdx].size(); ++i)
    {
        x[i] = i + 1;
        y[i] = monthStatistics[monthIdx][i];
    }

    graph->setData(x, y);
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
    graph->setPen(QPen(QColor(120, 120, 120), 2));

    ui->monthChart->rescaleAxes();
    ui->monthChart->xAxis->setRange(0, (monthStatistics[monthIdx].size() + 1));
    ui->monthChart->yAxis->setRange(0, ceiling(monthStatistics[monthIdx]));
    ui->monthChart->replot();
}

void congestion::slot_setYearStatistics(QVector<int> statisticData)
{
    yearStatistics = std::move(statisticData);

    slot_displayYearChart();
}

void congestion::slot_setMonthStatistics(QVector<QVector<int> > statisticData)
{
    monthStatistics = std::move(statisticData);

    slot_displayMonthChart();
}
