#include "GUIPanel.h"
#include "ui_GUIPanel.h"

#include <qlayout.h>
#include <qwt_compass.h>
#include <qwt_compass_rose.h>
#include <qwt_dial_needle.h>
#include <vector>
#include <QMessageBox>
#include <QtMqtt/QMqttClient>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QNetworkProxy>


#define TAMBUFFER 20

//QT_USE_NAMESPACE_SERIALPORT

unsigned int contMonstrarMov =0;


void escribirJson(const QString &rutaArchivo) {
    // 1. Crear el objeto principal
    QJsonArray listaUsuarios;
    QJsonObject objetoPrincipal1;
    objetoPrincipal1["usuario"] = "Juan Pérez";
    objetoPrincipal1["activo"] = true;

    // 2. Crear un array e insertarlo
    QJsonArray listaLenguajes;
    listaLenguajes.append("C++");
    listaLenguajes.append("Python");
    objetoPrincipal1["lenguajes"] = listaLenguajes;

    QJsonObject objetoPrincipal2;
    objetoPrincipal2["usuario"] = "Juan Pérez";
    objetoPrincipal2["activo"] = true;

    // 2. Crear un array e insertarlo
     QJsonArray listaLenguajes2;
    listaLenguajes2.append("C++");
    listaLenguajes2.append("Python");
    objetoPrincipal2["lenguajes"] = listaLenguajes2;
    listaUsuarios.append(objetoPrincipal1);
    listaUsuarios.append(objetoPrincipal2);

    // 3. Envolver en un documento
    QJsonDocument doc(listaUsuarios);

    // 4. Guardar en el archivo (formato legible/identado)
    QFile archivo(rutaArchivo);
    if (archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        archivo.write(doc.toJson(QJsonDocument::Indented)); // o .Compact para ahorrar espacio
        archivo.close();
        qDebug() << "Archivo JSON guardado con éxito.";
    } else {
        qWarning() << "No se pudo abrir el archivo para escritura.";
    }
}

void leerMultiplesUsuarios(const QString &rutaArchivo) {
    QFile archivo(rutaArchivo);
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Error al abrir el archivo para leer.";
        return;
    }

    QByteArray datos = archivo.readAll();
    archivo.close();

    QJsonDocument doc = QJsonDocument::fromJson(datos);

    // Validar que el nodo raíz sea un Array
    if (doc.isArray()) {
        QJsonArray listaUsuarios = doc.array();

        // Iterar de forma segura usando un bucle "for-range" de C++
        for (const QJsonValue &valor : std::as_const(listaUsuarios)) {
            // Cada elemento del array debe ser un objeto
            if (valor.isObject()) {
                QJsonObject usuario = valor.toObject();

                QString nombre = usuario["nombre"].toString();
                int edad = usuario["edad"].toInt();
                bool activo = usuario["activo"].toBool();

                qDebug() << "--- Usuario Encontrado ---";
                qDebug() << "Nombre:" << nombre;
                qDebug() << "Edad:" << edad;
                qDebug() << "Estado:" << (activo ? "Activo" : "Inactivo");
            }
        }
    } else {
        qWarning() << "El formato del JSON no es un array en la raíz.";
    }
}

void GUIPanel::preparaGraficaTemp()
{
    // Preparacion grafica temperatura

    ui->GraficaTempObj->setTitle("Temperatura Objeto");
    ui->GraficaTempObj->setAxisTitle(QwtPlot::xBottom, "Tiempo");
    ui->GraficaTempObj->setAxisTitle(QwtPlot::yLeft, "Valor ( ºC )");
    ui->GraficaTempObj->axisAutoScale(true); // Con Autoescala
    ui->GraficaTempObj->setAutoReplot(false);

    m_curve_a1 = new QwtPlotCurve();
    m_curve_a1->setPen(QPen(Qt::red));

    m_Grid = new QwtPlotGrid();
    m_Grid->attach(ui->GraficaTempObj);

    m_curve_a1->attach(ui->GraficaTempObj);
}

