#include "rfidgui.h"

#include <QApplication>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

/* includes for C code */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define DEBUG 0


using namespace std;

int main(int argc, char *argv[])
{

    /* Variables */
    int debug;
    /*int *debug_pointer;
    unsigned int tag_ID = 0;*/
    char device_name[] = "/dev/ttyS4";
    int RFID_port_fd = -1;        /* Initialised to -1 for error catching if we can't open the device. */
    /*struct sigaction action, oa;
    int oflags;*/

    /* Configuring debug logic */
    if ( argc == 2 && (*argv[1] == 49))
    {
      printf("Setting debug through argument: 1\n");
      debug = 1;
    }
    else if ( argc == 2 && (*argv[1] == 48))
    {
      printf("Setting debug through argument: 0\n");
      debug = 0;
    }
    else
    {
      //printf("Setting debug through global DEFINE.\n");
      debug = DEBUG;
    }

    /* hashmap for storing rfid tag IDs and their associated messages */
    map<unsigned int, string> rfid_map;
    ifstream rfid_file;
    unsigned int tag_number;
    string tag_num_string;
    stringstream str_to_int;
    string tag_message;
    rfid_file.open("rfid_tag_info.txt");
    if (!rfid_file)
    {
        cout << "Error opening file with RFID tag info." << endl;
    }

    /* read in tag number */
    while(getline(rfid_file, tag_num_string))
    {
        str_to_int << tag_num_string;
        str_to_int >> tag_number;

        /* read in tag message */
        getline(rfid_file, tag_message);
        rfid_map.insert(pair<unsigned int,string>(tag_number,tag_message));
    }

    rfid_file.close();

    /* Attributes for sensor */
    struct termios attributes;

    /* Baud rate of sensor, applies to input and output */
    speed_t baud_rate = B9600;

    /* Opening device. Connected to P9_11 for RX and P9_13 for TX
     * O_NONBLOCK may be needed (https://stackoverflow.com/questions/13075595/how-do-the-clocal-and-crtscts-flags-in-termios-c-cflag-affect-the-serial-port)
     */
    if ( ( RFID_port_fd = open(device_name, O_RDWR /*| O_EXCL | O_NONBLOCK*/) ) < 0 )
    {
      printf("Error opening device.\n");
      return  -1;
    }

    //turning off non-blocking
    /*fcntl( RFID_port_fd, F_SETFL, fcntl( RFID_port_fd, F_GETFL ) & ~O_NONBLOCK );*/

    /* Getting current attributes for device to populate structure
     * Configures attributes based on default values (?)
     */
    if ( tcgetattr(RFID_port_fd,&attributes) < 0 )
    {
      printf("Error in tcgetattr().\n");
      return  -1;
    }


    /* Setting terminal to RAW mode to prevent unwanted processing */
    cfmakeraw(&attributes);

    /* Setting input and output baud rate for attribute struct*/
    cfsetispeed(&attributes,baud_rate);
    cfsetospeed(&attributes,baud_rate);

    /* Setting control flags:
     * CREAD to enable the receiver. We may need CLOCAL if we have blocking issues.
     * If so, we need to add a O_NONBLOCK on the earlier open() command.
     */
    attributes.c_cflag |= ( CREAD /*| CLOCAL*/ );

    /* Applying new attributes to device along with when changes take effect.
     * TCSAFLUSH is chosen as the change occurs after all data has been transmitted
     */
    if ( tcsetattr(RFID_port_fd, TCSAFLUSH ,&attributes) < 0 )
    {
      printf("Error in tcgetattr().\n");
      return  -1;
    }

//    int RFID_port_fd = 1;
//    printf("RFID_port_fd: %d\n", RFID_port_fd);

    QApplication a(argc, argv);
    RFIDGui gui_screen(RFID_port_fd, rfid_map);
    gui_screen.show();
    return a.exec();
}
