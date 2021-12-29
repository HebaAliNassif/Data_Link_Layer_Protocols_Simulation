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
enum event_type
{
  frame_arrival,
  cksum_err,
  timeout,
  network_layer_ready,
  ack_timeout
};

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

  std::vector<MyMessage_Base *> timeoutBuffer;
  MyMessage_Base *timeoutEvent;

  // Phase 2
  int ack_expected;
  int next_frame_to_send;
  int frame_expected;
  int too_far;
  //int i;
  MyMessage_Base *received_frame;
  std::vector<std::pair<std::string, std::string>> out_buf;
  std::vector<MyMessage_Base *> in_buf;
  int nr_bufs;
  int max_seq_number;

  std::vector<bool> arrived;
  int nbuffered;
  int oldest_frame;
  //event_type event;

  MyMessage_Base *ack_timer;

protected:
  virtual void initialize();
  virtual void handleMessage(cMessage *msg);
  void sendNextMessage(int msg_id);
  void receiveMessage(MyMessage_Base *msg);
  MyMessage_Base *prepareMessageToSend(string payload, int message_id, double sending_time = 0, bits trailer = 0, int piggybacking_id = 0, int piggybacking = 0);
  void initializeMessageArray(string file_name);
  std::string frameMessage(std::string str);
  vector<string> stringSplit(const string &str);
  void initializeFile();
  // Error Methods
  std::string introduceErrorToPayload(string payload);
  // CRC Methods
  std::bitset<8> calculateCRC(std::string msg);
  string mod2div(string divident, string divisor);
  string xor1(string a, string b);

  void receiveMessageFromPeer(MyMessage_Base *mmsg);

  bool from_network_layer(std::pair<std::string, std::string> *);
  void send_frame(int frame_kind, int frame_num, int frame_exp);
  void to_physical_layer(MyMessage_Base *msg_to_send, std::string error_bits);
  void start_ack_timer();
  void stop_ack_timer();
  void start_timer(int frame_num);
  void stop_timer(int frame_num);
  void inc(int &seq, int operation);

  void from_physical_layer(MyMessage_Base *msg_received);

  bool between(int sf, int si, int sn); // Check if sf <= si < sn
  void printValues();
};

#endif
