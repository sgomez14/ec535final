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
#include <sstream>

#include <termios.h>


RFIDGui::RFIDGui(/* int sensorPortFd, map<unsigned int, string> rfid_hashmap,*/ int debug_status, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RFIDGui)
{
    /* setup UI */
    ui->setupUi(this);

    /* initialize serial port */
    init_serial_port();

    /* load tag data from file */
    init_tag_data();

    /* initialize read mode */
    init_read_mode();

    /* set internal debug status */
    debug = debug_status;

}

RFIDGui::~RFIDGui()
{
    /* clear all of the textboxes on the gui */
    ui->enter_item_msg->clear();
    ui->item_message->clear();
    ui->mode_message->clear();
    ui->tag_number_message->clear();

    delete ui;
}


void RFIDGui::on_changeModeButton_clicked()
{
    read_mode = false;

    /* error checking that read_mode is still false */
    if (read_mode)
    {
        /* set gui view for read mode */
        ui->mode_message->setText("READ");
        ui->enter_item_label->hide();
        ui->enter_item_msg->hide();
        ui->enter_item_msg->clear();
        ui->tag_number_message->clear();

        /* reset the tag buffer and tag write flag */
        scanned_tag_id = 0;
        tag_for_write_scanned = false;

    }
    else if (read_mode == false)
    {
        /* set flag that help determine if an RFID tag was scanned */
        scanned_tag_id = 0;
        tag_for_write_scanned = false;

        /* set gui view for write mode */
        ui->mode_message->setText("WRITE");
        ui->item_message->clear();
        ui->item_message->setText("Scan tag to enter new message");

        /* set counter that limits how many times the rfid read function is called */
        int counter = 0;

        /* while loop to repeatedly probe rfid module  */
        while (scanned_tag_id <= 1 && counter <= num_read_calls)
        {
            read_RFID_scanner();
            counter++;
        }

        /* signal to user that the rfid reader timed-out */
        if (counter > num_read_calls)
        {
            ui->item_message->setText("RFID reader timed-out: Press WRITE again");
        }

    }
    else
        ui->mode_message->setText("UKNOW MODE");


}

void RFIDGui::on_enter_item_msg_returnPressed()
{

    /* require user to enter message if they want to change a tag message */
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
        rfid_map[scanned_tag_id] = newMessage.toStdString();

        /* open data file that has rfid tag information */
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

        /* continue to indicate that device is in write mode */
        read_mode = false;

        /* reset flags that indicate if a tag has been scanned */
        scanned_tag_id = 0;
        tag_for_write_scanned = false;

        /* update GUI view */
        ui->enter_item_label->hide();
        ui->enter_item_msg->hide();
        ui->enter_item_msg->clear();
        ui->mode_message->setText("WRITE");
        ui->tag_number_message->clear();
        ui->item_message->setText("Scan tag to enter new message");
    }
}


void RFIDGui::read_RFID_scanner()
{
  /*
    Per RFID sensor documentation, we need to send a 3 byte signal to the device to signal
    a command, and then another byte to signal a specific RFID_Read command.

    Weird, but without the space at the end, the RFID module holds during the read...
    The square is hex value 0x01, which indicates the read command for our sensor.
   */
  char RFID_read_command[5] = {'!','R','W','',' '};


  /* variables to store the output from the RFID sensor */
  char RFID_output_buffer[5];
  char RFID_tag_ID[4];
  unsigned int  tag_ID = 0;

  /* variable to check how many bytes written */
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
   * Expecting 5 bytes returned. First byte is status, rest is the 32-bit tag ID.
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
         scanned_tag_id = tag_ID;
         QString tag_message = QString::fromStdString(rfid_map[tag_ID]);

         /* check if the scanned tag is in the database */
         if (tag_message == "")
         {
             ui->item_message->setText("Tag not in database.");
         }
         else
         {
             ui->item_message->setText(tag_message);
         }

         /* display the ID of the scanned tag */
         ui->tag_number_message->setText(QString::fromStdString(to_string(tag_ID)));

         if(debug) cout << "Scanned Message: " << rfid_map[tag_ID] << endl;
     }

  }
  else
  {
      /* check if the scan worked */
      if (tag_ID <= 1)
      {
          ui->item_message->setText("Scan Didn't Work. Scan Again.");
      }
      else if (!tag_for_write_scanned)
      {
          scanned_tag_id = tag_ID;
          tag_for_write_scanned = true;
          ui->enter_item_label->show();
          ui->enter_item_msg->show();
          ui->tag_number_message->setText(QString::fromStdString(to_string(tag_ID)));
      }
  }

}


