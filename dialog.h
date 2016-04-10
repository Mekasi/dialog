#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QByteArray>
#include "qcustomplot.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void readData();
    void realtimeDataSlot();

    void PulseGraph();
    void PulseBPM();

    void updateDataFromArduino(QStringList rx_data_parse);
    void updateTemperature(QString);
    void setupPlayground(QCustomPlot *customPlot);
    void ledTemp();

private:
    Ui::Dialog *ui;

    QSerialPort *arduino;
    QByteArray serialData;
    QString serialBuffer;
    QString parsed_data;
    double pValue;

    QTimer dataTimer;
};

#endif // DIALOG_H
