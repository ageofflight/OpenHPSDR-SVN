#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hpsdr/discovery.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    running=false;


    labels<<"Rx"<<"DSP Client"<<"PID"<<"Audio"<<"GUI Client"<<"Frequency";
    model.setHorizontalHeaderLabels(labels);

    ui->tableViewClients->setModel(&model);
    ui->tableViewClients->verticalHeader()->hide();

    // connect up menu items
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(actionAbout()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(actionQuit()));

    // connect up status items
    connect(ui->pushButtonStartStop,SIGNAL(clicked()),this,SLOT(actionStartStop()));

    // connect up configure items
    connect(ui->pushButtonUpdate,SIGNAL(clicked()),this,SLOT(saveConfiguration()));

    connect(ui->radioButtonHermes,SIGNAL(clicked()),this,SLOT(hermesSelected()));
    connect(ui->radioButtonHPSDR,SIGNAL(clicked()),this,SLOT(hpsdrSelected()));

    connect(ui->radioButtonOzy,SIGNAL(clicked()),this,SLOT(ozySelected()));
    connect(ui->radioButtonMetis,SIGNAL(clicked()),this,SLOT(metisSelected()));

    connect(ui->checkBoxMercury,SIGNAL(clicked()),this,SLOT(mercurySelected()));
    connect(ui->checkBoxPenelope,SIGNAL(clicked()),this,SLOT(penelopeSelected()));
    connect(ui->checkBoxPennylane,SIGNAL(clicked()),this,SLOT(pennylaneSelected()));
    connect(ui->checkBoxExcalibur,SIGNAL(clicked()),this,SLOT(excaliburSelected()));
    connect(ui->checkBoxAlex,SIGNAL(clicked()),this,SLOT(alexSelected()));

    connect(ui->comboBox10MHzClockSource,SIGNAL(activated(QString)),this,SLOT(clock10Selected(QString)));
    connect(ui->comboBox122_88MHzClockSource,SIGNAL(activated(QString)),this,SLOT(clock122_88Selected(QString)));

    connect(ui->comboBoxSampleRate,SIGNAL(activated(QString)),this,SLOT(sampleRateSelected(QString)));

    connect(ui->comboBoxInterface,SIGNAL(activated(QString)),this,SLOT(interfaceSelected(QString)));
    connect(ui->pushButtonDiscover,SIGNAL(clicked()),this,SLOT(actionDiscover()));
    connect(ui->comboBoxMetis,SIGNAL(activated(QString)),this,SLOT(metisCardSelected(QString)));


    // setup configuration
    settings=new QSettings("QtHPSDRServer.ini",QSettings::IniFormat);

    alex=Alex::getInstance();
    alex->configure(settings);
    ui->radioButton160RxAnt1->setChecked((alex->getRx(BAND_160)==ANT1));
    ui->radioButton160RxAnt2->setChecked((alex->getRx(BAND_160)==ANT2));
    ui->radioButton160RxAnt3->setChecked((alex->getRx(BAND_160)==ANT3));
    ui->radioButton160RxRX1->setChecked((alex->getRx(BAND_160)==RX1));
    ui->radioButton160RxRX2->setChecked((alex->getRx(BAND_160)==RX2));
    ui->radioButton160RxXVRX->setChecked((alex->getRx(BAND_160)==XVRX));
    ui->radioButton160TxAnt1->setChecked((alex->getTx(BAND_160)==ANT1));
    ui->radioButton160TxAnt2->setChecked((alex->getTx(BAND_160)==ANT2));
    ui->radioButton160TxAnt3->setChecked((alex->getTx(BAND_160)==ANT3));

    ui->radioButton80RxAnt1->setChecked((alex->getRx(BAND_80)==ANT1));
    ui->radioButton80RxAnt2->setChecked((alex->getRx(BAND_80)==ANT2));
    ui->radioButton80RxAnt3->setChecked((alex->getRx(BAND_80)==ANT3));
    ui->radioButton80RxRX1->setChecked((alex->getRx(BAND_80)==RX1));
    ui->radioButton80RxRX2->setChecked((alex->getRx(BAND_80)==RX2));
    ui->radioButton80RxXVRX->setChecked((alex->getRx(BAND_80)==XVRX));
    ui->radioButton80TxAnt1->setChecked((alex->getTx(BAND_80)==ANT1));
    ui->radioButton80TxAnt2->setChecked((alex->getTx(BAND_80)==ANT2));
    ui->radioButton80TxAnt3->setChecked((alex->getTx(BAND_80)==ANT3));

    ui->radioButton60RxAnt1->setChecked((alex->getRx(BAND_60)==ANT1));
    ui->radioButton60RxAnt2->setChecked((alex->getRx(BAND_60)==ANT2));
    ui->radioButton60RxAnt3->setChecked((alex->getRx(BAND_60)==ANT3));
    ui->radioButton60RxRX1->setChecked((alex->getRx(BAND_60)==RX1));
    ui->radioButton60RxRX2->setChecked((alex->getRx(BAND_60)==RX2));
    ui->radioButton60RxXVRX->setChecked((alex->getRx(BAND_60)==XVRX));
    ui->radioButton60TxAnt1->setChecked((alex->getTx(BAND_60)==ANT1));
    ui->radioButton60TxAnt2->setChecked((alex->getTx(BAND_60)==ANT2));
    ui->radioButton60TxAnt3->setChecked((alex->getTx(BAND_60)==ANT3));

    ui->radioButton40RxAnt1->setChecked((alex->getRx(BAND_40)==ANT1));
    ui->radioButton40RxAnt2->setChecked((alex->getRx(BAND_40)==ANT2));
    ui->radioButton40RxAnt3->setChecked((alex->getRx(BAND_40)==ANT3));
    ui->radioButton40RxRX1->setChecked((alex->getRx(BAND_40)==RX1));
    ui->radioButton40RxRX2->setChecked((alex->getRx(BAND_40)==RX2));
    ui->radioButton40RxXVRX->setChecked((alex->getRx(BAND_40)==XVRX));
    ui->radioButton40TxAnt1->setChecked((alex->getTx(BAND_40)==ANT1));
    ui->radioButton40TxAnt2->setChecked((alex->getTx(BAND_40)==ANT2));
    ui->radioButton40TxAnt3->setChecked((alex->getTx(BAND_40)==ANT3));

    ui->radioButton30RxAnt1->setChecked((alex->getRx(BAND_30)==ANT1));
    ui->radioButton30RxAnt2->setChecked((alex->getRx(BAND_30)==ANT2));
    ui->radioButton30RxAnt3->setChecked((alex->getRx(BAND_30)==ANT3));
    ui->radioButton30RxRX1->setChecked((alex->getRx(BAND_30)==RX1));
    ui->radioButton30RxRX2->setChecked((alex->getRx(BAND_30)==RX2));
    ui->radioButton30RxXVRX->setChecked((alex->getRx(BAND_30)==XVRX));
    ui->radioButton30TxAnt1->setChecked((alex->getTx(BAND_30)==ANT1));
    ui->radioButton30TxAnt2->setChecked((alex->getTx(BAND_30)==ANT2));
    ui->radioButton30TxAnt3->setChecked((alex->getTx(BAND_30)==ANT3));

    ui->radioButton20RxAnt1->setChecked((alex->getRx(BAND_20)==ANT1));
    ui->radioButton20RxAnt2->setChecked((alex->getRx(BAND_20)==ANT2));
    ui->radioButton20RxAnt3->setChecked((alex->getRx(BAND_20)==ANT3));
    ui->radioButton20RxRX1->setChecked((alex->getRx(BAND_20)==RX1));
    ui->radioButton20RxRX2->setChecked((alex->getRx(BAND_20)==RX2));
    ui->radioButton20RxXVRX->setChecked((alex->getRx(BAND_20)==XVRX));
    ui->radioButton20TxAnt1->setChecked((alex->getTx(BAND_20)==ANT1));
    ui->radioButton20TxAnt2->setChecked((alex->getTx(BAND_20)==ANT2));
    ui->radioButton20TxAnt3->setChecked((alex->getTx(BAND_20)==ANT3));

    ui->radioButton17RxAnt1->setChecked((alex->getRx(BAND_17)==ANT1));
    ui->radioButton17RxAnt2->setChecked((alex->getRx(BAND_17)==ANT2));
    ui->radioButton17RxAnt3->setChecked((alex->getRx(BAND_17)==ANT3));
    ui->radioButton17RxRX1->setChecked((alex->getRx(BAND_17)==RX1));
    ui->radioButton17RxRX2->setChecked((alex->getRx(BAND_17)==RX2));
    ui->radioButton17RxXVRX->setChecked((alex->getRx(BAND_17)==XVRX));
    ui->radioButton17TxAnt1->setChecked((alex->getTx(BAND_17)==ANT1));
    ui->radioButton17TxAnt2->setChecked((alex->getTx(BAND_17)==ANT2));
    ui->radioButton17TxAnt3->setChecked((alex->getTx(BAND_17)==ANT3));

    ui->radioButton15RxAnt1->setChecked((alex->getRx(BAND_15)==ANT1));
    ui->radioButton15RxAnt2->setChecked((alex->getRx(BAND_15)==ANT2));
    ui->radioButton15RxAnt3->setChecked((alex->getRx(BAND_15)==ANT3));
    ui->radioButton15RxRX1->setChecked((alex->getRx(BAND_15)==RX1));
    ui->radioButton15RxRX2->setChecked((alex->getRx(BAND_15)==RX2));
    ui->radioButton15RxXVRX->setChecked((alex->getRx(BAND_15)==XVRX));
    ui->radioButton15TxAnt1->setChecked((alex->getTx(BAND_15)==ANT1));
    ui->radioButton15TxAnt2->setChecked((alex->getTx(BAND_15)==ANT2));
    ui->radioButton15TxAnt3->setChecked((alex->getTx(BAND_15)==ANT3));

    ui->radioButton12RxAnt1->setChecked((alex->getRx(BAND_12)==ANT1));
    ui->radioButton12RxAnt2->setChecked((alex->getRx(BAND_12)==ANT2));
    ui->radioButton12RxAnt3->setChecked((alex->getRx(BAND_12)==ANT3));
    ui->radioButton12RxRX1->setChecked((alex->getRx(BAND_12)==RX1));
    ui->radioButton12RxRX2->setChecked((alex->getRx(BAND_12)==RX2));
    ui->radioButton12RxXVRX->setChecked((alex->getRx(BAND_12)==XVRX));
    ui->radioButton12TxAnt1->setChecked((alex->getTx(BAND_12)==ANT1));
    ui->radioButton12TxAnt2->setChecked((alex->getTx(BAND_12)==ANT2));
    ui->radioButton12TxAnt3->setChecked((alex->getTx(BAND_12)==ANT3));

    ui->radioButton10RxAnt1->setChecked((alex->getRx(BAND_10)==ANT1));
    ui->radioButton10RxAnt2->setChecked((alex->getRx(BAND_10)==ANT2));
    ui->radioButton10RxAnt3->setChecked((alex->getRx(BAND_10)==ANT3));
    ui->radioButton10RxRX1->setChecked((alex->getRx(BAND_10)==RX1));
    ui->radioButton10RxRX2->setChecked((alex->getRx(BAND_10)==RX2));
    ui->radioButton10RxXVRX->setChecked((alex->getRx(BAND_10)==XVRX));
    ui->radioButton10TxAnt1->setChecked((alex->getTx(BAND_10)==ANT1));
    ui->radioButton10TxAnt2->setChecked((alex->getTx(BAND_10)==ANT2));
    ui->radioButton10TxAnt3->setChecked((alex->getTx(BAND_10)==ANT3));

    ui->radioButton6RxAnt1->setChecked((alex->getRx(BAND_6)==ANT1));
    ui->radioButton6RxAnt2->setChecked((alex->getRx(BAND_6)==ANT2));
    ui->radioButton6RxAnt3->setChecked((alex->getRx(BAND_6)==ANT3));
    ui->radioButton6RxRX1->setChecked((alex->getRx(BAND_6)==RX1));
    ui->radioButton6RxRX2->setChecked((alex->getRx(BAND_6)==RX2));
    ui->radioButton6RxXVRX->setChecked((alex->getRx(BAND_6)==XVRX));
    ui->radioButton6TxAnt1->setChecked((alex->getTx(BAND_6)==ANT1));
    ui->radioButton6TxAnt2->setChecked((alex->getTx(BAND_6)==ANT2));
    ui->radioButton6TxAnt3->setChecked((alex->getTx(BAND_6)==ANT3));

    ui->radioButtonGENRxAnt1->setChecked((alex->getRx(BAND_GEN)==ANT1));
    ui->radioButtonGENRxAnt2->setChecked((alex->getRx(BAND_GEN)==ANT2));
    ui->radioButtonGENRxAnt3->setChecked((alex->getRx(BAND_GEN)==ANT3));
    ui->radioButtonGENRxRX1->setChecked((alex->getRx(BAND_GEN)==RX1));
    ui->radioButtonGENRxRX2->setChecked((alex->getRx(BAND_GEN)==RX2));
    ui->radioButtonGENRxXVRX->setChecked((alex->getRx(BAND_GEN)==XVRX));
    ui->radioButtonGENTxAnt1->setChecked((alex->getTx(BAND_GEN)==ANT1));
    ui->radioButtonGENTxAnt2->setChecked((alex->getTx(BAND_GEN)==ANT2));
    ui->radioButtonGENTxAnt3->setChecked((alex->getTx(BAND_GEN)==ANT3));

    connect(ui->pushButtonAlexSave,SIGNAL(clicked()),this,SLOT(alexSaveConfiguration()));

    server=Server::getInstance();
    server->configure(settings);

    ui->radioButtonHermes->setChecked(server->getHermes());
    ui->radioButtonHPSDR->setChecked(server->getHPSDR());
    ui->radioButtonOzy->setChecked(server->getOzy());
    ui->radioButtonMetis->setChecked(server->getMetis());
    ui->checkBoxMercury->setChecked(server->getMercury());
    ui->checkBoxPenelope->setChecked(server->getPenelope());
    ui->checkBoxPennylane->setChecked(server->getPennylane());
    ui->checkBoxExcalibur->setChecked(server->getExcalibur());
    ui->checkBoxAlex->setChecked(server->getAlex());
    ui->checkBoxRandom->setChecked(server->getRandom());
    ui->checkBoxDither->setChecked(server->getDither());

    int index=ui->comboBoxSampleRate->findText(QString::number(server->getSampleRate()));
    if(index!=-1) {
        ui->comboBoxSampleRate->setCurrentIndex(index);
    }

    index=ui->comboBox10MHzClockSource->findText(server->get10MHzClock());
    if(index!=-1) {
        ui->comboBox10MHzClockSource->setCurrentIndex(index);
    }

    index=ui->comboBox122_88MHzClockSource->findText(server->get122_88MHzClock());
    if(index!=-1) {
        ui->comboBox122_88MHzClockSource->setCurrentIndex(index);
    }

    updateInterfaces();

    index=ui->comboBoxInterface->findText(server->getInterface());
    if(index!=-1) {
        ui->comboBoxInterface->setCurrentIndex(index);
    }
    if(server->getInterface()!="") {
        server->bind();
        actionDiscover();
        index=ui->comboBoxMetis->findText(server->getMetisDetail());
        if(index!=-1) {
            ui->comboBoxMetis->setCurrentIndex(index);
        }
    }

    ui->checkBoxAutoStartServer->setChecked(server->getAutoStart());
    ui->checkBoxAutoStartDSPServer->setChecked(server->getAutoStartDsp());

    clientListener=new ClientListener();
    clientListener->configure(settings);

    connect(clientListener,SIGNAL(clientConnected()),this,SLOT(updateClientList()));
    connect(clientListener,SIGNAL(clientDisconnected()),this,SLOT(updateClientList()));
    connect(server,SIGNAL(clientStateChanged()),this,SLOT(updateClientList()));
    connect(server,SIGNAL(firmwareVersionChanged()),this,SLOT(firmwareVersionChanged()));

    connect(ui->checkBoxAutoStartServer,SIGNAL(clicked()),this,SLOT(autoStartSelected()));
    connect(ui->checkBoxAutoStartDSPServer,SIGNAL(clicked()),this,SLOT(autoStartDSPSelected()));

    if(server->getAutoStart()) {
        server->start();
    }

    updateClientList();
}

