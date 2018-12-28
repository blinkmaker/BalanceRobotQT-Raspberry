#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initButtons();

    setWindowTitle(tr("BalanceRobot Remote Control"));

    ui->m_textStatus->setStyleSheet("font-size: 12pt; color: #cccccc; background-color: #003333;");

    ui->labelPP->setStyleSheet("font-size: 12pt; color: #ffffff; background-color: #006666;");
    ui->labelPI->setStyleSheet("font-size: 12pt; color: #ffffff; background-color: #006666;");
    ui->labelPD->setStyleSheet("font-size: 12pt; color: #ffffff; background-color: #006666;");
    ui->labelVS->setStyleSheet("font-size: 12pt; color: #ffffff; background-color: #006666;");
    ui->labelAC->setStyleSheet("font-size: 12pt; color: #ffffff; background-color: #006666;");

    ui->m_pBForward->setStyleSheet("font-size: 12pt; color: #ffffff; rgba(255, 255, 255, 0);");
    ui->m_pBBackward->setStyleSheet("font-size: 12pt; color: #ffffff; rgba(255, 255, 255, 0);");
    ui->m_pBLeft->setStyleSheet("font-size: 12pt; color: #ffffff; rgba(255, 255, 255, 0);");
    ui->m_pBRight->setStyleSheet("font-size: 12pt; color: #ffffff; rgba(255, 255, 255, 0);");

    ui->m_pBSearch->setStyleSheet("font-size: 12pt; color: #ffffff; background-color: #336699;");
    ui->m_pBConnect->setStyleSheet("font-size: 12pt; color: #ffffff; background-color: #336699;");
    ui->m_pBExit->setStyleSheet("font-size: 12pt; color: #ffffff; background-color: #cc3300;");

    connect(&m_bleConnection, &BluetoothClient::statusChanged, this, &MainWindow::statusChanged);
    connect(&m_bleConnection, SIGNAL(changedState(BluetoothClient::bluetoothleState)),this,SLOT(changedState(BluetoothClient::bluetoothleState)));

    connect(ui->m_pBSearch, SIGNAL(clicked()),&m_bleConnection, SLOT(startScan()));
    connect(ui->m_pBConnect, SIGNAL(clicked()),this, SLOT(on_ConnectClicked()));

    connect(ui->m_pBForward, SIGNAL(pressed()),this, SLOT(on_ForwardPressed()));
    connect(ui->m_pBForward, SIGNAL(released()),this, SLOT(on_ForwardReleased()));

    connect(ui->m_pBBackward, SIGNAL(pressed()),this, SLOT(on_BackwardPressed()));
    connect(ui->m_pBBackward, SIGNAL(released()),this, SLOT(on_BackwardReleased()));

    connect(ui->m_pBLeft, SIGNAL(pressed()),this, SLOT(on_LeftPressed()));
    connect(ui->m_pBLeft, SIGNAL(released()),this, SLOT(on_LeftReleased()));

    connect(ui->m_pBRight, SIGNAL(pressed()),this, SLOT(on_RightPressed()));
    connect(ui->m_pBRight, SIGNAL(released()),this, SLOT(on_RightReleased()));

    connect(ui->m_pBExit, SIGNAL(clicked()),this, SLOT(on_Exit()));

    QPixmap pixmapf(":/icons/forward.png");
    QIcon ForwardIcon(pixmapf.scaled(128, 64));
    ui->m_pBForward->setIcon(ForwardIcon);
    ui->m_pBForward->setIconSize(pixmapf.scaled(128, 64).rect().size());
    ui->m_pBForward->setFixedSize(pixmapf.scaled(128, 64).rect().size());

    QPixmap pixmapb(":/icons/back.png");
    QIcon BackwardIcon(pixmapb.scaled(128, 64));
    ui->m_pBBackward->setIcon(BackwardIcon);
    ui->m_pBBackward->setIconSize(pixmapb.scaled(128, 64).rect().size());
    ui->m_pBBackward->setFixedSize(pixmapb.scaled(128, 64).rect().size());

    QPixmap pixmapl(":/icons/left.png");
    QIcon LeftIcon(pixmapl.scaled(128, 64));
    ui->m_pBLeft->setIcon(LeftIcon);
    ui->m_pBLeft->setIconSize(pixmapl.scaled(128, 64).rect().size());
    ui->m_pBLeft->setFixedSize(pixmapl.scaled(128, 64).rect().size());

    QPixmap pixmapr(":/icons/right.png");
    QIcon RightIcon(pixmapr.scaled(128, 64));
    ui->m_pBRight->setIcon(RightIcon);
    ui->m_pBRight->setIconSize(pixmapr.scaled(128, 64).rect().size());
    ui->m_pBRight->setFixedSize(pixmapr.scaled(128, 64).rect().size());

    statusChanged("No Device Connected.");
}