void RFIDGui::on_turnOffButton_clicked()
{
    QApplication::quit();
}


void RFIDGui::on_read_button_clicked()
{
    /* gui view for read mode */
    read_mode = true;
    ui->mode_message->setText("READ");
    ui->enter_item_label->hide();
    ui->enter_item_msg->hide();
    ui->enter_item_msg->clear();
    ui->tag_number_message->clear();
    ui->item_message->clear();

    /* reset the tag buffer and tag write flag */
    scanned_tag_id = 0;
    tag_for_write_scanned = false;

    int counter = 0;

    while (scanned_tag_id <= 1 && counter <= num_read_calls)
    {
        read_RFID_scanner();
        counter++;
    }

    /* signal to user that the rfid reader timed-out */
    if (counter > num_read_calls)
    {
        ui->item_message->setText("RFID reader timed-out: Press READ again");
    }
}

int RFIDGui::init_serial_port()
{
        /* Attributes for sensor */
        struct termios attributes;

        /* Baud rate of sensor, applies to input and output */
        speed_t baud_rate = B9600;

        /* Opening device. Connected to P9_11 for RX and P9_13 for TX
         */
        if ( ( RFID_port_fd = open(serial_port_name, O_RDWR) ) < 0 )
        {
          printf("Error opening device.\n");
          return  -1;
        }

        /* Getting current attributes for device to populate structure */
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
         * CREAD to enable the receiver.
         */
        attributes.c_cflag |= (CREAD);

        /* Applying new attributes to device along with when changes take effect.
         * TCSAFLUSH is chosen as the change occurs after all data has been transmitted
         */
        if ( tcsetattr(RFID_port_fd, TCSAFLUSH ,&attributes) < 0 )
        {
          printf("Error in tcgetattr().\n");
          return  -1;
        }

        return 0;
}

void RFIDGui::init_tag_data()
{
    ifstream rfid_file;
    unsigned int tag_number;
    string tag_num_string;
    string tag_message;

    /* open data file with tag-message info */
    rfid_file.open("rfid_tag_info.txt");
    if (!rfid_file)
    {
        cout << "Error opening file with RFID tag info." << endl;
    }

    /* read in tag number and associated message */
    while(getline(rfid_file, tag_num_string))
    {
        stringstream str_to_int;

        if (debug) cout << "Number read in from file: " << tag_num_string << endl;

        /* convert string number into an unsigned int */
        str_to_int << tag_num_string;
        str_to_int >> tag_number;

        if (debug) cout << "Number coming out of streambuffer: " << tag_number << endl;

        /* reset the streambuffer */
        str_to_int.flush();

        /* read in tag message */
        getline(rfid_file, tag_message);

        if (debug) cout << "tag#: " << tag_number << " | reading from file message: " << tag_message << endl;

        rfid_map.insert(pair<unsigned int,string>(tag_number,tag_message));
    }

    rfid_file.close();
}

void RFIDGui::init_read_mode()
{
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
}

void RFIDGui::on_read_button_pressed()
{
    read_mode = true;
    ui->mode_message->setText("READ");
    ui->enter_item_label->hide();
    ui->enter_item_msg->hide();
    ui->enter_item_msg->clear();
    ui->tag_number_message->clear();
    ui->item_message->clear();
}

void RFIDGui::on_changeModeButton_pressed()
{
    scanned_tag_id = 0;
    tag_for_write_scanned = false;
    ui->mode_message->setText("WRITE");
    ui->tag_number_message->clear();
    ui->item_message->clear();
    ui->item_message->setText("Scan tag to enter new message");
    ui->enter_item_msg->hide();
    ui->enter_item_msg->clear();
    ui->enter_item_label->hide();
}