void MainWindow::saveSettings() {
    server->save();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    qDebug()<<"MainWindow::closeEvent";
    server->stop();
    saveSettings();
}

void MainWindow::actionAbout() {
    // show a dialog with application information
}

void MainWindow::actionQuit() {
    qDebug()<<"MainWindow::actionQuit";
    server->stop();
    saveSettings();
    exit(0);
}

void MainWindow::actionDiscover() {
    Discovery* discovery=new Discovery();
    ui->comboBoxMetis->clear();
    for(int i=0;i<server->getMetisCount();i++) {
        ui->comboBoxMetis->addItem(server->getMetisText(i));
    }
}

void MainWindow::actionStartStop() {
    if(running) {
        ui->pushButtonStartStop->setText(QString("Start"));
        server->stop();
        running=false;
    } else {
        ui->pushButtonStartStop->setText(QString("Stop"));
        server->start();
        running=true;
    }
}

void MainWindow::saveConfiguration() {
    // save the configuration
    server->save();
}

void MainWindow::alexSaveConfiguration() {
    if(ui->radioButton160RxAnt1->isChecked()) alex->setRx(BAND_160,ANT1);
    if(ui->radioButton160RxAnt2->isChecked()) alex->setRx(BAND_160,ANT2);
    if(ui->radioButton160RxAnt3->isChecked()) alex->setRx(BAND_160,ANT3);
    if(ui->radioButton160RxRX1->isChecked()) alex->setRx(BAND_160,RX1);
    if(ui->radioButton160RxRX2->isChecked()) alex->setRx(BAND_160,RX2);
    if(ui->radioButton160RxXVRX->isChecked()) alex->setRx(BAND_160,XVRX);
    if(ui->radioButton160TxAnt1->isChecked()) alex->setTx(BAND_160,ANT1);
    if(ui->radioButton160TxAnt2->isChecked()) alex->setTx(BAND_160,ANT2);
    if(ui->radioButton160TxAnt3->isChecked()) alex->setTx(BAND_160,ANT3);

    if(ui->radioButton80RxAnt1->isChecked()) alex->setRx(BAND_80,ANT1);
    if(ui->radioButton80RxAnt2->isChecked()) alex->setRx(BAND_80,ANT2);
    if(ui->radioButton80RxAnt3->isChecked()) alex->setRx(BAND_80,ANT3);
    if(ui->radioButton80RxRX1->isChecked()) alex->setRx(BAND_80,RX1);
    if(ui->radioButton80RxRX2->isChecked()) alex->setRx(BAND_80,RX2);
    if(ui->radioButton80RxXVRX->isChecked()) alex->setRx(BAND_80,XVRX);
    if(ui->radioButton80TxAnt1->isChecked()) alex->setTx(BAND_80,ANT1);
    if(ui->radioButton80TxAnt2->isChecked()) alex->setTx(BAND_80,ANT2);
    if(ui->radioButton80TxAnt3->isChecked()) alex->setTx(BAND_80,ANT3);

    if(ui->radioButton60RxAnt1->isChecked()) alex->setRx(BAND_60,ANT1);
    if(ui->radioButton60RxAnt2->isChecked()) alex->setRx(BAND_60,ANT2);
    if(ui->radioButton60RxAnt3->isChecked()) alex->setRx(BAND_60,ANT3);
    if(ui->radioButton60RxRX1->isChecked()) alex->setRx(BAND_60,RX1);
    if(ui->radioButton60RxRX2->isChecked()) alex->setRx(BAND_60,RX2);
    if(ui->radioButton60RxXVRX->isChecked()) alex->setRx(BAND_60,XVRX);
    if(ui->radioButton60TxAnt1->isChecked()) alex->setTx(BAND_60,ANT1);
    if(ui->radioButton60TxAnt2->isChecked()) alex->setTx(BAND_60,ANT2);
    if(ui->radioButton60TxAnt3->isChecked()) alex->setTx(BAND_60,ANT3);

    if(ui->radioButton40RxAnt1->isChecked()) alex->setRx(BAND_40,ANT1);
    if(ui->radioButton40RxAnt2->isChecked()) alex->setRx(BAND_40,ANT2);
    if(ui->radioButton40RxAnt3->isChecked()) alex->setRx(BAND_40,ANT3);
    if(ui->radioButton40RxRX1->isChecked()) alex->setRx(BAND_40,RX1);
    if(ui->radioButton40RxRX2->isChecked()) alex->setRx(BAND_40,RX2);
    if(ui->radioButton40RxXVRX->isChecked()) alex->setRx(BAND_40,XVRX);
    if(ui->radioButton40TxAnt1->isChecked()) alex->setTx(BAND_40,ANT1);
    if(ui->radioButton40TxAnt2->isChecked()) alex->setTx(BAND_40,ANT2);
    if(ui->radioButton40TxAnt3->isChecked()) alex->setTx(BAND_40,ANT3);

    if(ui->radioButton30RxAnt1->isChecked()) alex->setRx(BAND_30,ANT1);
    if(ui->radioButton30RxAnt2->isChecked()) alex->setRx(BAND_30,ANT2);
    if(ui->radioButton30RxAnt3->isChecked()) alex->setRx(BAND_30,ANT3);
    if(ui->radioButton30RxRX1->isChecked()) alex->setRx(BAND_30,RX1);
    if(ui->radioButton30RxRX2->isChecked()) alex->setRx(BAND_30,RX2);
    if(ui->radioButton30RxXVRX->isChecked()) alex->setRx(BAND_30,XVRX);
    if(ui->radioButton30TxAnt1->isChecked()) alex->setTx(BAND_30,ANT1);
    if(ui->radioButton30TxAnt2->isChecked()) alex->setTx(BAND_30,ANT2);
    if(ui->radioButton30TxAnt3->isChecked()) alex->setTx(BAND_30,ANT3);

    if(ui->radioButton20RxAnt1->isChecked()) alex->setRx(BAND_20,ANT1);
    if(ui->radioButton20RxAnt2->isChecked()) alex->setRx(BAND_20,ANT2);
    if(ui->radioButton20RxAnt3->isChecked()) alex->setRx(BAND_20,ANT3);
    if(ui->radioButton20RxRX1->isChecked()) alex->setRx(BAND_20,RX1);
    if(ui->radioButton20RxRX2->isChecked()) alex->setRx(BAND_20,RX2);
    if(ui->radioButton20RxXVRX->isChecked()) alex->setRx(BAND_20,XVRX);
    if(ui->radioButton20TxAnt1->isChecked()) alex->setTx(BAND_20,ANT1);
    if(ui->radioButton20TxAnt2->isChecked()) alex->setTx(BAND_20,ANT2);
    if(ui->radioButton20TxAnt3->isChecked()) alex->setTx(BAND_20,ANT3);

    if(ui->radioButton17RxAnt1->isChecked()) alex->setRx(BAND_17,ANT1);
    if(ui->radioButton17RxAnt2->isChecked()) alex->setRx(BAND_17,ANT2);
    if(ui->radioButton17RxAnt3->isChecked()) alex->setRx(BAND_17,ANT3);
    if(ui->radioButton17RxRX1->isChecked()) alex->setRx(BAND_17,RX1);
    if(ui->radioButton17RxRX2->isChecked()) alex->setRx(BAND_17,RX2);
    if(ui->radioButton17RxXVRX->isChecked()) alex->setRx(BAND_17,XVRX);
    if(ui->radioButton17TxAnt1->isChecked()) alex->setTx(BAND_17,ANT1);
    if(ui->radioButton17TxAnt2->isChecked()) alex->setTx(BAND_17,ANT2);
    if(ui->radioButton17TxAnt3->isChecked()) alex->setTx(BAND_17,ANT3);

    if(ui->radioButton15RxAnt1->isChecked()) alex->setRx(BAND_15,ANT1);
    if(ui->radioButton15RxAnt2->isChecked()) alex->setRx(BAND_15,ANT2);
    if(ui->radioButton15RxAnt3->isChecked()) alex->setRx(BAND_15,ANT3);
    if(ui->radioButton15RxRX1->isChecked()) alex->setRx(BAND_15,RX1);
    if(ui->radioButton15RxRX2->isChecked()) alex->setRx(BAND_15,RX2);
    if(ui->radioButton15RxXVRX->isChecked()) alex->setRx(BAND_15,XVRX);
    if(ui->radioButton15TxAnt1->isChecked()) alex->setTx(BAND_15,ANT1);
    if(ui->radioButton15TxAnt2->isChecked()) alex->setTx(BAND_15,ANT2);
    if(ui->radioButton15TxAnt3->isChecked()) alex->setTx(BAND_15,ANT3);

    if(ui->radioButton12RxAnt1->isChecked()) alex->setRx(BAND_12,ANT1);
    if(ui->radioButton12RxAnt2->isChecked()) alex->setRx(BAND_12,ANT2);
    if(ui->radioButton12RxAnt3->isChecked()) alex->setRx(BAND_12,ANT3);
    if(ui->radioButton12RxRX1->isChecked()) alex->setRx(BAND_12,RX1);
    if(ui->radioButton12RxRX2->isChecked()) alex->setRx(BAND_12,RX2);
    if(ui->radioButton12RxXVRX->isChecked()) alex->setRx(BAND_12,XVRX);
    if(ui->radioButton12TxAnt1->isChecked()) alex->setTx(BAND_12,ANT1);
    if(ui->radioButton12TxAnt2->isChecked()) alex->setTx(BAND_12,ANT2);
    if(ui->radioButton12TxAnt3->isChecked()) alex->setTx(BAND_12,ANT3);

    if(ui->radioButton10RxAnt1->isChecked()) alex->setRx(BAND_10,ANT1);
    if(ui->radioButton10RxAnt2->isChecked()) alex->setRx(BAND_10,ANT2);
    if(ui->radioButton10RxAnt3->isChecked()) alex->setRx(BAND_10,ANT3);
    if(ui->radioButton10RxRX1->isChecked()) alex->setRx(BAND_10,RX1);
    if(ui->radioButton10RxRX2->isChecked()) alex->setRx(BAND_10,RX2);
    if(ui->radioButton10RxXVRX->isChecked()) alex->setRx(BAND_10,XVRX);
    if(ui->radioButton10TxAnt1->isChecked()) alex->setTx(BAND_10,ANT1);
    if(ui->radioButton10TxAnt2->isChecked()) alex->setTx(BAND_10,ANT2);
    if(ui->radioButton10TxAnt3->isChecked()) alex->setTx(BAND_10,ANT3);

    if(ui->radioButton6RxAnt1->isChecked()) alex->setRx(BAND_6,ANT1);
    if(ui->radioButton6RxAnt2->isChecked()) alex->setRx(BAND_6,ANT2);
    if(ui->radioButton6RxAnt3->isChecked()) alex->setRx(BAND_6,ANT3);
    if(ui->radioButton6RxRX1->isChecked()) alex->setRx(BAND_6,RX1);
    if(ui->radioButton6RxRX2->isChecked()) alex->setRx(BAND_6,RX2);
    if(ui->radioButton6RxXVRX->isChecked()) alex->setRx(BAND_6,XVRX);
    if(ui->radioButton6TxAnt1->isChecked()) alex->setTx(BAND_6,ANT1);
    if(ui->radioButton6TxAnt2->isChecked()) alex->setTx(BAND_6,ANT2);
    if(ui->radioButton6TxAnt3->isChecked()) alex->setTx(BAND_6,ANT3);

    if(ui->radioButtonGENRxAnt1->isChecked()) alex->setRx(BAND_GEN,ANT1);
    if(ui->radioButtonGENRxAnt2->isChecked()) alex->setRx(BAND_GEN,ANT2);
    if(ui->radioButtonGENRxAnt3->isChecked()) alex->setRx(BAND_GEN,ANT3);
    if(ui->radioButtonGENRxRX1->isChecked()) alex->setRx(BAND_GEN,RX1);
    if(ui->radioButtonGENRxRX2->isChecked()) alex->setRx(BAND_GEN,RX2);
    if(ui->radioButtonGENRxXVRX->isChecked()) alex->setRx(BAND_GEN,XVRX);
    if(ui->radioButtonGENTxAnt1->isChecked()) alex->setTx(BAND_GEN,ANT1);
    if(ui->radioButtonGENTxAnt2->isChecked()) alex->setTx(BAND_GEN,ANT2);
    if(ui->radioButtonGENTxAnt3->isChecked()) alex->setTx(BAND_GEN,ANT3);

    alex->save();
}

