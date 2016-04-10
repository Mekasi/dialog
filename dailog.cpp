#include "dialog.h"
#include "ui_dialog.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QDebug>
#include <QMessageBox>

#include <QTimer>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->temp_lcdNumber->display("---");
    Dialog::PulseGraph();//строим график пульса

    arduino = new QSerialPort(this);
    bool arduino_is_available = false;
    QString arduino_port_name;
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        arduino_is_available = true;
        arduino_port_name = serialPortInfo.systemLocation();
    }

    /*
     *  Open and configure the arduino port if available
     */
    if(arduino_is_available){
        qDebug() << "Found the arduino port...\n";
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadWrite);
        arduino->setBaudRate(QSerialPort::Baud19200);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readData()));
        qDebug() << "Arduino port:"<<arduino_port_name;
 /*       if (arduino->isOpen()&&arduino->isWritable()){
            QByteArray data_tr = "0";
            arduino->write(data_tr);
            arduino->flush();
        }
*/
    }else{
        qDebug() << "Couldn't find the correct port for the arduino.\n";
        QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to arduino.");
    }


}

Dialog::~Dialog()
{
    if(arduino->isOpen()){
        arduino->close(); //    Close the serial port if it's open.
    }
    delete ui;
}

void Dialog::PulseGraph(){
    ui->graph->addGraph(); // blue line
    ui->graph->graph(0)->setPen(QPen(Qt::blue));
    ui->graph->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    ui->graph->graph(0)->setAntialiasedFill(false);
    ui->graph->addGraph(); // red line
    ui->graph->graph(1)->setPen(QPen(Qt::red));
    ui->graph->graph(0)->setChannelFillGraph(ui->graph->graph(1));

    ui->graph->addGraph(); // blue dot
    ui->graph->graph(2)->setPen(QPen(Qt::blue));
    ui->graph->graph(2)->setLineStyle(QCPGraph::lsNone);
    ui->graph->graph(2)->setScatterStyle(QCPScatterStyle::ssDisc);

    ui->graph->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->graph->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->graph->xAxis->setAutoTickStep(false);
    ui->graph->xAxis->setTickStep(2);
    ui->graph->axisRect()->setupFullAxesBox();

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->graph->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->graph->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->graph->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->graph->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

void Dialog::updateTemperature(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->temp_lcdNumber->display(sensor_reading);
}

void Dialog::readData(){

    serialData = arduino->readAll();
    serialBuffer = QString::fromStdString(serialData.toStdString());
    QStringList buffer_split = serialBuffer.split(",");
    qDebug()<<buffer_split[0];
    QStringList buffer_split_parse = buffer_split[0].split(" ");
    qDebug()<<buffer_split_parse[0];
    qDebug()<<buffer_split_parse[1];
    qDebug()<<buffer_split_parse[2];
    qDebug()<<buffer_split_parse[3];

    Dialog::updateDataFromArduino(buffer_split_parse);

}

void Dialog::updateDataFromArduino(QStringList rx_data_parse){
    Dialog::updateTemperature(rx_data_parse[0]); //отправка на дисплей
    double pValue = rx_data_parse[0].toDouble();
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    ui->graph->graph(0)->addData(key,pValue);
    ui->graph->graph(2)->clearData();//точка
    ui->graph->graph(2)->addData(key,pValue);//точка
    ui->graph->replot();
    //  Dialog::PulseBPM(rx_data_parse[0]);
}

void Dialog::PulseBPM(){
    //int thresh = 525;
    //if (rx_data_parse[0] > thresh){

    //    };
}

void Dialog::setupPlayground(QCustomPlot *customPlot)
{
    Q_UNUSED(customPlot)
}

void Dialog::realtimeDataSlot(){
    // calculate two new data points:
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
    double key = 0;
#else
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
#endif

    static double lastPointKey = 0;
    if (key-lastPointKey > 0.01) // at most add point every 10 ms
    {
        // remove data of lines that's outside visible range:
        ui->graph->graph(0)->removeDataBefore(key-8);
        //ui->graph->graph(1)->removeDataBefore(key-8);
        // rescale value (vertical) axis to fit the current data:
        ui->graph->graph(0)->rescaleValueAxis();
        //ui->graph->graph(1)->rescaleValueAxis(true);
        lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->graph->xAxis->setRange(key+0.25, 8, Qt::AlignRight);
    ui->graph->replot();

    /*calculate frames per second:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
        //        ui->statusBar->showMessage(
        //             QString("%1 FPS, Total Data points: %2")
        //              .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
        //             .arg(ui->graph->graph(0)->data()->count()+ui->graph->graph(1)->data()->count())
        //              , 0);
        lastFpsKey = key;
        frameCount = 0;
    }
*/
}
void Dialog::ledTemp(){
    //характеристика вывода цвета в зависимости от индекса
}