void MainWindow::changedState(BluetoothClient::bluetoothleState state){

    switch(state){

    case BluetoothClient::Scanning:
    {
        ui->m_cbDevicesFound->clear();

        ui->m_pBConnect->setEnabled(false);
        ui->m_pBSearch->setEnabled(false);
        ui->m_cbDevicesFound->setEnabled(false);
        statusChanged("Searching for low energy devices...");
        break;
    }
    case BluetoothClient::ScanFinished:
    {
        m_bleConnection.getDeviceList(m_qlFoundDevices);

        if(!m_qlFoundDevices.empty()){

            for (int i = 0; i < m_qlFoundDevices.size(); i++)
            {
                ui->m_cbDevicesFound->addItem(m_qlFoundDevices.at(i));
                qDebug() << ui->m_cbDevicesFound->itemText(i);
            }

            /* Initialise Slot startConnect(int) -> button press m_pBConnect */
            connect(this, SIGNAL(connectToDevice(int)),&m_bleConnection,SLOT(startConnect(int)));

            ui->m_pBConnect->setEnabled(true);
            ui->m_pBSearch->setEnabled(true);
            ui->m_cbDevicesFound->setEnabled(true);

            statusChanged("Please select BLE device");
        }
        else
        {
            statusChanged("No Low Energy devices found");
            ui->m_pBSearch->setEnabled(true);
        }

        break;
    }

    case BluetoothClient::Connecting:
    {
        ui->m_pBConnect->setEnabled(false);
        ui->m_pBSearch->setEnabled(false);
        ui->m_cbDevicesFound->setEnabled(false);
        break;
    }
    case BluetoothClient::Connected:
    {
        ui->m_pBConnect->setText("DisConnect");
        connect(&m_bleConnection, SIGNAL(newData(QByteArray)), this, SLOT(DataHandler(QByteArray)));
        ui->m_pBConnect->setEnabled(true);
        ui->m_pBConnect->setText("DisConnect");

        break;
    }
    case BluetoothClient::DisConnected:
    {
        statusChanged("Device disconnected.");
        ui->m_pBConnect->setEnabled(true);
        ui->m_pBConnect->setText("Connect");
        break;
    }
    case BluetoothClient::ServiceFound:
    {
        break;
    }
    case BluetoothClient::AcquireData:
    {
        requestData(mPP);
        requestData(mPI);
        requestData(mPD);
        requestData(mVS);
        requestData(mAC);

        break;
    }
    case BluetoothClient::Error:
    {
        ui->m_textStatus->clear();
        break;
    }
    default:
        //nothing for now
        break;
    }
}

