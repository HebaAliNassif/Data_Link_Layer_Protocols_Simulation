//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "Coordinator.h"
#include "MyMessage_m.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>
using namespace std;

Define_Module(Coordinator);

void Coordinator::initialize()
{
    // Reading the initialization file
    ifstream inFile;
    string line;
    inFile.open("../inputs_samples/coordinator.txt");
    if (!inFile)
    {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    int node_index = -1;
    while (std::getline(inFile, line))
    {
        vector<std::string> fields = stringSplit(line);
        node_index = std::atoi(fields[0].c_str());
        MyMessage_Base *mmsg = new MyMessage_Base((char*)"Initialization");
        mmsg->setMessage_Payload(line.c_str());

        if (node_index >= 2) //Phase one only (2 nodes only not 6)
            break;

        mmsg->setMessage_ID(-1);        // -1 for indicates initialization

        // Sending the information to the intended node.
        send(mmsg, "outs_nodes", node_index);
    }
    inFile.close();
}

void Coordinator::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

vector<string> Coordinator::stringSplit(const string &str)
{
    std::vector<std::string> result;
    std::istringstream iss(str);
    for (std::string s; iss >> s;)
        result.push_back(s);
    return result;
}
