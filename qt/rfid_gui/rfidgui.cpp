#include "rfidgui.h"
#include "ui_rfidgui.h"
#include <fcntl.h>
#include <unistd.h>

RFIDGui::RFIDGui(int sensorPortFd, int debug_status, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RFIDGui)
{
    ui->setupUi(this);

    RFID_port_fd = sensorPortFd;
    debug = debug_status;

    if (read_mode)
        ui->mode_message->setText("READ");
    else if (read_mode == false)
        ui->mode_message->setText("WRITE");
    else
        ui->mode_message->setText("UKNOW MODE");

    /* initialize timer for reading RFID scanner */
    connect(&this->rfidTimer, SIGNAL(timeout()), this, SLOT(read_RFID_scanner()));
    this->rfidTimer.start(100);

}

RFIDGui::~RFIDGui()
{
    delete ui;
}


void RFIDGui::on_changeModeButton_clicked()
{
    read_mode = !read_mode;
    if (read_mode)
        ui->mode_message->setText("READ");
    else if (read_mode == false)
        ui->mode_message->setText("WRITE");
    else
        ui->mode_message->setText("UKNOW MODE");



}

void RFIDGui::on_enter_item_msg_returnPressed()
{
    ui->enter_item_msg->clear();
}

void RFIDGui::tmpRead()
{
    //unsigned int tag_ID = 0;

    //while (tag_ID == 0) tag_ID = read_RFID_scanner();
    ui->item_message->setText("timer display message");
}


void RFIDGui::read_RFID_scanner()
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
    return;
  }

  /* Reading signal from RFID module
   * Expecting 5 bytes returned. First byte is status, rest of the 32-bit tag ID.
   */
  if(debug) printf("Reading RFID...\n");
  if ( read( RFID_port_fd , &RFID_output_buffer , 5 ) != 5)
  {
    if(debug) printf("Error in reading RFID output!\nOutput Buffer: %s\n",RFID_output_buffer);
    return;
  }

  /* Checking RFID-output's status. Per product guide, we expect 0x01. */
  if ( (int)RFID_output_buffer[0] != 1)
  {
    if(debug) printf("Error RFID module: %d\n Is a tag present?\n",(int)RFID_output_buffer[0]);
    return;
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

  //ui->item_message->setText("tags says this");

  switch(tag_ID)
        {
          case 3471480065:
            ui->item_message->setText("This is an apple.");
            break;
          case 4108817665:
            ui->item_message->setText("This is a banana.");
            break;
          case 2313852161:
            ui->item_message->setText("This is a pizza.");
            break;
          default:
            ui->item_message->setText("Tag not found.");
        }

  //return tag_ID;
}




void RFIDGui::on_turnOffButton_clicked()
{
    QApplication::quit();
}