void MainWindow::hpsdrSelected() {
    server->setHPSDR(true);
    ui->checkBoxMercury->setEnabled(true);
    ui->spinBoxReceivers->setEnabled(true);
    ui->checkBoxPenelope->setEnabled(true);
    ui->checkBoxPennylane->setEnabled(true);
    ui->checkBoxExcalibur->setEnabled(true);
    ui->comboBox10MHzClockSource->setEnabled(true);
    ui->comboBox122_88MHzClockSource->setEnabled(true);
    ui->checkBoxAlex->setEnabled(true);

    update10MHzClockSources();
    update122_88MHzClockSources();

    if(server->getMetis()) {
        updateInterfaces();
    }
}

void MainWindow::hermesSelected() {

    server->setHermes(true);
    ui->checkBoxMercury->setEnabled(false);
    ui->spinBoxReceivers->setEnabled(false);
    ui->checkBoxPenelope->setEnabled(false);
    ui->checkBoxPennylane->setEnabled(false);
    ui->checkBoxExcalibur->setEnabled(false);
    ui->comboBox10MHzClockSource->setEnabled(false);
    ui->comboBox122_88MHzClockSource->setEnabled(false);
    ui->checkBoxAlex->setEnabled(false);

    ui->checkBoxMercury->setChecked(true);
    ui->checkBoxPenelope->setChecked(false);
    ui->checkBoxPennylane->setChecked(true);
    ui->checkBoxExcalibur->setChecked(false);
    ui->checkBoxAlex->setChecked(false);

    ui->lineEditMetisPort->setEnabled(true);
    ui->comboBoxInterface->setEnabled(true);
    ui->comboBoxMetis->setEnabled(true);
    ui->pushButtonDiscover->setEnabled(true);

    update10MHzClockSources();
    update122_88MHzClockSources();

    updateInterfaces();
}

