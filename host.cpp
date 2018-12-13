#include <iostream> 
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>

#define MAX_ETHERNET_ADDRESSES 99

using namespace std;

void EthernetReceiveFromBridge(ifstream &);

int myip1, myip2, myethadd, mydrip1, mydrip2, mybridge, mybport, mydestip1, mydestip2, srcip1, srcip2;

int LastReceivedEthernetAddress, FoundEthernetAddress;

bool acked;

streampos position;

ofstream outputfile;
ifstream inputfile;

string outfilename, infilename;

struct ARPEntry
{
    int ip1, ip2;
};

ARPEntry ARPTable[MAX_ETHERNET_ADDRESSES];

void EthernetReceiveFromIP(string Message, int EthDest)
{
  // cout << "writing **" << Message << "** to Ethernet dest: " << EthDest << endl;
  outputfile << EthDest << " " << myethadd << " " << Message;
  outputfile.flush();
}

int GetARP(int tIPadd1, int tIPadd2)
{
  stringstream outputstream;

  outputstream << "ARP REQ " << tIPadd1 << " " << tIPadd2 << " " << myip1 << " " << myip2 << " " << myethadd << endl;
  //  cout << "Outputstream: " << outputstream.str() << endl;
  FoundEthernetAddress = -1;
  EthernetReceiveFromIP(outputstream.str(), 99);
  while (FoundEthernetAddress == -1)
    {
      inputfile.open(infilename);
      //	   cout << "Host: (" << myip1 << "," << myip2 << ") Seeking to position " << position << endl;
      inputfile.seekg(position);
      EthernetReceiveFromBridge(inputfile);
      this_thread::sleep_for(chrono::seconds(1));
      inputfile.close();
    }
  return FoundEthernetAddress;
}

void IPReceiveFromTransport(string TransMessage)
{
  bool FoundEthernet = false;
  stringstream outputstream;

  outputstream << "IP " << srcip1 << " " << srcip2 << " " << myip1 << " " << myip2 << " " << TransMessage;
  if (srcip1 == myip1)
    {
      for (int i = 0; (i < MAX_ETHERNET_ADDRESSES) && (FoundEthernet == false); i++)
	{
	  //	cout << "ARPTable = " << ARPTable[i].ip1 << "," << ARPTable[i].ip2 << endl;
	  if (ARPTable[i].ip1 == srcip1 && ARPTable[i].ip2 == srcip2)
	  {
	    //	    cout << "About to send to Ethernet: " << i << endl;
	    EthernetReceiveFromIP(outputstream.str(), i);
	    FoundEthernet = true;
	  }
	}
      if (!FoundEthernet)
	EthernetReceiveFromIP(outputstream.str(), GetARP(srcip1, srcip2));
    }

  //EthernetReceiveFromIP(outputstream.str(), LastReceivedEthernetAddress);

  // Start work here
  // Send to the Ethernet address of the default router
}

void TransportReceiveFromIP(string TransMessage)
{
  string data, protocol;
  int seqnum, chnum;

  stringstream inputstream;
  stringstream outputstream;

  inputstream.str(TransMessage);
  inputstream >> protocol;
  // cout << TransMessage << endl;
  if (protocol == "DA")
    {
      //    cout << "Transport Protocol " << protocol << endl;
      inputstream >> seqnum >> chnum;
      getline(inputstream, data);
            cout << "Host (" << myip1 << "," << myip2 << ") received data from host (" << srcip1 << "," << srcip2 << "): " << data << endl;
      outputstream << "AK " << seqnum << " " << chnum << endl;
      IPReceiveFromTransport(outputstream.str());
    }
  else if (protocol == "AK")
    {
      //   cout << "Got an ACK" << endl;
      acked = true;
    }
}

void IPReceiveFromEthernet(string IPMessage)
{
  string TransMessage, protocol, subprotocol;
  int tempsrcip1, tempsrcip2, destip1, destip2, srceth;

  stringstream inputstream;
  stringstream outputstream;

  //  cout << "Host (" << myip1 << "," << myip2 <<  ") received IP Message: " << IPMessage << endl;

  inputstream.str(IPMessage);
  inputstream >> protocol;
  
  //  cout << "Host (" << myip1 << "," << myip2 <<  ") received IP Message: " << inputstream.str() << endl;

  //  cout << "Host (" << myip1 << "," << myip2 << ") Protocol: " << protocol << endl;
  if (protocol == "IP")
    {
      //   cout << "Protocol: " << protocol << endl;
      inputstream >> destip1 >> destip2 >> srcip1 >> srcip2;
      getline(inputstream, TransMessage);
      //    cout << "Transport Message: " << TransMessage << endl;
      if (destip1 == myip1 && destip2 == myip2)
	TransportReceiveFromIP(TransMessage);
    }
  else if (protocol == "ARP")
    {
      inputstream >> subprotocol;
      if (subprotocol == "REQ")
	{
	  inputstream >> destip1 >> destip2 >> tempsrcip1 >> tempsrcip2 >> srceth;
	  ARPTable[srceth].ip1 = tempsrcip1;
	  ARPTable[srceth].ip2 = tempsrcip2;
	  if ((myip1 == destip1) && (myip2 == destip2))
	    {
	      outputstream << "ARP REP " << myip1 << " " << myip2 << " " << myethadd << " " << tempsrcip1 << " " << tempsrcip2 << " " << srceth << endl;
	      //	      cout << "Sending ARP REP: " << outputstream.str() << endl;
	      EthernetReceiveFromIP(outputstream.str(), srceth);
	    }
	}
      else if (subprotocol == "REP")
	{
	  //cout << "Found ARP REP" << endl;
	  inputstream >> tempsrcip1 >> tempsrcip2 >> srceth >> destip1 >> destip2;
	  if ((myip1 == destip1) && (myip2 == destip2))
	    {
	      ARPTable[srceth].ip1 = tempsrcip1;
	      ARPTable[srceth].ip2 = tempsrcip2;
	      FoundEthernetAddress = srceth;
	    }
	}
    }
}

