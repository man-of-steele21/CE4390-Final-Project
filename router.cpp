#include <iostream> 
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>

#define MAX_ETHERNET_ADDRESSES 99
#define BROADCAST_ETHERNET_ADDRESS 99
#define MAX_NETWORKS 10
#define MAX_HOSTS 10

using namespace std;

void EthernetReceiveFromBridge(ifstream &, int);

int numnetworks, LastReceivedEthernetAddress, FoundEthernetAddress, sleeptimer;
int* myip1, * myip2, * myethadd, * mybridge, * mybport, * position;

string* outfilename, *infilename;

int ARPTable[MAX_NETWORKS][MAX_HOSTS];

ofstream* outputfile;
ifstream* inputfile;

void EthernetReceiveFromIP(int destethernet, int networknumber, string ipmessage)
{
  //  cout << destethernet << " " << myethadd[networknumber] << " IP " << ipmessage;

  outputfile[networknumber] << destethernet << " " << myethadd[networknumber] << " " << ipmessage;
  outputfile[networknumber].flush();
}

int GetARP(int tIPadd1, int tIPadd2, int SIPadd1, int SIPadd2, int NetworkNumber)
{
  stringstream outputstream;

  outputstream << "ARP REQ " << tIPadd1 << " " << tIPadd2 << " " << SIPadd1 << " " << SIPadd2 << " " << myethadd[NetworkNumber] << endl;
  //  cout << "Outputstream: " << outputstream.str() << endl;
  FoundEthernetAddress = -1;
  EthernetReceiveFromIP(99, NetworkNumber, outputstream.str());
  while (FoundEthernetAddress == -1)
    {
      inputfile[NetworkNumber].open(infilename[NetworkNumber]);
      //	   cout << "Host: (" << myip1 << "," << myip2 << ") Seeking to position " << position << endl;
      inputfile[NetworkNumber].seekg(position[NetworkNumber]);
      EthernetReceiveFromBridge(inputfile[NetworkNumber], NetworkNumber);
      this_thread::sleep_for(chrono::seconds(1));
      inputfile[NetworkNumber].close();
    }
  return FoundEthernetAddress;
}

void IPReceiveFromEthernet(string IPMessage, int SrcNetwork)
{
  string protocol, subprotocol;
  int tempsrcip1, tempsrcip2, destip1, destip2, srceth;

  stringstream inputstream;
  stringstream outputstream;

  //  cout << "Host (" << myip1 << "," << myip2 <<  ") received IP Message: " << IPMessage << endl;

  inputstream.str(IPMessage);
  inputstream >> protocol;
  
  //  cout << "Router (" << myip1[SrcNetwork] << "," << myip2[SrcNetwork] <<  ") received IP Message: " << inputstream.str() << endl;

  //  cout << "Host (" << myip1 << "," << myip2 << ") Protocol: " << protocol << endl;
  if (protocol == "IP")
    {
      //   cout << "Protocol: " << protocol << endl;
      inputstream >> destip1 >> destip2;
      for (int i = 0; i < numnetworks; i++)
	{
	  //  	  cout << "destip1: " << destip1 << endl << "destip2: " << destip2 << endl << "myip1[i]: " << myip1[i] << endl;
	  if (myip1[i] == destip1)
	    {
	      //	      cout << "myip1[i] is equal to destip1" << endl;
	      //     cout << "Router about to write: " << IPMessage << ". to ethernet address: " << GetARP(destip1, destip2, myip1[i], myip2[i], i) << endl;
	      if (ARPTable[destip1][destip2] != -1)
		{
		  //		  cout << "IPMessage: ." << IPMessage << endl;
		  //	  cout << "Found ethernet address: " << ARPTable[destip1][destip2] << endl;
		  EthernetReceiveFromIP(ARPTable[destip1][destip2], i, IPMessage);
		}
	      else
		EthernetReceiveFromIP(GetARP(destip1, destip2, myip1[i], myip2[i], i), i, IPMessage);
	    }
	}
      //     getline(inputstream, TransMessage);
      //    cout << "Transport Message: " << TransMessage << endl;
      //      if (destip1 == myip1 && destip2 == myip2)
      //	TransportReceiveFromIP(TransMessage);
    }
    else if (protocol == "ARP")
    {
      inputstream >> subprotocol;
      if (subprotocol == "REQ")
	{
	  inputstream >> destip1 >> destip2 >> tempsrcip1 >> tempsrcip2 >> srceth;
	  ARPTable[tempsrcip1][tempsrcip2] = srceth;
	  for (int i = 0; i < numnetworks; i++)
	    {
	      if ((myip1[i] == destip1) && (myip2[i] == destip2))
		{
		  outputstream << "ARP REP " << myip1[i] << " " << myip2[i] << " " << myethadd[i] << " " << tempsrcip1 << " " << tempsrcip2 << " " << srceth << endl;
		  //	      cout << "Sending ARP REP: " << outputstream.str() << endl;
		  EthernetReceiveFromIP(srceth, i, outputstream.str());
		}
	    }
	 }
      else if (subprotocol == "REP")
	{
	  //cout << "Found ARP REP" << endl;
	  inputstream >> tempsrcip1 >> tempsrcip2 >> srceth >> destip1 >> destip2;
	  for (int i = 0; i < numnetworks; i++)
	    {
	      if ((myip1[i] == destip1) && (myip2[i] == destip2))
		{
		  ARPTable[tempsrcip1][tempsrcip2] = srceth;
		  FoundEthernetAddress = srceth;
		}
	    }
	}
      }
}