void MainWindow::ozySelected() {
    server->setOzy(true);

    ui->lineEditMetisPort->setEnabled(false);
    ui->comboBoxInterface->setEnabled(false);
    ui->comboBoxMetis->setEnabled(false);
    ui->pushButtonDiscover->setEnabled(false);
}

void MainWindow::metisSelected() {
    server->setMetis(true);
    ui->lineEditMetisPort->setEnabled(true);
    ui->comboBoxInterface->setEnabled(true);
    ui->comboBoxMetis->setEnabled(true);
    ui->pushButtonDiscover->setEnabled(true);
    updateInterfaces();
}

void MainWindow::interfaceSelected(QString interface) {
    server->setInterface(interface);
    server->bind();
    ui->comboBoxMetis->clear();
}

void MainWindow::metisCardSelected(QString card) {
    server->setMetisDetail(card);
}

void MainWindow::mercurySelected() {
    server->setMercury(ui->checkBoxMercury->isChecked());
    update10MHzClockSources();
    update122_88MHzClockSources();
}

void MainWindow::penelopeSelected() {
    server->setPenelope(ui->checkBoxPenelope->isChecked());
    if(server->getPenelope()) {
        ui->checkBoxPennylane->setChecked(false);
    }
    update10MHzClockSources();
    update122_88MHzClockSources();
}

