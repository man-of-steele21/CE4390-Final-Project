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
int* neighbors, * ethports;

string* outfilename, * infilename, * boutfilename, * binfilename;

ARPEntry ARPTable[MAX_ETHERNET_ADDRESSES];

ofstream* outputfile, * boutputfile;
ifstream* inputfile, * binputfile;

void EthernetPeriodicTasks()
{
  stringstream inputstream;
  stringstream outputstream;

  string MyMessage;

  int src, dest;

  bool ethmatch;

  for (int i = 0; i < numports; i++)
    {
      while (!inputfile[i].eof())
      {
	if (getline (inputfile[i], MyMessage))
	  {
	    inputstream.str(MyMessage);
	    inputstream >> dest >> src;
	    ethports[i] = src;
	    if (dest == 99)
	      {
		for (int j = 0; j < numports; j++)
		  {
		    if (i != j)
		      {
			outputfile[j] << MyMessage << endl;
			outputfile[j].flush();
		      }
		    else;
		  }

		for (int l = 0; l < numneighbors; l++)
		  {
		    boutputfile[l] << MyMessage << endl;
		    boutputfile[l].flush();
		  }
	      }

	    else
	      {
		ethmatch = false;
		for (int k = 0; k < numports; k++)
		  {
		    if (ethports[k] == dest)
		      {
			ethmatch = true;
			outputfile[k] << MyMessage << endl;
			outputfile[k].flush();
		      }
		    else
		      {
			
		      }
		  }//for (k)
		if (!ethmatch)
		  {
		    //  cout << "No Ethernet match" << endl;
		    for (int l = 0; l < numneighbors; l++)
		      {
			boutputfile[l] << MyMessage << endl;
			boutputfile[l].flush();
		      }
		  }
	      }
		  
	  }// if (getline)
      }// while
    }// for (i)

  for (int i = 0; i < numneighbors; i++)
    {
      while (!binputfile[i].eof())
      {
	if (getline (binputfile[i], MyMessage))
	  {
	    inputstream.str(MyMessage);
	    //   cout << "Bridge: " << myid << " " << MyMessage << endl;
	    inputstream >> dest >> src;
	    //	    ethports[i] = src;
	    if (dest == 99)
	      {
		for (int j = 0; j < numports; j++)
		  {
		    if (i != j)
		      {
			outputfile[j] << MyMessage << endl;
			outputfile[j].flush();
		      }
		    else;
		  }

		for (int l = 0; l < numneighbors; l++)
		  {
		    if (i != l)
		      {
			boutputfile[l] << MyMessage << endl;
			boutputfile[l].flush();
		      }
		    else;
		  }
	      }

	    else
	      {
		ethmatch = false;
		for (int k = 0; k < numports; k++)
		  {
		    if (ethports[k] == dest)
		      {
			ethmatch = true;
			outputfile[k] << MyMessage << endl;
			outputfile[k].flush();
		      }
		    else
		      {
			
		      }
		  }//for (k)
		if (!ethmatch)
		  {
		    //  cout << "No Ethernet match" << endl;
		    for (int l = 0; l < numneighbors; l++)
		      {
			if (i != l)
			  {
			    boutputfile[l] << MyMessage << endl;
			    boutputfile[l].flush();
			  }
			else;
		      }
		  }
	      }
		  
	  }// if (getline)
      }// while
    }// for (i)
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

    ethports = new int[numports];
    for (int h = 0; h < numports; h++)
      ethports[h] = -1;

    outfilename = new string[numports];
    infilename = new string[numports];
    outputfile = new ofstream[numports];
    inputfile = new ifstream[numports];
    
    boutfilename = new string[numneighbors];
    binfilename = new string[numneighbors];
    boutputfile = new ofstream[numneighbors];
    binputfile = new ifstream[numneighbors];

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

	/*	if (outputfile[i].is_open())
	  cout << "Output file sucessfully opened" << endl;
	else
	cout << "Output file NOT sucessfully opened" << endl;*/

	if (inputfile[i].is_open())
	  ;// cout << "Input file sucessfully opened" << endl;
	else
	  {
	    // cout << "Input file NOT sucessfully opened initially, creating file" << endl;
	    ofstream tempfile;
	    tempfile.open(infilename[i]);
	    tempfile.close();
	    inputfile[i].open(infilename[i]);
	  }
      }

    for (int i = 0; i < numneighbors; i++)
      {
	boutfilename[i] = "B";
	boutfilename[i].append(to_string(myid));
	boutfilename[i].append("B");
	boutfilename[i].append(to_string(neighbors[i]));
	boutfilename[i].append(".txt");
	binfilename[i] = "B";
	binfilename[i].append(to_string(neighbors[i]));
	binfilename[i].append("B");
	binfilename[i].append(to_string(myid));
	binfilename[i].append(".txt");

	boutputfile[i].open(boutfilename[i]);
	binputfile[i].open(binfilename[i]);

	if (binputfile[i].is_open())
	  ;// cout << "Input file sucessfully opened" << endl;
	else
	  {
	    // cout << "Input file NOT sucessfully opened initially, creating file" << endl;
	    ofstream tempfile;
	    tempfile.open(binfilename[i]);
	    tempfile.close();
	    binputfile[i].open(binfilename[i]);
	  }
      }

    //************************ Processing Data **************************

    this_thread::sleep_for(chrono::seconds(1));
    while (true)
      {
	EthernetPeriodicTasks();
	  for (int i = 0; i < numports; i++)
	    inputfile[i].clear();
	  for (int i = 0; i < numneighbors; i++)
	    binputfile[i].clear();
	  this_thread::sleep_for(chrono::seconds(1));
      }
  
    return 0; 
} 
