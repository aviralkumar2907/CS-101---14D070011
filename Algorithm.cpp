#include<iostream>
#include<fstream>
#include<cmath>

namespace constants
{
  const double PI=3.14159265;
  const int NoOfCycles=5;   // for optimal no. of cycles for optimalSolutionWithSwap()
}

using namespace constants;
using namespace std;

//Node in x, y because Cartesian coordinates allow easy swapping of nodes in path
struct Node
{
    double x, y;
    Node *nextNode;
    Node()
    {
        x=0.0;
        y=0.0;
        nextNode=NULL;
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
    Node *pathNode, *firstNode;
    int ballCount;
    double minPathLength;
   public:
    double branchLength(int nodePos1, int nodePos2);  //distance between nodePos1 and nodePos2
    void sortForGreedyAlgorithm();
    void convertPathToArray();
    void pushBackAnotherNode();

    Path()                                   // Constructor to initialise path and set it with 'Greedy Salesman' Algorithm
    {
        firstNode=pathNode=NULL;
        ballCount=0;

        ifstream ifile("Points.dat", ios::binary);
        while(!ifile.eof())
        {
           if(ifile.eof()) break;
           pushBackAnotherNode();
           ifile.read((char*)(pathNode), sizeof(Node));
        }
        ifile.close();

        ballCount--;    //-1 as there is some internal error while copying Node points from file as last entry/point seems to be repeated

        convertPathToArray();

        sortForGreedyAlgorithm();                     //Function Call to sort for approximately shortest path using 'Greedy Salesman' Algorithm

        minPathLength=0;
        for(int i=0; i<ballCount-1; i++)
        {
            minPathLength+=branchLength(i,i+1);       //Additive Accumulation idiom to calculate minPathLength obtained after sorting
        }

    }
    ~Path()
    {
        delete[] pathNode;
    }



    /*
    Function  : swap2BranchEnds(int, int)
    Input     : int i1, int i2,  i.e. indices of the fixed end points
                where corresponding next, pervious end points are to be swapped
    Output    : Returns void, pathNode[] representing a path with the two specified end points(i1+1, i2-1)
                swapped in the sequence, keeping the corresponding i1 and i2 branch end points same
    Logic     : Swapping nodes (i1+1) and (i2-1) by altering the path from i1, i1+1 to i2-1, i2
                to i1, i2-1 to i1+1 , i2 by simple array operations, i.e. making a part of the array be stored
                in reverse order by simply swapping entires till the middle.
                Also i1<i2 is necessary for the loop and thus the code snippet for the same is implemented.
                (given with comments)
    Example   : BotPath.swap2BranchEnds(0,3);
    */

void swap2BranchEnds(int i1, int i2)      //function to swap two branch endpoints and set the path accordingly
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
    Output    : Returns nothing. But, pathNode[] is set to the optimal solution for given set of points (/or quite close to it).
    Logic     : It tries out all possible swaps between end points of the branches (say with indices i1 and i2)
                keeping i1-1 and i2+1 fixed. Checks if new path length is lesser by
                only checking for the change in path length and seeing is its negative.
                And if so, sets this new path as the new solution using the function call to
                swap2BranchEnds(i1-1, i2+1) for above mentioned i1 and i2.
                With many iterations( in our case NoOfCycles=5, as mentioned in the namespace), we get the optimal solution.
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
                       swap2BranchEnds(i,j);
                   }

           }
        }

        //use swap2branches to find a better, more accurate path
        //printing the path as set of consecutive points on it:
        cout<<"\nPrinting the optimal solution in Cartesian System:\n";
        for(int k=0; k<ballCount; k++)
        {
            cout<<"\n("<<pathNode[k].x<<" "<<pathNode[k].y<<")";
        }

    }


    /*
    Function  : getPathNode(int)
    Input     : int i, i.e. index of the Node.
    Output    : Node on the pathNode[] with index i.
    Logic     : It's an accessor function.
                If i is in range i.e. 0 to ballCount-1 return pathNode[i], i.e. Node at index i,
                else return (0,0), i.e. error point.
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
    Output    : int ballCount ,i.e. total number of balls in the arena
    Logic     : It's a simple accessor function.
                Returns 'ballCount' variable defined in the class Path.
    Example   : int N=BotPath.getBallCount();

    */
    int getBallCount()     //accessor function returning number of Balls
    {
        return ballCount;
    }
};
 /*
    Function  : pushBackAnotherNode()
    Input     : void
    Output    : Requests for heap memory and stores the address in pathNode, to be used as a new Node point.
    Logic     : Using new statement to request for memory from the heap for the new node.
                In case of entry for the first time, firstNode is initialised with the address of the first node
                in the linked list. For all other entries the previous node created stores the address of the new node
                in its data member -'nextNode'.
                Thus, implementing the function for the linked list of nodes, for insertion of new entry.
    Example   : pushBackAnotherNode();
    */
    void Path::pushBackAnotherNode()
    {
        if(pathNode==NULL&&firstNode==NULL)
        {
            pathNode=new Node;
            firstNode=pathNode;
            ballCount++;
        }
        else
        {
            pathNode->nextNode=new Node;    // pathNode points to the previous node, whise data member nextNode
                                            // stores the address of the new node.
            pathNode=pathNode->nextNode;
            ballCount++;
        }
    }

  /*
    Function  : convertPathToArray()
    Input     : void
    Output    : Returns void, pathNode[] is created containing all the points in the file, which were earlier stored as a linked list.
    Logic     : Function requests for an array of heap memory of type Node, pointed to by pointer pathNode. All the entries
                of the linked list are copied into the pathNode array. Copying is implemented using simple accession of linked list
                elements, starting from first element - whose address is stored in firstNode. The address of the next element
                is stored in the data member 'nextNode'. Every next entry is accessed using its address stored in
                nextNode and then the current original node is deleted/freed using delete statement, since it's no longer required.
    Example   : void convertPathToArray();
    */
    void Path::convertPathToArray()
    {
        pathNode = new Node[ballCount];

        for(int i=0; i<ballCount; i++)
        {
            pathNode[i] = *firstNode;   //data of the node in the linked list is copied into the array
            delete firstNode;  // firstNode points to the original current node to be deleted after copying
            firstNode = pathNode[i].nextNode;  //pathNode[i].nextNode was copied from firstNode.nextNode
                                               // thus it stores the address to the next node
            pathNode[i].nextNode=NULL;
        }
        delete firstNode;  // as due to some internal error while  copying Node points from file as last entry/point seems to be repeated
                          // firstNode points to that not required Node Point which needs to be deleted.
    }

