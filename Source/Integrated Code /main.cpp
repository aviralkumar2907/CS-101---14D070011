
#include <gtk/gtk.h>
#include <stdlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <fstream>
#include <shortestpath.h>
#include <xbee.h>
#include <cstring>
#include <string>
using namespace cv;
using namespace std;

struct file
{
char filepath[100];
char dimension[100];
};

file* data = new file;

//image function copies the address of the file that has been entered by the user in the entry boxes into the data->filepath variable
void image(GtkWidget *widget,  GtkWidget *entry)
{
	strcpy(data->filepath, gtk_entry_get_text(GTK_ENTRY(entry)));

	cout<<data->filepath;



}

//dim function copies the dimension of the image entered the image into the variable data->dimension
void dim(GtkWidget *widget, GtkWidget *entry)
{
	strcpy(data->dimension, gtk_entry_get_text(GTK_ENTRY(entry)));
    cout<<data->dimension;

}

//new1 function is used for creating a new windows, using GtkWidget* and then creates  entry fields for entering the
//path and the dimension of the image. It also has a PROCEED button clicking on which causes the rest of the program, i.e.
//the image processing and shortest path algorithm to start.
void new1()
{
    //GtkWidget is used to create Gtk Widgets, and these widgets can be buttons, windows, entry fields, labels, etc;
    GtkWidget *window;
    GtkWidget *label;
    GtkWidget *proceed;
    GtkWidget *fixed;
    GtkWidget *entryfile;
    GtkWidget *entrydim;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "WELCOME");              //sets the title of the window created
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 550);        //sets the default size for the window
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);  //positions the window t the center of the screen

  fixed= gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(window), fixed);

  label = gtk_label_new("Enter the full path of the image");
  gtk_fixed_put(GTK_FIXED(fixed), label, 75, 150);

  label = gtk_label_new("The Dimension you enter must be vertical!");
  gtk_fixed_put(GTK_FIXED(fixed), label, 75, 250);

//gtk_label_new() returns a Widget which is text written on the window displayed i.e. it creates a new label .
  label = gtk_label_new("Image File:");
  gtk_fixed_put(GTK_FIXED(fixed), label, 50, 100);

  label = gtk_label_new(" Dimension:");
  gtk_fixed_put(GTK_FIXED(fixed), label, 50, 200);

   //gtk_entry_new() is tthe function used to create a new gtk_entry field.
   entryfile = gtk_entry_new();
   gtk_fixed_put(GTK_FIXED(fixed),entryfile, 175, 100);

    //g_signal_connect is used to run the function passed as parameter to the G_CALLBACK function with the parameter
    //entryfile in "activate" mode i.e. by pressing enter click after filling in the text box
   g_signal_connect(G_OBJECT(entryfile),"activate",G_CALLBACK(image),entryfile);

   entrydim = gtk_entry_new(); //gtk_entry_new() is tthe function used to create a new gtk_entry field.
   gtk_fixed_put(GTK_FIXED(fixed),entrydim, 175, 200);

   //g_signal_connect is used to run the function passed as parameter to the G_CALLBACK function i.e. the function dim()
   //with the parameter entrydim in "activate" mode i.e. by pressing enter click after filling in the text box
   g_signal_connect(G_OBJECT(entrydim),"activate",G_CALLBACK(dim),entrydim);

   //gtk_button_with_new_label is a functon returning a Gtkidget* used to create a new button with the label of "PROCEED"
   proceed=gtk_button_new_with_label("PROCEED");
   gtk_fixed_put(GTK_FIXED(fixed), proceed,300,300);

   //clicked type calls the function gtk_main_quit when the PROCEED button is clicked
   g_signal_connect(G_OBJECT(proceed),"clicked",G_CALLBACK(gtk_main_quit),NULL);

   //gtk_main_quit is a function that is used to close all windows and quit the GUI.
   g_signal_connect_swapped(G_OBJECT(window), "destroy",
    G_CALLBACK(gtk_main_quit), NULL);


   gtk_widget_show_all(window);


}

// Points class is used to represent, ccess and modify the points in the program
class Points
{
        public:
            double x; double y;
            Points()
            {
                x=0;
                y=0;
            }

            Points (double x, double y)
            {
                this->x = x;
                this->y = y;
            }

};

/*Function- getBinaryImage(Mat &img)
Input- an openCV 3 channel image matrix
Output- none (just converts a coloured image may to a black and white binary image)
Logic - Using proper threshold values for the image taken, the pixels with Red, Green and blue values corresponding to the balls are converted to white and the remaining are converted to black. In other words, other colours are filtered out from the image for easy processing after this.
*/

