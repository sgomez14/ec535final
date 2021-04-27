/* driver code that initializes GUI for Linux RFID scanning device */

#include "rfidgui.h"

#include <QApplication>



using namespace std;

int main(int argc, char *argv[])
{

    /* initialize Qt gui application */
    QApplication a(argc, argv);
    RFIDGui gui_screen;
    gui_screen.show();
    return a.exec();
}