void MainWindow::pennylaneSelected() {
    server->setPennylane(ui->checkBoxPennylane->isChecked());
    if(server->getPennylane()) {
        ui->checkBoxPenelope->setChecked(false);
    }
    update10MHzClockSources();
    update122_88MHzClockSources();
}

void MainWindow::excaliburSelected() {
    server->setExcalibur(ui->checkBoxExcalibur->isChecked());
    update10MHzClockSources();
}

void MainWindow::alexSelected() {
    server->setAlex(ui->checkBoxAlex->isChecked());
    ui->tabAlex->setEnabled(server->getAlex());
}

void MainWindow::clock10Selected(QString clock) {
    server->set10MHzClock(clock);
}

void MainWindow::clock122_88Selected(QString clock) {
    server->set122_88MHzClock(clock);
}

void MainWindow::update10MHzClockSources() {
    ui->comboBox10MHzClockSource->clear();
    if(server->getMercury()) ui->comboBox10MHzClockSource->addItem(QString("Mercury"));
    if(server->getPenelope()) ui->comboBox10MHzClockSource->addItem(QString("Penelope"));
    if(server->getPennylane()) ui->comboBox10MHzClockSource->addItem(QString("Pennylane"));
    if(server->getExcalibur()) ui->comboBox10MHzClockSource->addItem(QString("Excalibur"));
}