void getBinaryImage (Mat &img)
{
    for(int i=0;i<img.rows;i++)
    {
        for(int j=0;j<img.cols;j++)
        {
            if((int)img.data[i*img.cols*3+j*3+2]>190&& (int)img.data[i*img.cols*3+j*3+1]<100 && (int)img.data[i*img.cols*3+j*3+0]<100)
            {
                img.data[i*img.cols*3+j*3+2]=255;
                img.data[i*img.cols*3+j*3+1]=255;
                img.data[i*img.cols*3+j*3]=255;
            }
            else
            {
                img.data[i*img.cols*3+j*3+2]=0;
                img.data[i*img.cols*3+j*3+1]=0;
                img.data[i*img.cols*3+j*3]=0;
            }
        }
    }
}
/*
Function - convertToDigitalisedImage(Mat& img , Mat& img)
Input - 2 Mat passed by reference: Mat A - 3 channeled, Mat B - single channeled
Output - None (Just stores the 3-channel binary image developed earlier in a single channel format
Logic - The three channeled input image is searched for white pixels and in the corresponding pixels in the output single channeled image, white values are stored.

*/
void convertToDigitalisedImage(Mat& ImgMatrixA, Mat& ImgMatrixB)
{
    int i, j;
    unsigned char* input = (unsigned char*) (ImgMatrixA.data);
    unsigned char* output = (unsigned char*) (ImgMatrixB.data);
    for (i=0; i<ImgMatrixA.rows; i++)
    {
        for (j=0; j<ImgMatrixA.cols; j++)
        {

                if (input[ImgMatrixA.cols*i*3+j*3+1] == 255 && input[ImgMatrixA.cols*i*3+j*3+2] == 255 && input[ImgMatrixA.cols*i*3+j*3] ==255)    // image stored in Row Major format
                {
                    output[ImgMatrixB.cols*i+j] = 255;
                }
                else
                {
                    output[ImgMatrixB.cols*i+j] = 0;

                }
        }
    }
}

/*Function - bgr2hsv2bgr(Mat& img)
Input - a Mat 3-channeled image
Output - Outputs an image such that the balls remain in their original colour and remaining background turns red.
Logic - Converts the image to HSV and then converts those pixels which don’t have H values(hue values) between
10 and 40 to H = 0 (red), and let the balls be in their original colour.
*/
void bgr2hsv2bgr(Mat& img)
{
    Vec3b hsv;
    int H;
    for(int i=0;i<img.rows;i++)
    {
        for(int j=0;j<img.cols;j++)
        {
            hsv=img.at<Vec3b>(i,j);
            H=hsv.val[2];
            if(H>10&&H<40)
            {
                continue;
            }
            else
            {
                   img.at<Vec3b>(i,j).val[0]=180;
            }
        }
    }
}

/*Function- void edgeDetect(Mat & )
Inputs- Mat &ImgMatrix , i.e. reference to some image matrix
Outputs- Changes the given matrix into matrix containing only the edges of the balls instead of completely filled balls.
Logic- Checking the value of color of the pixel, after applying Gaussian blur, errosion and dilate to DigitalisedImageMatrix, to identify if it's an edge point. The edge point is defined to have color value 50 - 150. This function drastically reduces the number of points to be accounted for, per ball.
*/

void edgeDetect(Mat &ImgMatrix)
{
    int i, j;
    unsigned char* input = (unsigned char*) (ImgMatrix.data);
    unsigned char* output = (unsigned char*) (ImgMatrix.data);

    for (i=0; i<ImgMatrix.rows; i++)
    {
        for (j=0; j<ImgMatrix.cols; j++)
        {
            if (input[ImgMatrix.cols*i+j]>100 && input[ImgMatrix.cols*i+j]<200)
            {
                output[i*ImgMatrix.cols+j]=255;

            }
            else
            {
                output[i*ImgMatrix.cols+j]=0;

            }
        }
    }

}


/*
 Function  :   goToNextWhite (Mat , int , int , vector<Points>& )
 Inputs    :   Mat ImgMatrix, int i, int j, vector<Points>& AllThePoints
 Outputs   :   Stores all the points corresponding to a ball in vector AllThePoints
 Logic     :   A ball is a set of continuous white pixels. goToNextWhite(...) is a recursive function
               which calls itself with a neighbouring white pixel as new base point. It turns
               the current white pixel int black so that it is not stored again. Recursion stops
               when no more white pixels are left in the immediate neighbourhood. Finally,
               when whole ball is covered, all the points are stored in AllThePoints
               and control is returned.
 Example   :   goToNextWhite (ImgMatrix, p, q, AllThePoints);
*/