/*
  Function  : branchLength(int , int )
  Input     : int nodePos1, int nodePos2, i.e. indices of nodes between which distance is to be calculated
  Output    : (double) distance between the two nodes specified by the indices, on pathNode[]
  Logic     : using distance between two points (x1,y1), (x2,y2) i.e. distance in Cartesian coordinates
              i.e. square root of ( square of difference of(x) + square of difference of(y) ).
              Square root function is implemented using 'cmath' library, included above.
  Example   : double d0= BotPath.branchLength(0,1);
*/

double Path::branchLength(int nodePos1, int nodePos2)  //distance between nodePos1 and nodePos2
    {
        return sqrt(pow(pathNode[nodePos1].x-pathNode[nodePos2].x,2)+pow(pathNode[nodePos1].y-pathNode[nodePos2].y,2));
    }

/*
 Function  : sortForGreedyAlgorithm()
 Input     : void
 Output    : Returns void, pathNode[] set to a path decided by the Greedy Salesman Algorithm
             i.e., the Salesman travels to the next nearest town first
 Logic     : Use 'Selection sorting' algorithm to implement 'Greedy Salesman Algorithm'
             based on distances between nodes. (Given fixed initial point.) According to the Greedy
             Salesman algorithm/ Nearest Neighbour algorithm the closest Node/Point is set to be the next destination
             in the Path to cover all the points. This exercise is then repeated till all the Nodes/ Points are covered.
             The implementation for our case uses selection sort algorithm, in which the nearest neighbour 
             is picked and its position is swapped with the element at the next Node in the array.
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
    BranchVector* pathVector;
    int branchCount;

   public:

    BotVectorPath( Path P )
    {
        branchCount=P.getBallCount()-1;
        pathVector = new BranchVector[branchCount];
        Node Node1, Node2;

        int i=0;

        cout<<"\n\n\nPrinting the optimal path as set of Vectors in Polar Coordinates:\n";
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
              else if(Node2.y<Node1.y)
                pathVector[i].theta=270;
              else
                pathVector[i].theta=0;
            }
            //printing r, theta values in order
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
     ~BotVectorPath()
     {
         delete[] pathVector;
     }

     /*
      Function  : getBranch(int)
      Input     : int pos , i.e. index of the branch you require
      Output    : pathVector[pos], i.e. the BranchVector variable
                  corresponding to index pos on pathVector[] array.
      Logic     : It's an accessor function returning an element of an array pathVector[]
      Example   : BranchVector B=BotVecPath.getBranch(0);  // the first(i=0) branch in path is stored into B
     */
     BranchVector& getBranch(int pos)
     {
          return pathVector[pos];
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

    cout<<"\n\n\nPrinting the Optimal Solution as a set of Instructions in (distance, angle) format:\n";
    for(int i=0; i<count;i++)
    {
        BranchVector BranchV=BotVecPath.getBranch(i);
        cout<<"\n ("<<BranchV.radius<<" , "<<BranchV.theta<<")";
        ofile.write((char*)&BranchV, sizeof(BranchVector));  // writing into output instruction file
    }
    ofile.close();

    return 0;
}