void EthernetReceiveFromBridge(ifstream &MyInputFile)
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
	    position += (EthMessage.length() + 1);
	    inputstream.str(EthMessage);
	    inputstream >> dest >> LastReceivedEthernetAddress;
	    getline(inputstream, IPMessage);
	    //    cout << "Host (" << myip1 << "," << myip2 <<  ") calling IPReceiveFromEthernet with IP Message: " << IPMessage << endl;

	    //    cout << "IPMessage: " << IPMessage << endl;
	    IPReceiveFromEthernet(IPMessage);
	  }
      }
}

void SendMessage(string message, int destip1, int destip2)
{
  int i;
  int timer;
  stringstream temp;
  srcip1 = destip1;
  srcip2 = destip2;
  LastReceivedEthernetAddress = 1;

  for (i = 0; i < (message.length()/5); i++)
    {
      temp << "DA " << "0 0 " << message.substr((i*5), 5) << endl;
      acked = false;
      while (!acked)
	{
	  timer = 0;
	  IPReceiveFromTransport(temp.str());
	  do
	    {
	      inputfile.close();
	      inputfile.open(infilename);
	      inputfile.seekg(position);
	      EthernetReceiveFromBridge(inputfile);
	      this_thread::sleep_for(chrono::seconds(1)); // UPDATE TO 30
	      timer++;
	    } while (!acked && (timer < 30));
	}
      temp.str("");
    }

  if (message.length() % 5 != 0)
    {
      temp << "DA " << "0 0 " << message.substr((i*5), (message.length()-(i*5))) << endl;
      acked = false;
      while (!acked)
	{
	  IPReceiveFromTransport(temp.str());
	  do
	    {
	      inputfile.close();
	      inputfile.open(infilename);
	      inputfile.seekg(position);
	      EthernetReceiveFromBridge(inputfile);
	      this_thread::sleep_for(chrono::seconds(1)); // UPDATE TO 30
	      timer++;
	    } while (!acked && (timer < 30));
	}
      temp.str("");
    }
}

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
    string message;

    //   cout << "Beginning = " << ios_base::beg << endl;

    for (int i = 0; i < MAX_ETHERNET_ADDRESSES; i++)
      {
	ARPTable[i].ip1 = -1;
	ARPTable[i].ip2 = -1;
      }

    //   ARPTable[54].ip1 = 1;
    //   ARPTable[54].ip2 = 3;
    ARPTable[23].ip1 = 1;
    ARPTable[23].ip2 = 1;

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

    /*   if (outputfile.is_open())
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

      inputfile.close();


      //     cout << "Host: (" << myip1 << "," << myip2 << ") Output file name: " << outfilename << endl;
      //	  cout << "Host: (" << myip1 << "," << myip2 << ") Input file name: " << infilename << endl;

    position = 0;

    if (argc == 11) //A message needs to be sent
      {
	inputfile.open(infilename);
	TransportPeriodicTasks();
	//	EthernetReceiveFromBridge(inputfile);
	//	position = inputfile.tellg();
	SendMessage(message, mydestip1, mydestip2);
	inputfile.close();
      }


    //     cout << "Host: (" << myip1 << "," << myip2 << ") (After Send Message check)" << endl;

       while (true)
      {
	  inputfile.open(infilename);
	  // cout << "Host: (" << myip1 << "," << myip2 << ") Seeking to position " << position << endl;

	  inputfile.seekg(position);
	  TransportPeriodicTasks();
	  // cout << "Before EthernetReceiveFromBridge" << endl;
	  EthernetReceiveFromBridge(inputfile);
	  //  cout << "After EthernetReceiveFromBridge" << endl;
	  // inputfile.clear();
	  // cout << "Inside while loop" << endl;
	  this_thread::sleep_for(chrono::seconds(1));
	  //	  position = inputfile.tellg();
	  // cout << "Host: (" << myip1 << "," << myip2 << ") Position is: " << position << endl;
	  inputfile.close();
      }
    
  
    return 0; 
} 