void GUIPanel::preparaGraficaAcel()
{
    ui->GraficaSensorAccel->setTitle("Sensor Movimiento - Acelerometro");
    ui->GraficaSensorAccel->setAxisTitle(QwtPlot::xBottom, "Muestras");
    ui->GraficaSensorAccel->setAxisTitle(QwtPlot::yLeft, "Valor");
    ui->GraficaSensorAccel->axisAutoScale(true); // Con Autoescala
    ui->GraficaSensorAccel->setAutoReplot(false);

    m_curve_AcceX = new QwtPlotCurve();
    m_curve_AcceX->setPen(QPen(Qt::red));
    m_curve_AcceY = new QwtPlotCurve();
    m_curve_AcceY->setPen(QPen(Qt::blue));
    m_curve_AcceZ = new QwtPlotCurve();
    m_curve_AcceZ->setPen(QPen(Qt::darkGreen));

    m_Grid = new QwtPlotGrid();
    m_Grid->attach(ui->GraficaSensorAccel);
    m_curve_AcceX->attach(ui->GraficaSensorAccel);
    m_curve_AcceY->attach(ui->GraficaSensorAccel);
    m_curve_AcceZ->attach(ui->GraficaSensorAccel);
}


void GUIPanel::preparaGraficaGiro()
{
    ui->GraficaSensorGyro->setTitle("Sensor Movimiento - Giroscopio");
    ui->GraficaSensorGyro->setAxisTitle(QwtPlot::xBottom, "Muestras");
    ui->GraficaSensorGyro->setAxisTitle(QwtPlot::yLeft, "Valor");
    ui->GraficaSensorGyro->axisAutoScale(true); // Con Autoescala
    ui->GraficaSensorGyro->setAutoReplot(false);


    m_curve_GyroX = new QwtPlotCurve();
    m_curve_GyroX->setPen(QPen(Qt::red));
    m_curve_GyroY = new QwtPlotCurve();
    m_curve_GyroY->setPen(QPen(Qt::blue));
    m_curve_GyroZ = new QwtPlotCurve();
    m_curve_GyroZ->setPen(QPen(Qt::darkGreen));


    m_Grid = new QwtPlotGrid();
    m_Grid->attach(ui->GraficaSensorGyro);
    m_curve_GyroX->attach(ui->GraficaSensorGyro);
    m_curve_GyroY->attach(ui->GraficaSensorGyro);
    m_curve_GyroZ->attach(ui->GraficaSensorGyro);

}


void GUIPanel::preparaGraficaAnalog()
{

    ui->qwtPlotCanalesAnalogicos->setTitle("Sensor Movimiento - Giroscopio");
    ui->qwtPlotCanalesAnalogicos->setAxisTitle(QwtPlot::xBottom, "Muestras");
    ui->qwtPlotCanalesAnalogicos->setAxisTitle(QwtPlot::yLeft, "Valor");
    ui->qwtPlotCanalesAnalogicos->axisAutoScale(true); // Con Autoescala
    ui->qwtPlotCanalesAnalogicos->setAutoReplot(false);


    m_curveAnalog1 = new QwtPlotCurve();
    m_curveAnalog1->setPen(QPen(Qt::red));
    m_curveAnalog2 = new QwtPlotCurve();
    m_curveAnalog2->setPen(QPen(Qt::blue));
    m_curveAnalog3 = new QwtPlotCurve();
    m_curveAnalog3->setPen(QPen(Qt::darkGreen));


    m_Grid = new QwtPlotGrid();
    m_Grid->attach(ui->qwtPlotCanalesAnalogicos);
    m_curveAnalog1->attach(ui->qwtPlotCanalesAnalogicos);
    m_curveAnalog2->attach(ui->qwtPlotCanalesAnalogicos);
    m_curveAnalog3->attach(ui->qwtPlotCanalesAnalogicos);

}

