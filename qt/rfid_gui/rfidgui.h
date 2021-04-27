#ifndef RFIDGUI_H
#define RFIDGUI_H

#include <QMainWindow>
#include <map>
#include <string>


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

    /* custom slot function for reading RFID sensor */
    void read_RFID_scanner();

    /* when user presses enter button on keyboard */
    void on_enter_item_msg_returnPressed();

    /* when turn off button pressed and released */
    void on_turnOffButton_clicked();

    /* when read mode button pressed and released */
    void on_read_button_clicked();

    /* when read mode button pressed */
    void on_read_button_pressed();

    /* when write mode button pressed */
    void on_changeModeButton_pressed();

    /* when write mode button pressed and released */
    void on_changeModeButton_clicked();

private:
    Ui::RFIDGui *ui;
    int debug;

    /* data structure to store tags and their messages */
    map<unsigned int, string> rfid_map;

    /* variables to track when an RFID tag has been scanned */
    unsigned int scanned_tag_id;
    bool tag_for_write_scanned;

    /* variable to track read mode status */
    bool read_mode;

    /* variable is the threshold for when a function should stop probing the RFID sensor */
    const int num_read_calls = 100;


    /* settings for serial port */
    int RFID_port_fd;
    char serial_port_name[11] = "/dev/ttyS4";

    /* initialization functions */
    int  init_serial_port();
    void init_tag_data();
    void init_read_mode();

};
#endif // RFIDGUI_H
