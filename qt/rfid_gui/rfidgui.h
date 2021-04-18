#ifndef RFIDGUI_H
#define RFIDGUI_H

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class RFIDGui; }
QT_END_NAMESPACE

class RFIDGui : public QMainWindow
{
    Q_OBJECT

public:
    RFIDGui(int sensorPortFd ,int debug_status = 0, QWidget *parent = nullptr);
    ~RFIDGui();
    bool read_mode = true;

private slots:
    void on_pushButton_clicked();

    /* custom slot function for reading RFID sensor */
    void read_RFID_scanner();

    void on_enter_item_msg_returnPressed();

private:
    Ui::RFIDGui *ui;
    QTimer rfidTimer;
    int RFID_port_fd;
    int debug;

};
#endif // RFIDGUI_H