GUIPanel::GUIPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GUIPanel)
    , transactionCount(0)
{
    ui->setupUi(this);
    setWindowTitle(tr("Interfaz de Control"));

// inicializa los vectores de datos a 0

    memset(yValTemp,0,sizeof(yValTemp));
    memset(xVal2,0,sizeof(xVal2));
    memset(yValAcceX,0,sizeof(yValAcceX));
    memset(yValAcceY,0,sizeof(yValAcceY));
    memset(yValAcceZ,0,sizeof(yValAcceZ));
    memset(yValGyroX,0,sizeof(yValGyroX));
    memset(yValGyroY,0,sizeof(yValGyroY));
    memset(yValGyroZ,0,sizeof(yValGyroZ));
    memset(canal1Datos,0,sizeof(canal1Datos));
    memset(canal2Datos,0,sizeof(canal2Datos));
    memset(canal3Datos,0,sizeof(canal3Datos));

    // Inicializa los vectores usados como ejes de tiempos
    for (int i=0; i<TMAX; i++) {
        xVal[i]=i;

    }

    for (int i=0; i<NMAX; i++) {
        xVal2[i]=i;
    }
    //Preparamos las gráficas

    //Parte de la grafica:

    preparaGraficaTemp();
    preparaGraficaAcel();
    preparaGraficaGiro();
    preparaGraficaAnalog();


    connected=false;

    tcpSocket = new QTcpSocket(this); // socket Udp
    connect(tcpSocket, SIGNAL(readyRead()), // En cuanto se estñe preparado para leer, se lanza una
            this, SLOT(readRequest()));   // petición de datos por el puerto serie
    connect(tcpSocket,  &QTcpSocket::errorOccurred,this, &GUIPanel::displayError);
    tcpSocket->setProxy(QNetworkProxy::NoProxy);

    activateRunButton();
// // prueba si encuentra la librería mqtt
//     auto client = std::make_shared<QMqttClient>(this); // puntero inteligente para que lo libere automáticamente si no seusa más
//     client->setHostname("broker.hivemq.com");
//     client->setPort(1883);
//     client->connectToHost();
}

void GUIPanel::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("GUI panel"),
                                 tr("Host no encontrado, compruebe direccion y puerto."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("GUI panel"),
                                 tr("Connexión rechazada, compruebe que el servidor está corriendo y que el puerto y la dirección son correctas"));
        break;
    default:
        QMessageBox::information(this, tr("GUI panel"),
                                 tr("Ha ocurrido el siguiente error: %1.")
                                 .arg(tcpSocket->errorString()));
    }
    activateRunButton();
}

GUIPanel::~GUIPanel()
{
    delete ui;
}


void GUIPanel::readRequest()
{
    int tam;
    std::vector<DataMpu6050> datos;
    std::vector<int8_t> msg;
    msg.resize(1400); //max tam posible
    datos.resize(TAMBUFFER);
    //request.append(tcpSocket->readAll());
    tam = tcpSocket->read((char *)msg.data(), msg.size());
    if (msg[0] == 1)
    {
        // copy
        int numDatos = tam/ sizeof(DataMpu6050);
        datos.resize(numDatos);
        memcpy(datos.data(), &msg[1], sizeof(DataMpu6050) * numDatos);
        // printf ("Datos recividos acel x:  %f\n",datos.x_accel);
        if (datos[0].temperature < 0)
        {
            ui->statusLabel_2->setText("Remoto reporta Error en bus I2C");
            //QMessageBox::information(this, tr("GUI panel"),tr("Sistema reporta error de lectura en el bus I2C Abortando la conexión "));
            //tcpSocket->abort();
            //activateRunButton();
            return;
        }
        if (numDatos > 0)
        {   //Paquete aparentemente correcto, se puede procesar
            ui->statusLabel_2->setText("Recepción de datos remoto");
            recibirDatosMovACCEL(datos);
            recibirDatosMovGYRO(datos);
            recibirDatosTemp(datos);
        }
    }
}

void GUIPanel::startSlave()
{

    tcpSocket->abort();
    tcpSocket->connectToHost(ui->direccionIp->text(),ui->puerto->text().toInt());

    //habilitamos los diferentes widgets
    ui->runButton->setEnabled(false);
    ui->statusLabel->setText(tr("Estado: Ejecucion, conectado al nodo %1.")
                         .arg(ui->direccionIp->text()));
    connected=true;
}


void GUIPanel::activateRunButton()
{
    ui->runButton->setEnabled(true);
}


void GUIPanel::on_runButton_clicked()
{
    startSlave();
}


void GUIPanel::cambiaDig()
{
    DataAnalog parametro; // Se crea una estructura con los parametros del comando

     if (tcpSocket->state() == QAbstractSocket::ConnectedState)
     {
            // Se rellenan los parametros del paquete (en este caso, el estado de los LED)
            parametro.canal1 = ui->Act1->isChecked();
            parametro.canal2 = ui->Act2->isChecked();
            parametro.canal3 = ui->Act3->isChecked();
            // Se crea la trama con n de secuencia 0; comando COMANDO_LEDS; se le pasa la
            // estructura de parametros, indicando su tamaño; el 100 final es el tamaño maximo
            // de trama
            tcpSocket->write(reinterpret_cast<char*>(&parametro),sizeof(parametro));
     }
}



