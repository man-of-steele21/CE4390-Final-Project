#include <iostream> 
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>

#define MAX_ETHERNET_ADDRESSES 99

using namespace std;

int myip1, myip2, myethadd, mydrip1, mydrip2, mybridge, mybport, mydestip1, mydestip2, srcip1, srcip2;

ofstream outputfile;
ifstream inputfile;

struct ARPEntry
{
    int ip1, ip2, Ethernet;
};

void EthernetReceiveFromIP(string Message, int EthDest)
{
  //cout << "writing **" << Message << "** to Ethernet dest: " << EthDest << endl;
  outputfile << EthDest << " " << myethadd << " " << Message;
  outputfile.flush();
}

void IPReceiveFromTransport(string TransMessage)
{
  stringstream outputstream;

  outputstream << "IP " << srcip1 << " " << srcip2 << " " << myip1 << " " << myip2 << " " << TransMessage;
  if (srcip1 == myip1)
     EthernetReceiveFromIP(outputstream.str(), 1);

  // Start work here
}

void TransportReceiveFromIP(string TransMessage)
{
  string data, protocol;
  int seqnum, chnum;

  stringstream inputstream;
  stringstream outputstream;

  inputstream.str(TransMessage);
  inputstream >> protocol;
  if (protocol == "DA")
    {
      cout << "Transport Protocol " << protocol << endl;
      inputstream >> seqnum >> chnum;
      getline(inputstream, data);
      cout << "Host (" << myip1 << "," << myip2 << ") received data from host (" << srcip1 << "," << srcip2 << "): " << data << endl;
      outputstream << "AK " << seqnum << " " << chnum << endl;
      IPReceiveFromTransport(outputstream.str());
    }
}

void IPReceiveFromEthernet(string IPMessage)
{
  string TransMessage, protocol;
  int destip1, destip2;

  stringstream inputstream;

  inputstream.str(IPMessage);
  inputstream >> protocol;
  if (protocol == "IP")
    {
      cout << "Protocol: " << protocol << endl;
      inputstream >> destip1 >> destip2 >> srcip1 >> srcip2;
      getline(inputstream, TransMessage);
      //    cout << "Transport Message: " << TransMessage << endl;
      if (destip1 == myip1 && destip2 == myip2)
	TransportReceiveFromIP(TransMessage);
    }
}

void EthernetReceiveFromBridge(ifstream &MyInputFile)
{
    string EthMessage;
    string IPMessage;
    int src, dest;

    stringstream inputstream;
    while (!MyInputFile.eof())
      {
	if (getline (MyInputFile, EthMessage))
	  {
	    inputstream.str(EthMessage);
	    inputstream >> dest >> src;
	    getline(inputstream, IPMessage);
	    //    cout << "IPMessage: " << IPMessage << endl;
	    IPReceiveFromEthernet(IPMessage);
	  }
      }
}

void GetARP(int tIPadd1, int tIPadd2, int sIPadd1, int sIPadd2, int sEthadd)
{
  // Finish code
}

//  *************************************************************** Added
void TransportPeriodicTasks()
{
  stringstream outputstream;

  // Output "Hello" message

       outputstream << "HL " << myip1 << " " << myip2 << " " << myethadd<< endl;
      //     cout << outputstream.str();
      EthernetReceiveFromIP(outputstream.str(), 99);
      outputstream.str("");

  this_thread::sleep_for(chrono::seconds(4));
}
  
int main(int argc, char** argv) 
{
    string message, outfilename, infilename;
    ARPEntry ARPTable[MAX_ETHERNET_ADDRESSES];

    if (argc != 11 && argc != 8)
      {
      cout << "Incorrect number of arguments entered. Terminating" << endl;
      exit(1);
      }

    myip1 = stoi(argv[1]);
    myip2 = stoi(argv[2]);

    myethadd = stoi(argv[3]);

    mydrip1 = stoi(argv[4]);
    mydrip2 = stoi(argv[5]);

    mybridge = stoi(argv[6]);
    mybport = stoi(argv[7]);

    if (argc == 11)
      {
	mydestip1 = stoi(argv[8]);
	mydestip2 = stoi(argv[9]);

	message = argv[10];
      }

    cout << endl << "********************** HOST ************************" << endl;

    cout << "Host IP: (" << myip1 << "," << myip2 << ")" << endl;
    cout << "Ethernet address: " << myethadd << endl;
    cout << "Default router IP: (" << mydrip1 << "," << mydrip2 << ")" << endl;
    cout << "Bridge ID and bridge port: (" << mybridge << "," << mybport << ")" << endl;
    if (argc == 11)
      {
    cout << "Destination node IP: (" << mydestip1 << "," << mydestip2 << ")" << endl;
    cout << "Message contents: " << message << endl;
      }
    else
      cout << "No message to send" << endl;
  
    outfilename = "toB";
    outfilename.append(to_string(mybridge));
    outfilename.append("P");
    outfilename.append(to_string(mybport));
    outfilename.append(".txt");
    infilename = "fromB";
    infilename.append(to_string(mybridge));
    infilename.append("P");
    infilename.append(to_string(mybport));
    infilename.append(".txt");

    outputfile.open(outfilename);
    inputfile.open(infilename);

    /* if (outputfile.is_open())
      cout << "Output file sucessfully opened" << endl;
    else
    cout << "Output file NOT sucessfully opened" << endl;*/

    if (inputfile.is_open())
      ;// cout << "Input file sucessfully opened" << endl;
    else
      {
	//	cout << "Input file NOT sucessfully opened initially, creating file" << endl;
	ofstream tempfile;
	tempfile.open(infilename);
	tempfile.close();
	inputfile.open(infilename);
      }


    /*    cout << "Host: (" << myip1 << "," << myip2 << ") Output file name: " << outfilename << endl;
	  cout << "Host: (" << myip1 << "," << myip2 << ") Input file name: " << infilename << endl; */

       while (true)
      {
	  TransportPeriodicTasks();
	  EthernetReceiveFromBridge(inputfile);
	  inputfile.clear();
	  // cout << "Inside while loop" << endl;
	  this_thread::sleep_for(chrono::seconds(1));
      }
    
  
    return 0; 
} 
