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
  ack_timeout,
  data_finished
};
enum print_mode
{
  sender_,
  receiver_,
  timeout_,
  drop_
};

/* Macro inc is expanded in-line: Increment k circularly. */
//#define inc(k) if (k < MAX_SEQ) k = k + 1; else k = 0

class Node : public cSimpleModule
{
  
  std::queue<std::pair<std::string, std::string>> messages; // Queue of msgs that was read from the input file
  ofstream out_file;                                        // Outfile pointer
  std::string file_name;                                    // Outfile name

  int index;                                                // Index of the currend node
   
  // Statistics variables
  int start_transmission_time = -1;
  int transmissions_number = 0;
  int expected_total_transmissions = 0;

  int nr_bufs;                                              // Number of buffers i.e window size (window size is half of max_seq_number)
  int max_seq_number;                                       // Max seq number (2^n - 1)

  // Sender variables
  int ack_expected;                                         // Lower edge of sender's window
  int next_frame_to_send;                                   // Upper edge of sender's window + 1
  std::vector<std::pair<std::string, std::string>> out_buf; // Buffers for the outbound stream
  int nbuffered;                                            // How many output buffers currently used


  // Receiver variables
  int frame_expected;                                       // Lower edge of receiver's window
  int too_far;                                              // Upper edge of receiver's window + 1
  std::vector<MyMessage_Base *> in_buf;                     // Buffers for the inbound stream
  std::vector<bool> arrived;                                // Inbound bit map

  MyMessage_Base * received_frame;
  std::vector<MyMessage_Base *> timeoutBuffer;
  MyMessage_Base *timeoutEvent;
  MyMessage_Base *ack_timer;

  bool peer_finished = false;

  int error_position = -1;

protected:
  virtual void initialize();
  virtual void handleMessage(cMessage *msg);

  // Network methods
  void receiveFrame(MyMessage_Base *mmsg);
  void sendFrame(int frame_kind, int frame_num, int frame_exp);

  bool fromNetworkLayer(std::pair<std::string, std::string> *);
  void toNetworkLayer(MyMessage_Base *msg_received);

  void fromPhysicalLayer(MyMessage_Base *msg_received);
  void toPhysicalLayer(MyMessage_Base *msg_to_send, std::string error_bits);

  void startAckTimer();
  void stopAckTimer();

  void startDataTimer(int frame_num);
  void stopDataTimer(int frame_num);

  void checkEndOfTransmission();
  void finishMsg();

  void enableNetworkLayer();
  void disableNetworkLayer();
  
  // Helper Methods
  vector<string> stringSplit(const string &str);
  std::string stringToBits(std::string str);
  std::string bitsToString(std::string str);
  std::string frameMessage(std::string str);
  bool between(int sf, int si, int sn); // Check if sf <= si < sn

  // File Methods
  void initializeFile();
  void initializeMessageArray(string file_name);

  // Error Methods
  std::string introduceErrorToPayload(string payload);

  // CRC Methods
  std::bitset<8> calculateCRC(std::string msg);
  string mod2div(string divident, string divisor);
  string xor1(string a, string b);

  // Printing Methods
  void printNodeVariables();
  void printStatistics();
  void printNode(MyMessage_Base *msg, int print_mode, bool error);

  // Hamming code Methods
  string generateHammingCode(string payload);
  string correctMsgUsingHammingCode(string payload);
  string removeHammingBits(string payload);

};

#endif