void MainWindow::DataHandler(QByteArray data)
{
    uint8_t parsedCommand;
    uint8_t rw;
    QByteArray parsedValue;
    parseMessage(&data, parsedCommand, parsedValue, rw);
    bool ok;
    int value =  parsedValue.toHex().toInt(&ok, 16);

    if(rw == mRead)
    {

    }
    else if(rw == mWrite)
    {
        switch(parsedCommand){

        case mPP:
        {
            ui->scrollPP->setValue(value);
            break;
        }
        case mPI:
        {
            ui->scrollPI->setValue(value);
            break;
        }
        case mPD:
        {
            ui->scrollPD->setValue(value);
            break;
        }
        case mVS:
        {
            ui->scrollVS->setValue(value);
            break;
        }
        case mAC:
        {
            ui->scrollAC->setValue(value);
            break;
        }
        default:
            //nothing for now
            break;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectDevice(){

    emit connectToDevice(ui->m_cbDevicesFound->currentIndex());
}

void MainWindow::initButtons(){
    /* Init Buttons */
    ui->m_pBSearch->setText("Search");
    ui->m_pBConnect->setText("Connect");
    ui->m_pBConnect->setEnabled(false);
}

void MainWindow::statusChanged(const QString &status)
{
    ui->m_textStatus->append(status);
}

void MainWindow::on_ConnectClicked()
{
    if(ui->m_pBConnect->text() == QString("Connect"))
    {
        connectDevice();
    }
    else
    {
        ui->m_textStatus->clear();
        m_bleConnection.disconnectFromDevice();
    }
}

void MainWindow::createMessage(uint8_t msgId, uint8_t rw, QByteArray payload, QByteArray *result)
{
    uint8_t buffer[MaxPayload+8] = {'\0'};
    uint8_t command = msgId;

    int len = message.create_pack(rw , command , payload, buffer);

    for (int i = 0; i < len; i++)
    {
        result->append(buffer[i]);
    }
}

void MainWindow::parseMessage(QByteArray *data, uint8_t &command, QByteArray &value,  uint8_t &rw)
{
    MessagePack parsedMessage;

    uint8_t* dataToParse = reinterpret_cast<uint8_t*>(data->data());
    QByteArray returnValue;
    if(message.parse(dataToParse, (uint8_t)data->length(), &parsedMessage))
    {
        command = parsedMessage.command;
        rw = parsedMessage.rw;
        for(int i = 0; i< parsedMessage.len; i++)
        {
            value.append(parsedMessage.data[i]);
        }
    }
}

void MainWindow::requestData(uint8_t command)
{
    QByteArray payload;
    QByteArray sendData;
    createMessage(command, mRead, payload, &sendData);
    m_bleConnection.writeData(sendData);
}

void MainWindow::sendData(uint8_t command, uint8_t value)
{
    QByteArray payload;
    payload[0] = value;

    QByteArray sendData;
    createMessage(command, mWrite, payload, &sendData);

    m_bleConnection.writeData(sendData);
}

void MainWindow::on_scrollPP_valueChanged(int value)
{
    sendData(mPP, value);
    ui->labelPP->setText(QString::number(value));
}

void MainWindow::on_scrollPI_valueChanged(int value)
{
    sendData(mPI, value);
    ui->labelPI->setText(QString::number(value));
}

void MainWindow::on_scrollPD_valueChanged(int value)
{
    sendData(mPD, value);
    ui->labelPD->setText(QString::number(value));
}

void MainWindow::on_scrollVS_valueChanged(int value)
{
    sendData(mVS, value);
    ui->labelVS->setText(QString::number(value));
}

void MainWindow::on_scrollAC_valueChanged(int value)
{
    sendData(mAC, value);
    ui->labelAC->setText(QString::number(value));
}

void MainWindow::on_Exit()
{
    exit(0);
}

void MainWindow::on_ForwardPressed()
{
    sendData(mForward, 100);
}

void MainWindow::on_ForwardReleased()
{
    sendData(mForward, 0);
}

void MainWindow::on_BackwardPressed()
{
    sendData(mBackward, 100);
}

void MainWindow::on_BackwardReleased()
{
    sendData(mBackward, 0);
}

void MainWindow::on_RightPressed()
{
    sendData(mRight, 100);
}

void MainWindow::on_RightReleased()
{
    sendData(mRight, 0);
}

void MainWindow::on_LeftPressed()
{
    sendData(mLeft, 100);
}

void MainWindow::on_LeftReleased()
{
    sendData(mLeft, 0);
}