void goToNextWhite (Mat ImgMatrix, int i, int j, vector<Points>& AllThePoints)
{
    if (i>=ImgMatrix.rows || i<0 || j>=ImgMatrix.cols || j<0) return;

	else
    {
        Points Temp(i,j);
        AllThePoints.push_back(Temp);
        unsigned char* output = (unsigned char*) (ImgMatrix.data);
        output[ImgMatrix.cols*i + j] = 0;                      // turn the current point to black(COLOR==0) so that we don't go back to this pixel again.
        for (int p = i-1; p<=i+1; p++)
        {
            for (int q=j-1; q<=j+1; q++)
            {
                if (p==i && q==j) {}
                else if (p>=0 && q>=0 && p<ImgMatrix.rows && q<ImgMatrix.cols  && output[ImgMatrix.cols*p + q] == 255)     // finding the next WHITE pixel IN RANGE and neighbour to given pixel
                {
                    goToNextWhite(ImgMatrix, p, q, AllThePoints);
                }
            }
        }
	}
}


/*
 Function  :  void findCentre(vector<Points>& , vector<Points>& )
 Inputs    :  vector<Points>& AllThePoints, vector<Points>& TheCentres
 Outputs   :  Stores centre of the ball represented by the set of points
              in AllThePoints vector, into TheCentres vector
 Logic     :  The Centre of the ball, i.e. colllection of points can be calculated
              by taking average of the x and y coordinates of all the points on the ball.
 Example   :  void findCentre(Allpoints, Centres);

*/
void findCentre(vector<Points>& AllThePoints, vector<Points>& TheCentres, double scale)
{
    double sumOfX =0, sumOfY = 0;
    long double totalCount=0;      // used long double, not int so as to increase the range of n, to support large number of points

    vector<Points>::iterator it;
    for (it = AllThePoints.begin(); it<AllThePoints.end(); ++it)
    {
        sumOfX = sumOfX + (10E-4)*(it->x);   // scaling down the x coordinate by 1/1000 so as to keep sumOfX within limits
        sumOfY = sumOfY + (10E-4)*(it->y);   // scaling down the y coordinate by 1/1000 so as to keep sumOfY within limits
        totalCount++;
    }

    double centreX = sumOfX/totalCount;
    double centreY = sumOfY/totalCount;

    //cout<<"\nPrinting Center:"<<centreX<<" , "<<centreY;

    Points Temp;
    Temp.x = 1000*centreX*scale;
    Temp.y = 1000*centreY*scale;            //scaling up the x and y coordinates to bring back to original scale
    TheCentres.push_back(Temp);

    AllThePoints.clear();
}


/*
 Function  :  void find_a_ball (Mat , vector<Points>& , vector<Points>& )
 Inputs    :  Mat ImgMatrix, vector<Points>& AllThePoints, vector<Points>& TheCentres
 Outputs   :  Locates a complete ball and finds its centre. Does the same for all the balls
              in the arena and stores their centres in vector TheCentres.
 Logic     :  Calls function goToNextWhite(Mat, int, int, vector) to find a ball in the arena,
              store all the points corresponding points for the ball in vector AllThePoints
              and calls function findCentre(vector &, vector &) to store the centres
              corresponding to various balls in vector TheCentres.
 Example   :  void find_a_ball (DigitalisedImageMatrix, AllPoints, Centres);

*/
void find_a_ball (Mat ImgMatrix, vector<Points>& AllThePoints, vector<Points>& TheCentres, double ratios)
{
	int i, j;
	AllThePoints.clear();
    for (i=0; i<ImgMatrix.rows; i++)
	{
		for (j=0; j<ImgMatrix.cols; j++)
		{
            unsigned char* output = (unsigned char*) (ImgMatrix.data);
			if (output[ImgMatrix.cols*i+j] == 255)                        // Goes in if a white-pixel(COLOR==255) of the ball is located
			{
                goToNextWhite(ImgMatrix, i, j, AllThePoints);
                findCentre(AllThePoints, TheCentres, ratios);      // function call to findCentre(...), after AllThePoints has been allocated the required pixels' data
                AllThePoints.clear();
            }
		}
	}
}

void storevalues(vector<Points>& Centres)
{
    ofstream ofile;
    ofile.open("Points.dat",ios::binary|ios::out);
    vector<Points>::iterator it;         // using iterator to access points stored in vector.
    Node temp;
    temp.x =0;
    temp.y =0;
    ofile.write ((char*)&temp, sizeof(temp));
    //cout<<"\n\n\nn\n\nDisplaying center coordinates:\n";
    for (it = Centres.begin(); it<Centres.end(); ++it)
        {
            temp.x = it->x;
            temp.y = it->y;
			temp.nextNode=NULL;
            ofile.write ((char*)&temp, sizeof(temp));
        }
}