void GUIPanel::recibirDatosMovACCEL(const std::vector<DataMpu6050> &datos){


    //Actualizamos gráfica correspondiente
    memmove(yValAcceX,yValAcceX+datos.size(),sizeof(double)*(NMAX-datos.size()));
    memmove(yValAcceY,yValAcceY+datos.size(),sizeof(double)*(NMAX-datos.size()));
    memmove(yValAcceZ,yValAcceZ+datos.size(),sizeof(double)*(NMAX-datos.size()));
    for (int i = 0; i < TAMBUFFER; i++)
    {
        yValAcceX[NMAX-datos.size()+i]=datos[i].x_accel;
        yValAcceY[NMAX-datos.size()+i]=datos[i].y_accel;
        yValAcceZ[NMAX-datos.size()+i]=datos[i].z_accel;
    }
    m_curve_AcceX->setRawSamples(xVal2,yValAcceX,NMAX);
    m_curve_AcceY->setRawSamples(xVal2,yValAcceY,NMAX);
    m_curve_AcceZ->setRawSamples(xVal2,yValAcceZ,NMAX);
    ui->GraficaSensorAccel->replot();
}

void GUIPanel::recibirDatosMovGYRO(const std::vector<DataMpu6050> &datos){


    memmove(yValGyroX,yValGyroX+datos.size(),sizeof(double)*(NMAX-datos.size()));
    memmove(yValGyroY,yValGyroY+datos.size(),sizeof(double)*(NMAX-datos.size()));
    memmove(yValGyroZ,yValGyroZ+datos.size(),sizeof(double)*(NMAX-datos.size()));
    for (unsigned int i = 0; i < datos.size(); i++)
    {
        yValGyroX[NMAX-datos.size()+i] = datos[i].x_gyro;
        yValGyroY[NMAX-datos.size()+i] = datos[i].y_gyro;
        yValGyroZ[NMAX-datos.size()+i] = datos[i].z_gyro;
    }
    m_curve_GyroX->setRawSamples(xVal2,yValGyroX,NMAX);
    m_curve_GyroY->setRawSamples(xVal2,yValGyroY,NMAX);
    m_curve_GyroZ->setRawSamples(xVal2,yValGyroZ,NMAX);
    ui->GraficaSensorGyro->replot();

}



void GUIPanel::recibirDatosAnalog(const std::vector<DataAnalog> &datos){


    memmove(canal1Datos,canal1Datos+datos.size(),sizeof(double)*(NMAX-datos.size()));
    memmove(canal2Datos,canal2Datos+datos.size(),sizeof(double)*(NMAX-datos.size()));
    memmove(canal3Datos,canal3Datos+datos.size(),sizeof(double)*(NMAX-datos.size()));
    for (unsigned int i = 0; i < datos.size(); i++)
    {
        canal1Datos[NMAX-datos.size()+i] = datos[i].canal1;
        canal2Datos[NMAX-datos.size()+i] = datos[i].canal1;
        canal3Datos[NMAX-datos.size()+i] = datos[i].canal1;
    }
    m_curveAnalog1->setRawSamples(xVal2,canal1Datos,NMAX);
    m_curveAnalog2->setRawSamples(xVal2,canal2Datos,NMAX);
    m_curveAnalog3->setRawSamples(xVal2,canal3Datos,NMAX);
    ui->qwtPlotCanalesAnalogicos->replot();

}



void GUIPanel::recibirDatosTemp(const std::vector<DataMpu6050> &datos){
    //Introducimos los valores en los termómetros de la interfaz gráfica

    //Actualizamos gráfica correspondiente
    memmove(yValTemp,yValTemp+datos.size(),sizeof(double)*(TMAX-datos.size()));

    for (unsigned int i = 0; i < datos.size(); i++)
        yValTemp[TMAX-datos.size()+i] = datos[i].temperature;

    m_curve_a1->setRawSamples(xVal,yValTemp,TMAX);
    ui->GraficaTempObj->replot();
}

void GUIPanel::on_Act1_stateChanged(int arg1)
{
    cambiaDig();
}

void GUIPanel::on_Act2_stateChanged(int arg1)
{
    cambiaDig();
}

void GUIPanel::on_Act3_stateChanged(int arg1)
{
    cambiaDig();
}


