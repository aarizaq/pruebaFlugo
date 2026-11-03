#ifndef GUIPANEL_H
#define GUIPANEL_H

#include <QWidget>
#include <QVector>
#include <QTcpSocket>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
//#include <QNetworkSession>
#include <vector>

namespace Ui {
class GUIPanel;
}

//QT_USE_NAMESPACE_SERIALPORT
#define NMAX 400
#define TMAX 400

class GUIPanel : public QWidget
{
    Q_OBJECT
    
public:
    //GUIPanel(QWidget *parent = 0);
    explicit GUIPanel(QWidget *parent = 0);
    ~GUIPanel(); // Da problemas
    
private slots:
    void on_runButton_clicked();
    void readRequest();
    void displayError(QAbstractSocket::SocketError socketError);
    //void on_ModoCont_stateChanged(int arg1);

    void on_Act1_stateChanged(int arg1);

    void on_Act2_stateChanged(int arg1);

    void on_Act3_stateChanged(int arg1);

private: // funciones privadas
    struct DataMpu6050 // 4
    {
      float x_accel;      // 5
      float y_accel;
      float z_accel;
      float temperature;  // 6
      float x_gyro;       // 7
      float y_gyro;
      float z_gyro;
    };

    struct DataAnalog // 4
    {
      char canal1;      // 5
      char canal2;      // 5
      char canal3;      // 5
    };

    void startSlave();
    void activateRunButton();
    void cambiaDig();
    void recibirDatosMovACCEL(const std::vector<DataMpu6050> &);
    void recibirDatosMovGYRO(const std::vector<DataMpu6050> &);
    void recibirDatosTemp(const std::vector<DataMpu6050> &);
    void recibirDatosAnalog(const std::vector<DataAnalog> &datos);

    void preparaGraficaTemp();
    void preparaGraficaAcel();
    void preparaGraficaGiro();
    void preparaGraficaAnalog();

private:
    Ui::GUIPanel *ui;
    int transactionCount;
    bool connected;
    QByteArray request;

    int time;
    double xVal[TMAX];
    double yValTemp[NMAX];
    double xVal2[NMAX];
    double yValAcceX[NMAX];
    double yValAcceY[NMAX];
    double yValAcceZ[NMAX];
    double yValGyroX[NMAX];
    double yValGyroY[NMAX];
    double yValGyroZ[NMAX];

    double canal1Datos[NMAX];
    double canal2Datos[NMAX];
    double canal3Datos[NMAX];

    QwtPlotCurve *m_curve_a1;
    QwtPlotGrid  *m_Grid;
    QwtPlotCurve *m_curve_AcceX;
    QwtPlotCurve *m_curve_AcceY;
    QwtPlotCurve *m_curve_AcceZ;
    QwtPlotCurve *m_curve_GyroX;
    QwtPlotCurve *m_curve_GyroY;
    QwtPlotCurve *m_curve_GyroZ;

    // canales analogicos
    QwtPlotCurve *m_curveAnalog1;
    QwtPlotCurve *m_curveAnalog2;
    QwtPlotCurve *m_curveAnalog3;

    QTcpSocket *tcpSocket;
};

#endif // GUIPANEL_H
