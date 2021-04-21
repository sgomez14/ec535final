#ifndef RFIDGUI_H
#define RFIDGUI_H

#include <QMainWindow>
#include <QTimer>
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
    RFIDGui(int sensorPortFd, map<unsigned int, string> rfid_hashmap, int debug_status = 0, QWidget *parent = nullptr);
    ~RFIDGui();


private slots:
    void on_changeModeButton_clicked();

    /* custom slot function for reading RFID sensor */
    void read_RFID_scanner();

    void on_enter_item_msg_returnPressed();

    void on_turnOffButton_clicked();

    void tmpRead();

    void on_changeModeButton_pressed();

    void on_turnOffButton_pressed();

private:
    Ui::RFIDGui *ui;
    QTimer rfidTimer;
    int RFID_port_fd;
    int debug;
    map<unsigned int, string> rfid_map;
    unsigned int tag_for_write;
    bool tag_for_write_scanned;
    bool read_mode;

};
#endif // RFIDGUI_H