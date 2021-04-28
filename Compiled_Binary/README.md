# ec535final

EC 535 Spring 2021 Final Project
To successfully run the program on the Beaglebone from a cold boot:
1. Make sure "rfid_tag_info.txt" is in the /root/ folder.
2. Chmod text file and rfid_gui.
3. Launch ./rfid_gui 
4. Once GUI is open, press either "Read" or "Write". A tag does not need to be present. 
5. Wait for it to crash. This is a persistent bug in the code we were unable to solve. A message should print.
6. Click off.
7. Relaunch ./rfid_gui.  The program should now work fully as designed. 
8. Interact with program by either placing tags and then interfacing with the screen or vice versa.  Tags can sometimes be finicky and may need to be moved slightly if read isn't instantaenous.
