 #include<iostream>
#include<fstream>
#include<cmath>

namespace constants
{
  const double PI=3.14159265;
  const int NMax=100;   //global variable for assumed max number of balls
  const int NoOfCycles=5;   // for optimal no. of cycles for optimalSolutionWithSwap()
}

using namespace constants;
using namespace std;

//Node in x, y because Cartesian coordinates allow easy swapping of nodes in path
struct Node                         
{
    double x, y;
    Node()
    {
        x=0.0;
        y=0.0;
    }

};

//Structure- Branch Vector to store the coordinates of a point in basic 2D polar coordinates
struct BranchVector
{
    double radius;
    double theta;               //General variables in the Polar coordinates system
};


/*Function : absolute(double)
  Input    : double x, i.e. number
  Output   : |x|, i.e. absolute value of x
  Logic    : if x>0 return x
             if x<0 return -x, as -x will be positive magnitude of x
  Example  : x=absolute(-5.2);
*/

double absolute( double x)
{   if(x<0)
     return -1*x;
    else
     return x;
}

// Class ->Path for storing and maintaining an array of Nodes i.e. maintaining the path

class Path
{
    Node pathNode[NMax];

    int ballCount;
    double minPathLength;
   public:
    double branchLength(int nodePos1, int nodePos2);  //distance between nodePos1 and nodePos2
    void sortForGreedyAlgorithm();

    Path()                                   // Constructor to initialise path and set it with 'Greedy Salesman' Algorithm
    {
        int i=0;

        ifstream ifile("Points.dat", ios::binary);
        while(!ifile.eof())
        {
           if(ifile.eof()) break;
           ifile.read((char*)&pathNode[i++], sizeof(Node));
        }
        ifile.close();
        ballCount=i-1;    //-1 as there is some internal error while copying Node points from file as last entry/point seems to be repeated

        sortForGreedyAlgorithm();                     //Function Call to sort for approximately shortest path using 'Greedy Salesman' Algorithm

        minPathLength=0;
        for(i=0; i<ballCount-1; i++)
        {
            minPathLength+=branchLength(i,i+1);       //Additive Accumulation idiom to calculate minPathLength obtained after sorting
        }

    }


    /*
    Function  : swap2branchends(int, int)
    Input     : int i1, int i2,  i.e. indices of the fixed end points
                where corresponding next, pervious end points are to be swapped
    Output    : pathNode[] representing a path with the two specified end points(i1+1, i2-1)
                swapped in the sequence, keeping the corresponding i1 and i2 branch end points same
    Logic     : Swapping nodes (i1+1) and (i2+1) by altering the path from i1, i1+1 to i2-1, i2
                to i1, i2-1 to i1+1 , i2 by simple array operations.
                Also i1<i2 is necessary for the loop and thus the code snippet for the same is implemented.
                (given with comments)

    Example   : BotPath.swap2branchends(0,3);
    */
    
void swap2branchends(int i1, int i2)      //function to swap two branch endpoints and set the path accordingly
    {
       if(i1>i2)
       {   int temp=i1;
           i1=i2;
           i2=temp;
       }                        //To ensure the initial condition for the loop(i1<i2) are satisfied

       for(int i=i1+1 , j=i2-1; j-i>=1; i++, j-- )
       {
           Node temp=pathNode[i];
           pathNode[i]=pathNode[j];
           pathNode[j]=temp;
       }                         //swapping the 2 branches each following the nodes i1 and i2
    }

    bool checkPathLength()
    {  double pathLength=0.0;
       for(int i=0; i<ballCount-1; i++)
       {
           pathLength+=branchLength(i,i+1);
       }
       return (pathLength<minPathLength);
    }


    /*
    Function  : optimalSolutionWithSwap()
    Input     : void
    Output    : pathNode[] is set to a path close to the optimal solution for given set of points
    Logic     : Trying out all possible swaps between end points of the branches (say with indices i1 and i2)
                keeping i1-1 and i2-1 fixed. Checking if new path length is lesser
                and if so, setting this path as the new solution.
                With many iterations, we get the optimal solution.

    Example   : BotPath.optimalSolutionWithSwap();
    */
    void optimalSolutionWithSwap()    //improvising the path after 'Greedy Salesman' Algorithm by swapping end points of two branches
    {   for(int n=0; n<NoOfCycles; n++)    // After repeated iterations we shall get the optimal solution
        for(int i=0; i<ballCount-3; i++)
        {
           for(int j=i+3; j<ballCount; j++)
           {
                   if(branchLength(i,i+1)+branchLength(j-1,j)>branchLength(i,j-1)+branchLength(i+1,j))
                   {
                       minPathLength=minPathLength-(branchLength(i,i+1)+branchLength(j,j-1))+(branchLength(i,j-1)+branchLength(i+1,j));
                       swap2branchends(i,j);
                   }

           }
        }

        //use swap2branches to find a better, more accurate path
        //printing the path as set of consecutive points on it:
        cout<<"\nPrinting the optimal solution in Cartesian System:\n"
        for(int i=0; i<ballCount; i++)
        {
            cout<<"\n("<<pathNode[i].x<<" "<<pathNode[i].y<<")";
        }

    }


    /*
    Function  : getPathNode(int)
    Input     : int i, i.e. index of the Node
    Output    : Node on the pathNode[] with index i
    Logic     : It's an accessor function.
                If i is in range i.e. 0 to ballCount-1 return Node[i]
                else return (0,0), i.e. error point
    Example   : Node N= BotPath.getPathNode(0);

    */
    Node getPathNode(int i)
    {
        if(i>=ballCount||i<0)
        {
          Node N;
          return (N);     // since i is out of range return (0,0) i.e. default initialised value
        }
        else
          return pathNode[i];   // Else return the point/node corresponding to index i as asked for
    }
    /*
    Function  : getBallCount()
    Input     : void
    Output    : ballCount ,i.e. total number of balls in the arena
    Logic     : It's a simple accessor function.
                Returns ballCount variable defined in the class
    Example   : int N=BotPath.getBallCount();

    */
    int getBallCount()     //accessor function returning number of Balls
    {
        return ballCount;
    }
};

