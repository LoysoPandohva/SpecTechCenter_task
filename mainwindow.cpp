#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QList"
#include "QComboBox"
#include "QString"
#include "QTextEdit"
#include "QTime"
#include "QMessageBox"
#include "QtSerialPort/QSerialPortInfo"
#include "package.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);
    this->setWindowTitle("SpecTechCenter task");

    QList<QSerialPortInfo> portsList;
    portsList = QSerialPortInfo::availablePorts();

    for (int i = 0; i < portsList.length(); i++) {
        ui->selectorCOM->addItem(portsList[i].portName());
    }
}

MainWindow::~MainWindow() {
    port->close();
    delete port;
    delete ui;
}

void MainWindow::on_connectButton_clicked() {
    port = new QSerialPort(this);
    port->setPortName(ui->selectorCOM->currentText());
    port->setBaudRate(QSerialPort::Baud115200);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);

    if(port->open(QIODevice::ReadWrite)){
        connect(port, SIGNAL(readyRead()), this, SLOT(serialReceived()));
        ui->statusbar->showMessage("Port is connected");
    }
    else{
        QMessageBox::critical(0,"ERROR", "Failed connect to the port");
    }
}

void MainWindow::on_sendButton_clicked() {
    if(port->isOpen()){
        QString time = QTime::currentTime().toString();
        QString message = ui->numToSend->toPlainText();
        Package package(message);
        QByteArray serializeMes = package.serializeMessage();

        if(serializeMes.size() == 17 && serializeMes[0] == (char)0x40){ // 1+4+4+4+4 = 17 bytes in package
            port->write(serializeMes);
            ui->story->append(time + "\n" + "entered numbers: \n" + message + "\n");
        }
        else{
            ui->story->append(time + "\n" + "entered numbers: \n" + message + "\n"
                               "[ERROR] entered data is incorrect \n");
        }
        ui->story->append("-------\n");

        ui->numToSend->clear();
        ui->statusbar->showMessage("Package send");
    }
    else{
        QMessageBox::critical(0,"ERROR", "First connect to the port");
    }
}

void MainWindow::serialReceived() {
    QByteArray serializeMes = port->readAll();
    Package package;
    QString message = package.deSerializeMessage(serializeMes);

    QString time = QTime::currentTime().toString();

    if(message.size() > 0){
        ui->story->append(time + "\n" + "received numbers: \n" + message + "\n");
    }
    else{
        ui->story->append(time + "\n" + "[ERROR] received data is incorrect \n");
    }
    ui->story->append("-------\n");
    ui->statusbar->showMessage("Package received");
}
