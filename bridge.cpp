#define MAX_ETHERNET_ADDRESSES 99

#include <iostream> 
#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include <fstream>
using namespace std;

struct ARPEntry
{
    int ip1, ip2, Ethernet;
};

int myid, numports, numneighbors;
int * neighbors;

string* outfilename, *infilename;

ARPEntry ARPTable[MAX_ETHERNET_ADDRESSES];

ofstream* outputfile;
ifstream* inputfile;

void EthernetPeriodicTasks()
{
  stringstream inputstream;
  stringstream outputstream;

  string MyMessage;

  for (int i = 0; i < numports; i++)
    {
      while (!inputfile[i].eof())
      {
	if (getline (inputfile[i], MyMessage))
	  cout << MyMessage << endl;
      }
    }
}
  
int main(int argc, char** argv) 
{
    if (argc < 3)
      {
      cout << "Incorrect number of arguments entered. Terminating" << endl;
      exit(1);
      }

    myid = stoi(argv[1]);
    numports = stoi(argv[2]);
    numneighbors = argc - 3;

    neighbors = new int[numneighbors];

    for (int i = 0; i < numneighbors; i++)
	neighbors[i] = stoi(argv[i + 3]);

    cout << endl << "********************** BRIDGE ************************" << endl;
	
    cout << "Bridge ID: " << myid << endl;
    cout << "Number of ports: " << numports << endl;
    cout << "Number of neighbors: " << numneighbors << endl;
    cout << "Neighboring bridges:";
    
    for (int i = 0; i < numneighbors; i++)
      cout << " " << neighbors[i];

    cout << endl;

    // ************************* Open port files ************************

    /*  myip1 = new int[numports];
    myip2 = new int[numports];
    mybridge = new int[numports];
    mybport = new int[numports];
    myethadd = new int[numports]; */

    outfilename = new string[numports];
    infilename = new string[numports];
    outputfile = new ofstream[numports];
    inputfile = new ifstream[numports];

    for (int i = 0; i < numports; i++)
      {
	outfilename[i] = "fromB";
	outfilename[i].append(to_string(myid));
	outfilename[i].append("P");
	outfilename[i].append(to_string(i));
	outfilename[i].append(".txt");
	infilename[i] = "toB";
	infilename[i].append(to_string(myid));
	infilename[i].append("P");
	infilename[i].append(to_string(i));
	infilename[i].append(".txt");

	outputfile[i].open(outfilename[i]);
	inputfile[i].open(infilename[i]);

	if (outputfile[i].is_open())
	  cout << "Output file sucessfully opened" << endl;
	else
	  cout << "Output file NOT sucessfully opened" << endl;

	if (inputfile[i].is_open())
	  cout << "Input file sucessfully opened" << endl;
	else
	  {
	    cout << "Input file NOT sucessfully opened initially, creating file" << endl;
	    ofstream tempfile;
	    tempfile.open(infilename[i]);
	    tempfile.close();
	    inputfile[i].open(infilename[i]);
	  }
      }

    //************************ Processing Data **************************

    while (true)
      {
	  for (int i = 0; i < numports; i++)
	    inputfile[i].clear();
	  this_thread::sleep_for(chrono::seconds(1));
      }
  
    return 0; 
} 