/*
  Function  : branchLength(int , int )
  Input     : int nodePos1, int nodePos2, i.e. indices of nodes between which distance is to be calculated
  Output    : (double) distance between the two nodes specified by the indices, on pathNode[]
  Logic     : using distance between two points (x1,y1), (x2,y2) i.e. distance in Cartesian coordinates
              i.e. square root of ( square of difference of(x) + square of difference of(y) )
  Example   : double d0= BotPath.branchLength(0,1);
*/

double Path::branchLength(int nodePos1, int nodePos2)  //distance between nodePos1 and nodePos2
    {
        return sqrt(pow(pathNode[nodePos1].x-pathNode[nodePos2].x,2)+pow(pathNode[nodePos1].y-pathNode[nodePos2].y,2));
    }

/*
 Function  : sortForGreedyAlgorithm()
 Input     : void
 Output    : pathNode[] set to a path decided by the Greedy Salesman Algorithm
             i.e., the Salesman travels to the next nearest town first
 Logic     : Use 'Selection sorting' algorithm to implement 'Greedy Salesman Algorithm'
             based on distances between nodes. (Given fixed initial point.)
 Example   : BotPath.sortForGreedyAlgorithm();
*/

void Path::sortForGreedyAlgorithm()                 //Function applying Greedy Salesman Algorithm using sorting Algorithm
    {   int i, j;
        for(i=0; i < ballCount-2; i++)
        {
           int pos=i+1;
           double minDistance=branchLength(i,i+1);

           for(j=i+1; j<ballCount; j++)
           {
              if(branchLength(i,j)<minDistance)
              {
                  pos=j;
                  minDistance=branchLength(i,j);
              }
           }

           if(j!=i+1)                            //to avoid unnecessary execution
           {   Node Temp=pathNode[i+1];
               pathNode[i+1]=pathNode[pos];
               pathNode[pos]=Temp;
           }

        }

    }


class BotVectorPath
{  
     BranchVector pathVector[NMax-1] ;
    int branchCount;

   public:

    BotVectorPath( Path P )
    {
        Node Node1, Node2;

        int i=0;
        branchCount=P.getBallCount()-1;

        for(i=0; i< branchCount ; i++)
        {   Node1=P.getPathNode(i);
            Node2=P.getPathNode(i+1);                       // Node1 and 2 are the initial and the final points of the vector respectively
            pathVector[i].radius=P.branchLength(i,i+1);    //calculating the length of the branch
            if(Node2.x!=Node1.x)
            {
               pathVector[i].theta=atan((Node2.y-Node1.y)/(Node2.x - Node1.x))*180/PI;                  // calculating the angle in 0 to 360 degrees
               if(Node2.x-Node1.x<0)
                 pathVector[i].theta=(180-absolute(pathVector[i].theta))*(((Node2.y-Node1.y)>0)?1:-1);
               if(pathVector[i].theta<0)
                 pathVector[i].theta=360+pathVector[i].theta;
            }
            else
            {
              if(Node2.y>Node1.y)
                pathVector[i].theta=90;
              else
                pathVector[i].theta=270;
            }
            //printing r, theta values in reverse order
            cout<< "\n"<<pathVector[i].radius<<" units,"<<pathVector[i].theta <<"degrees";

        }

        for(i=branchCount-1; i>0; i--)
        {
             pathVector[i].theta=pathVector[i].theta-pathVector[i-1].theta;

             if(pathVector[i].theta>180.0)
                 pathVector[i].theta=360.00-pathVector[i].theta;        // to optimise turning

             if(pathVector[i].theta<-180.0)
                 pathVector[i].theta=360.00+ pathVector[i].theta;        // to optimise turning
        }

     }

     /*
      Function  : getBranch(int)
      Input     : int pos , i.e. index of the branch you require
      Output    : pathVector[pos], i.e. the BranchVector variable
                  corresponding to index pos on pathVector[]
      Logic     : It's an accessor function returning an element of an array pathVector[]
      Example   : BranchVector B=BotVecPath.getBranch(0);
     */
    
     BranchVector& getBranch(int pos)
     {
          return pathVector[pos];
     }


     double branchRadius(int i)
     {
          return pathVector[i].radius;
     }

     /*
      Function  : getBranchCount()
      Input     : void
      Output    : branchCount, i.e. total number of branch vectors in the optimal path found
      Logic     : It's an accessor function returning branchCount i.e. a member of the class.
      Example   : int N=BotVecPath.getBranchCount();
     */

     int getBranchCount()
     {
           return branchCount;
     }

};


int main()
{   Path BotPath;
    BotPath.optimalSolutionWithSwap();

    BotVectorPath BotVecPath(BotPath);

    int count = BotVecPath.getBranchCount();
    ofstream ofile("Instr.dat", ios::binary|ios::trunc);
    
    cout<<"\nPrinting the Optimal Solution as a set of Vectors:\n"
    for(int i=0; i<count;i++)
    {  
        BranchVector BranchV=BotVecPath.getBranch(i);
        cout<<"\n"<<BranchV.radius<<","<<BranchV.theta;
        ofile.write((char*)&BranchV, sizeof(BranchVector));  // writing into output instruction file
    }
    ofile.close();

    return 0;
}



