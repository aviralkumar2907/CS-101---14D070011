#                                                 CS 101 PROJECT SPRING 2015
                                                      GROUP NUMBER 418
                                          PROJECT TITLE "Optimised Ball Collector"
                                                 14D070011 - Sudeep Salgia 
                                                 140070031 - Aviral Kumar 
                                                 140070001 - Sohum Dhar
                                                 140110091 - muthamizhSelvan S. 
                  Youtube working project link : https://www.youtube.com/watch?v=on3euKLPWc0
                  Youtube video tutorial link  :


Instructions for Setup:

       Please ensure that the CodeBlocks IDE integrated with OpenCV 2.4.10 or a later version  and GTK+ 2.24 or a later             version is installed on your device.
       
       For the installation of OpenCV please follow the instructions provided in the link below.
              http://kevinhughes.ca/tutorials/opencv-install-on-windows-with-codeblocks-and-mingw/

       For the installation of GTK+ 2.24 please follow the instructions provided in the links below.
       https://www.youtube.com/watch?v=sD9jdmtG1gM
       
       http://ftp.gnome.org/pub/gnome/binaries/win32/gtk+/2.24/gtk+-bundle_2.24.10-20120208_win32.zip
       
       Please move the header files – “shortestpath.h” and “xbee.h” into the folder containing the header files of the OpenCV        which is given by   C:\……\opencv\build\include.Here opencv is the folder where our libraries have been installed.            Preferentially install OpenCV library in C Drive.
       
       Configure the transmitter and the receiver x-bee as per the manual provided by the ERTS Lab.
       
       Connect the x-bee to your device and configure it as per this manual.
       
       Position the bot as per the given specification in this manual.
       
       Then take the image as per guidelines mentioned in this manual and save the image in your device. Copy the path of the        file.
       
       Open the project file and build and run the program “ main.cpp “.

Steps to be followed after the program is run are:

       1. A window pops up which the project name and team details are written. It has two buttons to choose whether                   to proceed or to exit. Choose Proceed.      
       2.  Another window pops-up, with two entry boxes for the file path and the  dimension of the arena. 
       3. Please enter the complete file path of the image of the arena( like C:\.......Documents\opencv\image.jpg) in its              corresponding box which was copied earlier.Press the enter key .
       4. In the other box please enter the actual vertical length of the arena (in centimeters) corresponding to the top of           the image of which the image has been provided. Press the enter key and then finally press proceed.
       5.   The image is displayed in a window called named “ipc”;
       6.   While on the window “ipc”, press the enter key.
       7.   The image is processed and the coordinates of the centers of the ball in   real time are displayed on the console              with the top left corner as origin. 
       8.  The path is figured out and the data corresponding to the ideal path is sent to the bot using x-bee.
       9.  The encoded data which is transmitted is displayed on the console as the bot moves.

Setting the COM Port:

Attach the XBee module using the adapter to the computer.
Open Control Panel.
Open Hardware and Sound.
Under Devices and Printers open Device Manager.
Now expand Ports (COM and LPT).
Identify the COM port corresponding to the XBee and open its properties (Right-Click).
Click on the Port Settings tab.
Click on the Advanced button. A second dialog appears.
On this new dialog, select the “COM Port Number” option near the top. Change it to COM '3' and click on OK to close the dialog. You might get a warning message about other programs using the device and whether you wish to continue. Click on Yes.
Click on OK again to close the first dialog.
Close the Device Manager. Note that the Device Manager might still show the old COM port number. If you close it and re-open it, you will then be able to see the new COM port number listed. 
Now the COM port has been set.


Guidelines for taking the image:

 The image has to be a top view of the arena. In cases where an exact top view image is difficult to obtain the image should be taken from as high as possible. Also it should be ensured that the position of the camera must be as close as possible to what it would have been in a top view.  
 The origin of the image is the top left corner of the image.So make sure that the top left corner of the arena touches the top left corner of the image. The origin is defined where the bot is initially kept. 
 It should be ensured that the image is taken in sufficient light such that all the balls have sufficient light falling on them.
It must be ensured that the balls at a certain height so that they can be detected by the sharp sensor.
Also the balls must be separated by about atleast 30 cm (in the arena) and  must not be near the corners of the image.


Placing the Robot:

The initial position of the robot has to be in a specific orientation. It is kept at the origin pointing along the y-axis.The direction of the bot is along the positive y-axis, which here is opposite to the standard direction in a 2D Cartesian coordinate system.
Also it needs to be ensured that the center of the shaft of the robot has to be kept at the point of intersection of the coordinate axes.

















