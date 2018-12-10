#include <iostream> 
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

#define MAX_ETHERNET_ADDRESSES 99

using namespace std;

struct ARPEntry
{
    int ip1, ip2, Ethernet;
};

void EthernetReceiveFromBridge(ifstream &MyInputFile)
{
    string MyMessage;

    while (!MyInputFile.eof())
      {
	if (getline (MyInputFile, MyMessage))
	  cout << MyMessage << endl;
      }
}

void GetARP(int tIPadd1, int tIPadd2, int sIPadd1, int sIPadd2, int sEthadd)
{
  // Finish code
}

  
int main(int argc, char** argv) 
{ 
    int myip1, myip2, myethadd, mydrip1, mydrip2, mybridge, mybport, mydestip1, mydestip2;
    string message, outfilename, infilename;

    ARPEntry ARPTable[MAX_ETHERNET_ADDRESSES];

    ofstream outputfile;
    ifstream inputfile;

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

    if (outputfile.is_open())
      cout << "Output file sucessfully opened" << endl;
    else
      cout << "Output file NOT sucessfully opened" << endl;

    if (inputfile.is_open())
      cout << "Input file sucessfully opened" << endl;
    else
      {
	cout << "Input file NOT sucessfully opened initially, creating file" << endl;
	ofstream tempfile;
	tempfile.open(infilename);
	tempfile.close();
	inputfile.open(infilename);
      }

       while (true)
      {
	  EthernetReceiveFromBridge(inputfile);
	  inputfile.clear();
	  // cout << "Inside while loop" << endl;
	  this_thread::sleep_for(chrono::seconds(1));
      }

    /*   cout << "Output file name: " << outfilename << endl;
    cout << "Input file name: " << infilename << endl;
    */
  
    return 0; 
} 
