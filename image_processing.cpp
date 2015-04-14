
/* Program - Image Processing to identify balls and find out the coordinates of the centres of the balls

    Header Files Used:  1.Iostream
                        2.Opencv Library
                        3.Vector
                        4.fstream
    Uses of the Header Files:
                        1.iostream -
                            ->iostream is used to take input such as the length of the arena and also for the output.
                            ->All the input and output functions are handled with iostream.

                        2.opencv-
                            ->OpenCV (Open Source Computer Vision) is a library of programming functions mainly aimed at real-time computer vision.
                            ->All the Image processing in the code is taken care by opencv library function
                                and pre defined data tyes of opencv such as Mat,etc
                            ->In Built functions and Data types of opencv library used in this code:

                                KeyWords

                                    1.Mat-
                                        Mat is basically a class with two data parts: the matrix header (containing information
                                        such as the size of the matrix, the method used for storing, at which address is the matrix stored, and so on)
                                        and a pointer to the matrix containing the pixel values (taking any dimensionality depending on the method chosen for storing) .
                                        The matrix header size is constant, however the size of the matrix itself may vary from image
                                        to image and usually is larger by orders of magnitude.

                                    2.CV_WINDOW_AUTOSIZE-
                                        This keyword when used makes sure that the window created to display an image would be of the size of the image to be displayed.

                                    3.CV_BGR2HSV-
                                        This Keyword is used to convert the image from RGB format to HSV format.

                                    4.MORPH_ELLLIPSE-
                                        It denotes that the element to be created is ellipse. It is used in getStructuringelement function.

                                    5.CV_8UC1-
                                        It is used to define a image with just one channel.

                                Functions:

                                    1.imread-
                                        The Function is used to read an image into a Mat data type .The Image must be present in the directory where the code is present.

                                    2.resize-
                                        This Funtion is used resize the image read into the Mat data type.It is a must if the image goes out of the screen .

                                    3.cvtColor-
                                        This function is used for image transformations.For Example the image has been converted from RGB format to HSV format
                                        for efficient work of the code.

                                    4.namedWindow-
                                        This Function is used to create a window for displaying the image.

                                    5.imshow-
                                        This Function is used to display the image in the created Display Window.

                                    6.waitKey-
                                        It waits for the press of a key.until the time specified in the parameter and if zero is provided the program waits
                                        forever for the press of a key.

                                    7.GaussianBlue-
                                        This is an in-built funtion used to create a transition in those area where the pixels values change abruptly.
                                        Thus it smooths the sharp change in pixel values.

                                    8.getStructuringelement-
                                        This function is used to create a Mat element that would be used in the functions erode and dilate.

                                    9.Erode-
                                        This function is used to expand the pixels around its neighbourhood.

                                    10.Dilate-
                                        This Function is used to reduce the size of the certain areas of pixels according to the structuring element .


    Logic -  OpenCV library has been used for this code. In order to find out the coordinates of the balls,
    		we first filter out the components (pixel values) of the image that are not of the same colour as that
    		of the balls by converting the image to HSV, then converting it to binary and finally applying blurs to
    		modify this filtered image. After this an edgedetect() function detects all edge points and 
    		the find_a_ball() function stores the coordinates of the edge points in a vector which are used for
    		calculating the centers of the ball. Finally this data is written into a file, which is further used
    		for the algorithm code */

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <fstream>
using namespace cv;
using namespace std;


//Structure -> Node - Structure to represent a point in 2D
//Used while writing the point coordinates in a file

struct Node {
        double x;
        double y;
};

//Class-> Points - To represent the Points in the program 
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
/*
Function- getBinaryImage() 
Input- an openCV 3 channel image matrix 
Output-  none (just converts a coloured image may to a black and white binary image) 
Logic -  using proper threshold values for the image taken,  the pixels with Red,  Green  and blue values 
	 corresponding to the balls are converted to white and the remaining are converted to black.  In other 
	 words,  other colours are  filtered out from the image for easy processing after this.  */

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

Function - convertToDigitalisedImage(Mat&, Mat&)

Input - 2 Mat passed by reference: Mat A - 3 channeled, Mat B - single channeled

Output - None (Just stores the 3-channel binary image developed earlier in a single channel format         

Logic - The three channeled input image is searched for white pixels and in the corresponding pixels in the output
	single channeled image, white values are stored. */


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

/*

Function - bgr2hsv2bgr(Mat&)

Input - a Mat 3-channeled image

Output - Outputs an image such that the balls remain in their original colour and remaining background turns red. 
Logic - Converts the image to HSV and then converts those pixels which don’t have H values(hue values) between 
	10 and 40 to H = 0 (red), and let the balls be in their original colour. */  

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

/*

 Function  :  void edgeDetect(Mat & )

 Inputs    :  Mat &ImgMatrix , i.e. reference to some image matrix

 Outputs   :  Changes the given matrix into matrix containing only the edges of the balls

              instead of completely filled balls.

 Logic     :  Checking the value of color of the pixel, after applying Gaussian blur, errosion and dilate

              to DigitalisedImageMatrix, to identify if it's an edge point. The edge point is defined to have

              color value 50 - 150. This function drastically reduces the number of points to be accounted for, per ball.
 Example   :  edgeDetect(ImageMatrix); */

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

/*

Function - storevalues(vector<Points> &)

Input - a vector of Points type

Output - Creates a file named Points.dat and writes the coordinates of the centres of the balls into that file 

Logic - Uses a temporary instance of the struct Node and a Points iterator to access the vector that stores the 
	Centre coordinates and then assigns the x and y of a record to the Temp Node and writes it in a file called Points.dat */


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
            ofile.write ((char*)&temp, sizeof(temp));
        }
}

int main()
{

    Mat img;
    img=imread("image.jpg",1);  //image.jpg is the name of the image taken - you ca write your own image name
    while(img.cols>1024)  //bringing the image to the screen size (maximum size after this loop is the screen size)
    {
        resize(img, img, Size (img.cols/2,img.rows/2));
    }
    namedWindow("ipc",CV_WINDOW_AUTOSIZE);
    imshow("ipc",img);
    waitKey(0);		//Press enter here
    
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
    
    Mat singleimg = Mat::zeros(Size(img.cols, img.rows),CV_8UC1);  /*singleimg - name for a single channel image*/
    convertToDigitalisedImage(img,singleimg);
    GaussianBlur(singleimg, singleimg, Size(9,9), 0,0);
    edgeDetect(singleimg);
    
    double actual;	//this is the actual dimension of the arena which is completely present in the image taken
    cout<<endl<<"Enter actual Length of arena:  ";
    cin>>actual;
    
    double ratios = actual/(img.rows);
    
    //Declaring vectors to store all points for each ball, one by one, and all the centres of balls in the arena
    vector<Points> AllPoints;
    vector<Points> Centre;
    
    //function call to store the required centres of all the balls in the arena
    find_a_ball(singleimg, AllPoints, Centre, ratios);

    vector<Points>::iterator it;         // using iterator to access points stored in vector.
    
    cout<<"\n\n\n\n\n\nDisplaying center coordinates:\n";
    for (it = Centre.begin(); it<Centre.end(); ++it)//To Display the real time coordinatesof all the balls.
        cout<<"\nCenter Coordinates: "<<it->x<<" "<<it->y<<endl;
    
    waitKey(0);	//Press enter here
    storevalues(Centre);
    return 0;

}