int main(int argc, char* argv[])
{
     //GtkWidget is used to create Gtk Widgets, and these widgets can be buttons, windows, entry fields, labels, etc;
    GtkWidget *window;
  GtkWidget *label;
  GtkWidget *fixed;
  GtkWidget *moveon;
  //GtkWidget *decrypt;
  GtkWidget *quit;

  gtk_init(&argc, &argv); //initiaises the gtk GUI in the main()

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL); //gtk_window_new() is used to return a Widget which is of window type
  gtk_window_set_title(GTK_WINDOW(window), "OPTIMISED BALL COLLECTOR"); // sets title for the window created
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 550); // sets the default size of the window to be created
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER); // places the window created on the center of the screen



   fixed= gtk_fixed_new();  //  a container which allows you to position widgets at fixed coordinates
  gtk_container_add(GTK_CONTAINER(window), fixed);

    //gtk_label_new() returns a Widget which is text written on the window displayed i.e. it creates a new label .
  label = gtk_label_new("OPTIMISED BALL COLLECTOR");
  gtk_fixed_put(GTK_FIXED(fixed), label, 110, 100); //this function is used to place the label at the position specified

  label=gtk_label_new("CS101 - SPRING 2015");
  gtk_fixed_put(GTK_FIXED(fixed),label,140,150);

  label=gtk_label_new("Sudeep Salgia");
  gtk_fixed_put(GTK_FIXED(fixed),label,100,175);

  label=gtk_label_new("Aviral Kumar");
  gtk_fixed_put(GTK_FIXED(fixed),label,100,200);

  label=gtk_label_new("Sohum Dhar");
  gtk_fixed_put(GTK_FIXED(fixed),label,100,225);

  label=gtk_label_new("MuthamizhSelvan S.");
  gtk_fixed_put(GTK_FIXED(fixed),label,100,250);

  label=gtk_label_new("14D070011");
  gtk_fixed_put(GTK_FIXED(fixed),label,240,175);

  label=gtk_label_new("140070031");
  gtk_fixed_put(GTK_FIXED(fixed),label,240,200);

  label=gtk_label_new("140070001");
  gtk_fixed_put(GTK_FIXED(fixed),label,240,225);

   label=gtk_label_new("140110091");
  gtk_fixed_put(GTK_FIXED(fixed),label,240,250);

  label=gtk_label_new("TA:");
  gtk_fixed_put(GTK_FIXED(fixed),label,100,295);

  label=gtk_label_new("Chinmay Kulkarni");
  gtk_fixed_put(GTK_FIXED(fixed),label,240,295);

   //gtk_button_with_new_label is a functon returning a Gtkidget* used to create a new button with the label of "PROCEED"
   moveon=gtk_button_new_with_label("Click here to Proceed");
  gtk_fixed_put(GTK_FIXED(fixed), moveon,125,400);



  g_signal_connect(G_OBJECT(moveon),"clicked",G_CALLBACK(new1),NULL);



  g_signal_connect_swapped(G_OBJECT(window), "destroy",
      G_CALLBACK(gtk_main_quit), NULL);




   quit=gtk_button_new_with_label("Exit");
   gtk_fixed_put(GTK_FIXED(fixed), quit,175,450);

   g_signal_connect(G_OBJECT(quit), "clicked",
      G_CALLBACK(gtk_main_quit), G_OBJECT(window));

   g_signal_connect_swapped(G_OBJECT(window), "destroy",
      G_CALLBACK(gtk_main_quit), NULL);


    gtk_widget_show_all(window); // show_all() is used o display the window created on the screen

    gtk_main();

    //cout<<"----------OPTIMISED BALL COLLECTOR-----------";
    string str = data->filepath;
    //string::size_type sz;
    int actual1 = atoi(data->dimension);
    double actual = (double) actual1;
    Mat img;
    img=imread(str,1);
    while(img.cols>1024)
    {
        resize(img, img, Size (img.cols/3,img.rows/3));
    }
    namedWindow("ipc",CV_WINDOW_AUTOSIZE);
    imshow("ipc",img);
    waitKey(0);
    cvtColor(img,img,CV_BGR2HSV);
    bgr2hsv2bgr(img);
    cvtColor(img,img,CV_HSV2BGR,0);
    getBinaryImage(img);
    GaussianBlur(img,img,Size(5,5),0,0);
    int erosion_size =5;
    int erosion_type = MORPH_ELLIPSE;
    Mat element = getStructuringElement( erosion_type,Size( 2*erosion_size + 1, 2*erosion_size+1 ),Point( erosion_size, erosion_size ) );
    dilate(img,img,element);
    erode( img, img, element );
    Mat singleimg = Mat::zeros(Size(img.cols, img.rows),CV_8UC1);
    convertToDigitalisedImage(img,singleimg);
    GaussianBlur(singleimg, singleimg, Size(9,9), 0,0);
    edgeDetect(singleimg);
    //double actual;
   // cout<<endl<<"Enter actual Length of arena:  ";
    //cin>>actual;
    double ratios = actual/(img.rows);
        //Declaring vectors to store all points for each ball, one by one, and all the centres of balls in the arena
    vector<Points> AllPoints;
    vector<Points> Centre;
        //function call to store the required centres of all the balls in the arena
    find_a_ball(singleimg, AllPoints, Centre, ratios);
    vector<Points>::iterator it;         // using iterator to access points stored in vector.
    cout<<"\n\n\n\n\n\nDisplaying center coordinates:\n";
    for (it = Centre.begin(); it<Centre.end(); ++it)
        cout<<"\nCenter Coordinates: "<<it->x<<" "<<it->y<<endl;
    waitKey(0);
    storevalues(Centre);



    system("cls");
    cout<<"----------OPTIMISED BALL COLLECTOR-----------";


    //shortest path
    Path BotPath;
    BotPath.optimalSolutionWithSwap();

    BotVectorPath BotVecPath(BotPath);

    int count = BotVecPath.getBranchCount();
    ofstream ofile("Instr.dat", ios::binary|ios::trunc);

    cout<<"\nPrinting the Optimal Solution as a set of Vectors:\n";
    for(int i=0; i<count;i++)
    {
        BranchVector BranchV=BotVecPath.getBranch(i);
        cout<<"\n"<<BranchV.radius<<","<<BranchV.theta;
        ofile.write((char*)&BranchV, sizeof(BranchVector));  // writing into output instruction file
    }
    ofile.close();


    system("cls");
    cout<<"----------OPTIMISED BALL COLLECTOR-----------";


    //xbee
     int signbit;    // To store the data containing information regarding the type of data and its sign
    InstructionVector Temp;
    ifstream ifile("Instr.dat", ios::in);
    unsigned char datahigh;
    unsigned char datalow;

    if (!ifile) return 0;           // If the file is empty exit.
    while (!ifile.eof())
    {

          ifile.read((char*)&Temp, sizeof(Temp));

          if (ifile.eof())  break;

         //The following code snippet is to send angle
         // To store the data containing information regarding sign of data, corresponding to the last 2 bits in datalow
           if (Temp.angle >=0) signbit = 0;
           else if (Temp.angle <0) signbit =2;

           int Angle = (int) (abs(Temp.angle));
           int binaryAngle=convert_to_binary(Angle);

           datalow=(unsigned char) convert_to_int(binaryAngle%1000000)*4 + signbit ;  // storing the last 6 bits of theta and b0=1, b1=1 for forward , positive  value respectively
           datahigh=(unsigned char) convert_to_int(binaryAngle/1000000);     // storing the bits after the first 6 bits

           if (writebyte(&datalow)) cout<<"\nData Low Sent\n";
           cout<<"\n Data Received::"<<readByte();   //Recieving from XBee to synchronize transmission.
           if (writebyte(&datahigh)) cout<<"\nData High Sent\n";
           cout<<"\n Data Received::"<<readByte();   //Recieving from XBee to synchronize transmission.

              //The following code snippet is to send distance
           int Distance=(int)(Temp.distance*10);    //*10 for converting into mm
           int binaryDistance=convert_to_binary(Distance);
           datalow=(unsigned char) convert_to_int(binaryDistance%1000000)*4 + 3 ;  // storing the last 6 bits of distance and b0=1, b1=1 for forward , positive  value respectively
           datahigh=(unsigned char) convert_to_int(binaryDistance/1000000);     // storing the bits after the first 6 bits into datahigh

            if (writebyte(&datalow)) cout<<"\nData Low Sent\n";
           cout<<"\n Data Received::"<<readByte();       //Recieving from XBee to synchronize transmission.
           if (writebyte(&datahigh)) cout<<"\nData High Sent\n";
           cout<<"\n Data Received::"<<readByte();    //Recieving from XBee to synchronize transmission.


    }
    CloseHandle(hPort);

    return 0;

}