void EthernetReceiveFromBridge(ifstream &MyInputFile, int NetworkNumber)
{
    string EthMessage;
    string IPMessage;
    int dest;

    stringstream inputstream;

    //      cout << "Host (" << myip1 << "," << myip2 <<  ") Inside EthernetReceiveFromBridge: " << inputstream.str() << endl;

    // 	cout << "(" << myip1 << "," << myip2 << ") position is: " << MyInputFile.tellg() << endl;

    if (!MyInputFile.eof()) // changed from while
      {
	if (getline (MyInputFile, EthMessage))
	  {
	    //	    cout << "Host (" << myip1 << "," << myip2 <<  ") received Ethernet Message: " << EthMessage << endl;
	    position[NetworkNumber] += (EthMessage.length() + 1);
	    inputstream.str(EthMessage);
	    inputstream >> dest >> LastReceivedEthernetAddress;
	    inputstream.get();
	    getline(inputstream, IPMessage);
	    IPMessage.append("\n");
	    //	    cout << "Router (" << myip1[NetworkNumber] << "," << myip2[NetworkNumber] <<  ") calling IPReceiveFromEthernet with IP Message: " << IPMessage << endl;

	    //    cout << "IPMessage: " << IPMessage << endl;
	    IPReceiveFromEthernet(IPMessage, NetworkNumber);
	  }
      }
}
  
void TransportPeriodicTasks()
{
  stringstream outputstream;

  // Output "Hello" message
  if (sleeptimer > 4)
    {
      for (int i = 0; i < numnetworks; i++)
	{
	  outputstream << "HL " << myip1[i] << " " << myip2[i] << " " << myethadd[i]<< endl;
	  //     cout << outputstream.str();
	  EthernetReceiveFromIP(99, i, outputstream.str());
	  outputstream.str("");
	}
      sleeptimer = 0;
    }

  //  this_thread::sleep_for(chrono::seconds(2));
}

int main(int argc, char** argv) 
{

  for (int i = 0; i < MAX_NETWORKS; i++)
    for (int j = 0; j < MAX_HOSTS; j++)
      ARPTable[i][j] = -1;

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

    position = new int[numnetworks];

    for (int i = 0; i < numnetworks; i++)
      position[i] = 0;

    for (int i = 0; i < numnetworks; i++)
      {
	cout << "Host IP: (" << myip1[i] << "," << myip2[i] << ")" << endl;
	cout << "Bridge ID and bridge port: (" << mybridge[i] << "," << mybport[i] << ")" << endl;
	cout << "Ethernet address: " << myethadd[i] << endl;
      }
    sleeptimer = 0;
    while (true)
      {
	for (int i = 0; i < numnetworks; i++)
	  {
	    inputfile[i].open(infilename[i]);
	    inputfile[i].seekg(position[i]);
	  }
	 
	  TransportPeriodicTasks();
	  // cout << "Before EthernetReceiveFromBridge" << endl;
	  for (int i = 0; i < numnetworks; i++)
	    EthernetReceiveFromBridge(inputfile[i], i);
	  //  cout << "After EthernetReceiveFromBridge" << endl;
	  // cout << "Inside while loop" << endl;
	  this_thread::sleep_for(chrono::seconds(1));
	  TransportPeriodicTasks();
	  for (int i = 0; i < numnetworks; i++)
	    inputfile[i].close();
      }
  
    return 0; 
} 
