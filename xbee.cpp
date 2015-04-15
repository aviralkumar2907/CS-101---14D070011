/*Windows.h is a header file available in MinGW compiler of CodeBlocks IDE that can be used for accessing and manipulating the Ports, Menus, Icons etc. through a C++ program*/

#include <windows.h>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

 //Defining a structure to hold the polar coordinates in two dimensions of a point 
struct Point
{
    double r;
    double theta;
};

/*HANDLE -> In windows programming, the windows maintains a table of all icons, ports, menus, etc, 
and each file has a unique identifier through which it can be accessed. Windows is given the handle
and it returns the port we want.*/
 
HANDLE hPort = CreateFile("COM3", GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

/*DCB ->A structure that defines the control setting for a serial communications device. */
DCB dcb;


/* DWORD - DWORD is datatype used to represent 32-bit quantities in Windows */

/*
Function - writebyte();
Input    - unsigned char* , (pointer to an unsigned char data)
Output   - bool - TRUE or FALSE
Logic    - This function is used for writing a character (only 1 byte) on to the COM port 
         (given by Handle hPort. First of all it checks whether the Port can be opened or not 
         by checking GetCommState(),if FALSE, function returns, else the Port Baud rate, bytesize, parity ,
         etc are specified and then data is written on to the port using writeFIle(),
         which returns TRUE if data is written successfully, else returns FALSE.
Example -  char data = 0x05;  bool x =writebyte(&data);
*/

   
bool writebyte (unsigned char* data)
{
    DWORD byteswritten;
    if (!GetCommState(hPort, &dcb))
    {
        cout<<"\n Serial port can't be opened";
        return false;
    }

    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    if (!SetCommState(hPort, &dcb))
    return false;

    bool retVal = WriteFile(hPort, data, 1, &byteswritten, NULL);
    return retVal;
}

/* BYTE -> BYTE is a datatype used to represent 8-bit data in Windows Programming*/
/*
Function  - readByte()
Input     - None
Output    - ASCII value of the character(byte) read at the port
Logic     - This function reads a BYTE (8 bit) data from the port and then first checks whether 
            the COM PORT is accessible or not. If it is, then SetCommMask() is called. 
SetCommMask() - Specifies a set of events to be monitored for a communications device.
            Then the function waits until the COM event is finished, 
            i.e. the entire COM file is received. Then this value is read and stored in BYTE byte, 
            and finally an int variable of the type int with ASCII value of BYTE character is returned.*/  
  
int readByte()
{
    int Val;
    BYTE byte;
    DWORD dwBytesTransferred;
    DWORD dwCommModemStatus;
    if(!GetCommState(hPort, &dcb))
        return 0;

    SetCommMask(hPort, EV_RXCHAR|EV_ERR);
    WaitCommEvent(hPort, &dwCommModemStatus, 0);
    if (dwCommModemStatus& EV_RXCHAR)
        ReadFile(hPort, &byte, 1, &dwBytesTransferred, 0);
    Val = byte;
    return Val;
}

/*
Function : int convert_to_binary(int )
Inputs :  int x , i.e.  number whose binary equivalent is to be calculated
Outputs : binary equivalent of input - decimal number
Logic : binary equivalent is given by the remainders obtained after successive division by 2
Example : long int binaryDistance= convert_to_binary(data);
*/
int convert_to_binary(int x)
{
    int num=0, i=1;
    while (x!=0)
    {
        int dig = x%2;
        num = num + dig*i;
        x/=2;
        i=i*10;
    }
    return num;
}

/*
Function : int convert_to_int(long int )
Inputs :  long int x , i.e.  number whose decimal equivalent is to be calculated
Outputs : decimal equivalent of input - binary number
Logic : decimal equivalent is given by the series D= b0 + b1*2 + b2*4 +b3*8 + ...
Example : int distance= convert_to_int(data);
*/

int convert_to_int(int x)
{
    int result=0, powerOf2=1;
    while (x!=0)
    {
        result+=(x%10)*powerOf2;
        powerOf2*=2;
        x/=10;
    }
    return result;
}


int main()
{
    char r;
    Point Temp;
    int i;
    ifstream ifile("Instr.dat", ios::in);
    unsigned char datahigh;
    unsigned char datalow;
    int b=0;
    
    if (!ifile) return 0;           // If the file is empty exit.
    while (!ifile.eof())
    {

          ifile.read((char*)&Temp, sizeof(Temp));
       
          if (ifile.eof())  break;
          //now this sends angle
          int signbit;
          if (Temp.theta >=0) signbit = 0;
          else if (Temp.theta <0) signbit =2;

          int angle = (int) (abs(Temp.theta));
          int binaryAngle=convert_to_binary(angle);
          datalow=(unsigned char) convert_to_int(binaryAngle%1000000)*4 + signbit ;  // storing the last 6 bits of theta and b0=1, b1=1 for forward ,
                                                                                     //positive  value respectively
          datahigh=(unsigned char) convert_to_int(binaryAngle/1000000);              // storing the bits after the first 6 bits
        
          if (writebyte(&datalow)) cout<<"\nData Low Sent\n";
             cout<<"\n Data Received::"<<readByte();
          if (writebyte(&datahigh)) cout<<"\nData High Sent\n";
             cout<<"\n Data Received::"<<readByte();


          //sends distance
          int distance=(int)(Temp.r*10);    //converting into mm
          int binaryDistance=convert_to_binary(distance);
          datalow=(unsigned char) convert_to_int(binaryDistance%1000000)*4 + 3 ;  // storing the last 6 bits of distance and b0=1, b1=1 for forward , 
                                                                                  //positive  value respectively
          datahigh=(unsigned char) convert_to_int(binaryDistance/1000000);        // storing the bits after the first 6 bits
        
          if (writebyte(&datalow)) cout<<"\nData Low Sent\n";
             cout<<"\n Data Received::"<<readByte();                              // Recieving from XBee to synchronize transmission.
          if (writebyte(&datahigh)) cout<<"\nData High Sent\n";
             cout<<"\n Data Received::"<<readByte();                              // Recieving from XBee to synchronize transmission.
    
    }
    CloseHandle(hPort);
}



