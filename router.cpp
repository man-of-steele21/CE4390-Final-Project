#include <iostream> 
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>

#define MAX_ETHERNET_ADDRESSES 99
#define BROADCAST_ETHERNET_ADDRESS 99

using namespace std;

struct ARPEntry
{
    int ip1, ip2, Ethernet;
};

    int numnetworks;
    int* myip1, * myip2, * myethadd, * mybridge, * mybport;

    string* outfilename, *infilename;

    ARPEntry ARPTable[MAX_ETHERNET_ADDRESSES];

    ofstream* outputfile;
    ifstream* inputfile;

void EthernetReceiveFromIP(int destethernet, int networknumber, string ipmessage)
{
  //  cout << destethernet << " " << myethadd[networknumber] << " IP " << ipmessage;

  outputfile[networknumber] << destethernet << " " << myethadd[networknumber] << " " << ipmessage;
  outputfile[networknumber].flush();
}
  
void TransportPeriodicTasks()
{
  stringstream outputstream;

  // Output "Hello" message

  for (int i = 0; i < numnetworks; i++)
    {
      outputstream << "HL " << myip1[i] << " " << myip2[i] << " " << myethadd[i]<< endl;
      //     cout << outputstream.str();
      EthernetReceiveFromIP(99, i, outputstream.str());
      outputstream.str("");
    }

  this_thread::sleep_for(chrono::seconds(4));
}

int main(int argc, char** argv) 
{

    if ((((argc - 1) % 5) != 0) || argc == 1)
      {
      cout << "Incorrect number of arguments entered. Terminating" << endl;
      exit(1);
      }

    numnetworks = ((argc - 1) / 5);
    myip1 = new int[numnetworks];
    myip2 = new int[numnetworks];
    mybridge = new int[numnetworks];
    mybport = new int[numnetworks];
    myethadd = new int[numnetworks];

    outfilename = new string[numnetworks];
    infilename = new string[numnetworks];
    outputfile = new ofstream[numnetworks];
    inputfile = new ifstream[numnetworks];

    for (int i = 0; i < numnetworks; i++)
      {
	myip1[i] = stoi(argv[i * 5 + 1]);
	myip2[i] = stoi(argv[i * 5 + 2]);
	mybridge[i] = stoi(argv[i * 5 + 3]);
	mybport[i] = stoi(argv[i * 5 + 4]);
	myethadd[i] = stoi(argv[i * 5 + 5]);

	 outfilename[i] = "toB";
	 outfilename[i].append(to_string(mybridge[i]));
	 outfilename[i].append("P");
	 outfilename[i].append(to_string(mybport[i]));
	 outfilename[i].append(".txt");
	 infilename[i] = "fromB";
	 infilename[i].append(to_string(mybridge[i]));
	 infilename[i].append("P");
	 infilename[i].append(to_string(mybport[i]));
	 infilename[i].append(".txt");

	 outputfile[i].open(outfilename[i]);
	 inputfile[i].open(infilename[i]);

	 /*	 if (outputfile[i].is_open())
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

    cout << endl << "********************** ROUTER ************************" << endl;

    cout << "Router info:" << endl;

    for (int i = 0; i < numnetworks; i++)
      {
	cout << "Host IP: (" << myip1[i] << "," << myip2[i] << ")" << endl;
	cout << "Bridge ID and bridge port: (" << mybridge[i] << "," << mybport[i] << ")" << endl;
	cout << "Ethernet address: " << myethadd[i] << endl;
      }

    while (true)
      {
	  TransportPeriodicTasks();
	  for (int i = 0; i < numnetworks; i++)
	    inputfile[i].clear();
	  // cout << "Inside while loop" << endl;
	  this_thread::sleep_for(chrono::seconds(1));
      }
  
    return 0; 
} 
