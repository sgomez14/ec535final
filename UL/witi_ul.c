#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>  /* For setting up serial devices?? */

#define DEBUG 0

/* Function prototypes */
unsigned int read_RFID_scanner(int RFID_port_fd, int debug);
void sighandler(int);

/* Global variables */
int mode = 1;

int main(int argc, char *argv[] )
{
  /* Variables */
  int debug;
  int *debug_pointer; 
  unsigned int tag_ID = 0;
  char device_name[] = "/dev/ttyS4";
  int RFID_port_fd = -1;        /* Initialised to -1 for error catching if we can't open the device. */
  struct sigaction action, oa;
  int oflags;
   
  /* Opening a file descriptor* for KL interaction */
  int pFile = -1;
  
  /*
  if ((pFile = open("/dev/mytimer", O_RDWR)) < 0) {
      fputs("mytimer module isn't loaded\n",stderr);
      return -1;
  }
  */  
    
  /* Setup signal handler */
	memset(&action, 0, sizeof(action));
	action.sa_handler = sighandler;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	sigaction(SIGIO, &action, NULL);
	fcntl(pFile, F_SETOWN, getpid());
	oflags = fcntl(pFile, F_GETFL);
	fcntl(pFile, F_SETFL, oflags | FASYNC);
  
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
    printf("Setting debug through global DEFINE.\n");
    debug = DEBUG;
  }
  printf("Debug mode: %d\n", debug);
  
  /* Attributes for sensor */
  struct termios attributes;
  
  /* Baud rate of sensor, applies to input and output */
  speed_t baud_rate = B9600;
  
  /* Opening device. Connected to P9_11 for RX and P9_13 for TX 
   * O_NONBLOCK may be needed (https://stackoverflow.com/questions/13075595/how-do-the-clocal-and-crtscts-flags-in-termios-c-cflag-affect-the-serial-port)
   */
  if ( ( RFID_port_fd = open(device_name, O_RDWR | O_EXCL /*| O_NONBLOCK*/) ) < 0 )
  {
    printf("Error opening device.\n");
    return  -1;
  }

  /* HOW DOES THIS SECTION WORK?? */
  //fcntl( RFID_port_fd, F_SETFL, fcntl( RFID_port_fd, F_GETFL ) & ~O_NONBLOCK );


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
  attributes.c_iflag |= ( IGNPAR );    /* Ignores parity errors */
  
  /* Applying new attributes to device along with when changes take effect.
   * TCSAFLUSH is chosen as the change occurs after all data has been transmitted (?) 
   */
  if ( tcsetattr(RFID_port_fd, TCSAFLUSH ,&attributes) < 0 )
  {
    printf("Error in tcgetattr().\n");
    return  -1;
  }

  if(debug) printf("RFID_port_fd: %d\n",RFID_port_fd);
  
  printf("Please place tag: \n");
  
  int previousValue = 0;
  
  /* Reading RFID module 
   * Per documentation:
   * "In most standard implementations, the user will repeatedly call the desired function until the ERR_OK status byte is returned, indicating that the function successfully completed with no errors." 
   */ 
  while (1)
  {
    tag_ID = 0;
    while (tag_ID == 0) tag_ID = read_RFID_scanner(RFID_port_fd,debug);
    if (tag_ID != previousValue)
    {
      /* Printing RFID tag ID */
      printf("RFID tag ID is: %u \n", tag_ID);
      previousValue = tag_ID;
      switch(tag_ID)
      {
        case 3471480065:
          printf("This is an apple.\n");
          continue;
        case 4108817665:
          printf("This is a banana.\n");
          continue;
        case 2313852161:
          printf("This is a pizza.\n");
          continue;
        default:
          printf("SMDFTB! Tag not found\n");
      }
    }
  }

  return 0;
}

unsigned int read_RFID_scanner(int RFID_port_fd, int debug)
{
  /* Per RFID sensor documentation, we need to send a 3 byte signal to the device to signal
   * a command, and then another byte to signal a specific RFID_Read command.
   */
   
  char RFID_read_command[5] = {'!','R','W','',' '}; 
  /* Weird, but without the space at the end, the RFID module holds during the read... 
   * The square is hex value 0x01, which indicates the read command for our sensor.
   */
  char RFID_output_buffer[5];
  char RFID_tag_ID[4];
  unsigned int  tag_ID = 0;
  
  /* Writing command to RFID 
   * Catching error if all 4 characters of command aren't sent
   */
  if(debug) printf("Writing Instruction...\n");
  if ( write( RFID_port_fd , RFID_read_command , 5 ) != 5 )
  {
    printf("Error in sending instruction to RFID!\n");
    return 0;
  }
  
  /* Reading signal from RFID module
   * Expecting 5 bytes returned. First byte is status, rest of the 32-bit tag ID.
   */
  if(debug) printf("Reading RFID...\n");
  if ( read( RFID_port_fd , &RFID_output_buffer , 5 ) != 5)
  {
    if(debug) printf("Error in reading RFID output!\nOutput Buffer: %s\n",RFID_output_buffer);
    return 0;
  }
  
  /* Checking RFID-output's status. Per product guide, we expect 0x01. */
  if ( (int)RFID_output_buffer[0] != 1)
  {
    if(debug) printf("Error RFID module: %d\n Is a tag present?\n",(int)RFID_output_buffer[0]);
    return 0; 
  }
  
  /* Converting remaining elements of RFID output buffer 
   * to an integer for transmission out of function.
   * Takes the original output_buffer, and copies just the data over to a new buffer
   * which is then recast as an integer. 
   */
  for (int i = 1; i < 5; i++)
  {
    RFID_tag_ID[i-1] = RFID_output_buffer[i];
  }
  
  /* Converting string number into an integer by casting char array to unsigned int pointer, then dereferencing */
  tag_ID = *((unsigned int*) RFID_tag_ID);
  
  if(debug) printf("Data Buffer: %s\nTag_ID: %u\n",RFID_output_buffer,tag_ID);
  
  return tag_ID;
}

/* SIGIO Handler */
void sighandler(int signo)
{
  mode = ~mode;
  printf("Awaken! Hello World from the SIGIO handler!\n");
}