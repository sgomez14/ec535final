#include "rfidgui.h"
#include "ui_rfidgui.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <QString>
#include <map>
#include <string>
#include <iostream>
#include <fstream>


RFIDGui::RFIDGui(int sensorPortFd, map<unsigned int, string> rfid_hashmap, int debug_status, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RFIDGui)
{
    ui->setupUi(this);

    RFID_port_fd = sensorPortFd;
    rfid_map = rfid_hashmap;
    debug = debug_status;
    tag_for_write_scanned = false;
    read_mode = true;

    if (read_mode)
    {
        ui->mode_message->setText("READ");
        ui->enter_item_label->hide();
        ui->enter_item_msg->hide();
    }
    else if (read_mode == false)
    {
        ui->mode_message->setText("WRITE");
        ui->enter_item_label->show();
        ui->enter_item_msg->show();
    }
    else
        ui->mode_message->setText("UKNOW MODE");

    /* initialize timer for reading RFID scanner */
    connect(&this->rfidTimer, SIGNAL(timeout()), this, SLOT(read_RFID_scanner()));
    this->rfidTimer.start(750);

}

RFIDGui::~RFIDGui()
{
    ui->enter_item_msg->clear();
    ui->item_message->clear();
    delete ui;
}


void RFIDGui::on_changeModeButton_clicked()
{
    read_mode = !read_mode;
    if (read_mode)
    {
        ui->mode_message->setText("READ");
        ui->enter_item_label->hide();
        ui->enter_item_msg->hide();
    }
    else if (read_mode == false)
    {
        ui->mode_message->setText("WRITE");
        ui->item_message->clear();
        ui->item_message->setText("Scan tag to enter new message");
//        ui->enter_item_label->show();
//        ui->enter_item_msg->show();
    }
    else
        ui->mode_message->setText("UKNOW MODE");


}

void RFIDGui::on_enter_item_msg_returnPressed()
{

    if (ui->enter_item_msg->text().isEmpty())
    {
        ui->enter_item_msg->clear();
        ui->item_message->clear();
        ui->item_message->setText("Enter a message to save to tag");
    }
    else
    {
        /* retreive message entered into textbox*/
        QString newMessage = ui->enter_item_msg->text();

        /* save the new message in the rfid map */
        rfid_map[tag_for_write] = newMessage.toStdString();

        ofstream rfid_file;
        rfid_file.open("rfid_tag_info.txt");
        if (!rfid_file)
        {
            cout << "Error opening file with RFID tag info." << endl;
        }

        /* save rfid map to text file */
        for (auto const& x : rfid_map)
        {
            rfid_file << x.first << endl;
            rfid_file << x.second << endl;
        }

        rfid_file.close();

        read_mode = true;
        tag_for_write = 0;
        tag_for_write_scanned = false;
        ui->enter_item_label->hide();
        ui->enter_item_msg->hide();
        ui->enter_item_msg->clear();
        ui->mode_message->setText("READ");
        connect(&this->rfidTimer, SIGNAL(timeout()), this, SLOT(read_RFID_scanner()));
        this->rfidTimer.start(750);
    }
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
  int olen;

  /* Writing command to RFID
   * Catching error if all 4 characters of command aren't sent
   */

  if(debug) printf("Writing Instruction...\n");
  if ((olen = write( RFID_port_fd , RFID_read_command , 5 )) != 5 )
  {
    printf("Error in sending instruction to RFID!\n");
    printf("total bytes written: %d\n", olen);
    printf("errno: %d\n", errno);
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

  if (read_mode)
  {
     if (tag_ID > 1)
     {
         QString tag_message = QString::fromStdString(rfid_map[tag_ID]);
         ui->item_message->setText(tag_message);
         ui->tag_number_message->setText(QString::fromStdString(to_string(tag_ID)));
         cout << "Scanned Message: " << rfid_map[tag_ID] << endl;
     }

  }
  else
  {
      if (tag_ID == 0)
      {
          ui->item_message->setText("Scan Didn't Work. Scan Again.");
      }
      else if (!tag_for_write_scanned)
      {
          tag_for_write = tag_ID;
          tag_for_write_scanned = true;
          ui->enter_item_label->show();
          ui->enter_item_msg->show();
          ui->tag_number_message->setText(QString::fromStdString(to_string(tag_ID)));
          rfidTimer.stop();
      }
  }

}




void RFIDGui::on_turnOffButton_clicked()
{
    QApplication::quit();
}

void RFIDGui::on_changeModeButton_pressed()
{
    //rfidTimer.stop();
}

void RFIDGui::on_turnOffButton_pressed()
{
    rfidTimer.stop();
}