void MainWindow::update122_88MHzClockSources() {
    ui->comboBox122_88MHzClockSource->clear();
    if(server->getMercury()) ui->comboBox122_88MHzClockSource->addItem(QString("Mercury"));
    if(server->getPenelope()) ui->comboBox122_88MHzClockSource->addItem(QString("Penelope"));
    if(server->getPennylane()) ui->comboBox122_88MHzClockSource->addItem(QString("Pennylane"));
}

void MainWindow::sampleRateSelected(QString rate) {
    server->setSampleRate(rate.toInt());
}

void MainWindow::autoStartSelected() {
    server->setAutoStart(ui->checkBoxAutoStartServer->isChecked());
}

void MainWindow::autoStartDSPSelected() {
    server->setAutoStartDsp(ui->checkBoxAutoStartDSPServer->isChecked());
}

void MainWindow::clientConnected() {
    qDebug()<<"MainWindow::clientConnected";
    // update the client list
    updateClientList();
}

void MainWindow::clientDisconnected() {
    qDebug()<<"MainWindow::clientDisconnected";
    // update the client list
    updateClientList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getConfigDir() {
    if (!configDir.isNull()) {
        return configDir;
    }
    // Search config file

    QString binDir=QCoreApplication::applicationDirPath();
    QString organization=QCoreApplication::organizationName();
    QString configFileName=QCoreApplication::applicationName()+".ini";

    QStringList searchList;
    searchList.append(QDir::cleanPath(binDir));
    searchList.append(QDir::cleanPath(binDir+"/../etc"));
    searchList.append(QDir::cleanPath(binDir+"/../../etc")); // for development under windows
    searchList.append(QDir::rootPath()+"etc/xdg/"+organization);
    searchList.append(QDir::rootPath()+"etc/opt");
    searchList.append(QDir::rootPath()+"etc");

    foreach (QString dir, searchList) {
        QFile file(dir+"/"+configFileName);
        if (file.exists()) {
            // found
            configDir=dir;
            qDebug("Using config file %s",qPrintable(file.fileName()));
            return configDir;
        }
    }

    // not found
    foreach (QString dir, searchList) {
        qWarning("%s/%s not found",qPrintable(dir),qPrintable(configFileName));
    }
    qDebug("Cannot find config file %s",qPrintable(configFileName));
    configDir=binDir;
    return configDir;
}

void MainWindow::updateInterfaces() {
    Interfaces* interfaces=server->getInterfaces();
    ui->comboBoxInterface->clear();
    for(int i=0;i<interfaces->count();i++) {
        ui->comboBoxInterface->addItem(interfaces->getInterfaceNameAt(i));
    }
}

void MainWindow::updateClientList() {
    QStringList receiverInfo;
    QList<QStandardItem> items;
    model.clear();
    model.setHorizontalHeaderLabels(labels);

    int receivers=server->getReceivers();
    for(int i=0;i<receivers;i++) {
        receiverInfo=server->getReceiverInfo(i);
        for(int j=0;j<receiverInfo.length();j++) {
            QStandardItem* item=new QStandardItem(receiverInfo.at(j));
            model.setItem(i,j,item);
        }
    }

    ui->tableViewClients->setColumnWidth(0,50);
    ui->tableViewClients->setColumnWidth(1,150);
    ui->tableViewClients->setColumnWidth(2,100);
    ui->tableViewClients->setColumnWidth(3,50);
    ui->tableViewClients->setColumnWidth(4,100);
    ui->tableViewClients->setColumnWidth(5,100);


}

void MainWindow::firmwareVersionChanged() {
    ui->labelMetisVersion->setText(QString::number(server->getOzySoftwareVersion()));
    ui->labelMercuryVersion->setText(QString::number(server->getMercurySoftwareVersion()));
    ui->labelPenelopeVersion->setText(QString::number(server->getPenelopeSoftwareVersion()));
}
