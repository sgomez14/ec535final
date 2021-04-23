#ifndef RFIDGUI_H
#define RFIDGUI_H

#include <QMainWindow>
#include <QTimer>
#include <map>
#include <string>
//#include <QSerialPort>

//#include <termios.h>
//#include <string.h>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class RFIDGui; }
QT_END_NAMESPACE

class RFIDGui : public QMainWindow
{
    Q_OBJECT

public:
    RFIDGui(/*int sensorPortFd, map<unsigned int, string> rfid_hashmap,*/ int debug_status = 0, QWidget *parent = nullptr);
    ~RFIDGui();


private slots:
    void on_changeModeButton_clicked();

    /* custom slot function for reading RFID sensor */
    void read_RFID_scanner();

    void on_enter_item_msg_returnPressed();

    void on_turnOffButton_clicked();

    void tmpRead();

    //void on_changeModeButton_pressed();


    void on_read_button_clicked();

    void on_read_button_pressed();

    void on_changeModeButton_pressed();

private:
    Ui::RFIDGui *ui;
    QTimer rfidTimer;
    int rfidTimer_delay;
    int debug;
    map<unsigned int, string> rfid_map;
    unsigned int scanned_tag_id;
    bool tag_for_write_scanned;
    bool read_mode;
    const int num_read_calls = 100;


    /* settings for serial port */
//    QSerialPort serial_port;
//    const QString serial_port_name = "/dev/ttyS4";
    int RFID_port_fd;
    char serial_port_name[11] = "/dev/ttyS4";
//    speed_t baud_rate = B9600; //QSerialPort::Baud9600;

    int  init_serial_port();
    void init_tag_data();
    void init_read_mode();

};
#endif // RFIDGUI_H
