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

#ifndef __NETWORKSPROJECT_V2_NODE_H_
#define __NETWORKSPROJECT_V2_NODE_H_

#include <omnetpp.h>
#include <vector>
#include <cstring>
#include "MyMessage_m.h"
using namespace std;
using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{
  std::queue<std::pair<std::string, std::string>> messages;
  ofstream out_file;
  std::string file_name;

  int index;
  int last_msg_id = -1;

  int start_transmission_time = -1;
  int transmissions_number = 0;
  int expected_total_transmissions = 0;
  MyMessage_Base*  timeoutEvent;
  
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void sendNextMessage(int msg_id);
    void receiveMessage(MyMessage_Base *msg);
    MyMessage_Base * prepareMessageToSend(string payload, int message_id, double sending_time = 0, bits trailer = 0, int piggybacking_id = 0, int piggybacking = 0);
    void initializeMessageArray(string file_name);
    std::string frameMessage(std::string str);
    vector<string> stringSplit(const string& str);
    void initializeFile();
    // Error Methods
    std::string introduceErrorToPayload(string payload);
    // CRC Methods
    std::bitset<8> calculateCRC(std::string msg);
    string mod2div(string divident, string divisor);
    string xor1(string a, string b);
};

#endif
