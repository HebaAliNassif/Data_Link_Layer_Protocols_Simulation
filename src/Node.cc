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

#include "Node.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>
#include <stdio.h>
#include <conio.h>
using std::cout; using std::ofstream;
using std::endl; using std::string;
using std::cerr;
using std::fstream;
using namespace std;

Define_Module(Node);

void Node::initialize()
{
    // TODO - Generated method body
}

void Node::handleMessage(cMessage *msg)
{
    MyMessage_Base *received_msg = check_and_cast<MyMessage_Base *>(msg);

    if (received_msg->getMessage_ID() == -1) // Message from coordinator for initialization
    {
        string received_msg_payload = received_msg->getMessage_Payload();
        vector<std::string> fields = stringSplit(received_msg_payload);
        string file_name = fields[1];
        initializeMessageArray(file_name);
        index = received_msg->getArrivalModuleId() - 2;
        expected_total_transmissions = messages.size();

        initializeFile();       // initialize output file

        if (fields.size() > 2) // The pair which will start the simulation, so set the simulation start time
        {
            int start_time = std::atoi(fields[3].c_str());
            start_transmission_time = start_time;
            MyMessage_Base *msg_to_send = prepareMessageToSend("Invoke", -2);
            scheduleAt(start_time, msg_to_send);

        }
    }
    else if (received_msg->getMessage_ID() == -2) // First time to send -- invoked from scheduleAt
    {
        last_msg_id += 1;
        sendNextMessage(last_msg_id);
    }
    else //Message from the pair
    {

        if (timeoutEvent == msg) //timeoutEvent
        {
            cancelEvent(timeoutEvent);
            last_msg_id += 1;
            sendNextMessage(last_msg_id);
        }
        else if (received_msg->getPiggybacking() == 0) // 0 for data only. (i.e Receiver)
        {
            receiveMessage(received_msg);
        }
        else if  (received_msg->getPiggybacking() == -2)    // Drop msg
        {
        }
        else // 1 or -1 ack   -- phase 1 only         (i.e Sender)
        {
            cancelEvent(timeoutEvent);
            last_msg_id += 1;
            sendNextMessage(last_msg_id);
        }
        ///////////////////////
        //Phase 2
        ///////////////////////
        /*
        else if (received_msg->getPiggybacking() == 1) // 1 for ack
        {
        }
        else if (received_msg->getPiggybacking() == 1) // -1 for nack
        {
        }
        */
    }
}
void Node::sendNextMessage(int msg_id)
{
    if (messages.empty())
    {
        double total_time = simTime().dbl() - start_transmission_time;
        string str_1 = "node"+ std::to_string(index) + " end of input file\n";
        string str_2 = "Total transmission time = "+ std::to_string(simTime().dbl() - start_transmission_time) + "\n";
        string str_3 = "Total number of transmissions = "+ std::to_string(transmissions_number) + "\n";
        string str_4 = "The network throughput = "+ std::to_string(expected_total_transmissions/total_time) + "\n";

        EV <<"..............................................."<<endl;
        EV << str_1;
        EV << str_2;
        EV << str_3;
        EV << str_4 << endl<< endl;
        //-------------------------------------------------
        std::cout<<"..............................................."<<endl;
        std::cout << str_1;
        std::cout << str_2;
        std::cout << str_3;
        std::cout << str_4 << endl<< endl;
        //--------------------------------------------------
        out_file.open(file_name, std::ios_base::app);
        out_file<< "...............................................\n";
        out_file<< str_1.c_str();
        out_file<< str_2.c_str();
        out_file<< str_3.c_str();
        out_file<< str_4.c_str();
        out_file.close();

        endSimulation();
    }

    std::pair<std::string, std::string> msg_payload_to_send = messages.front();
    messages.pop();
    std::string framed_payload = frameMessage(msg_payload_to_send.second);
    std::bitset<8> CRC_byte = calculateCRC(framed_payload);

    // Checking for errors
    std::string string_to_print = "node";
    string_to_print += std::to_string(index);
    string_to_print += " sends message with id=";
    string_to_print += std::to_string(msg_id);
    string_to_print += " and content='";
    string_to_print += framed_payload;
    string_to_print += "' at ";
    string_to_print += std::to_string(simTime().dbl());

    if (msg_payload_to_send.first[0] == '1') // Modification
    {
        framed_payload = introduceErrorToPayload(framed_payload);
        string_to_print += " with modification";
    }
    MyMessage_Base *msg_to_send = prepareMessageToSend(framed_payload, msg_id, simTime().dbl(), CRC_byte, 0, 0);
    double time_delay_of_msg = par("prop_delay").doubleValue();

    if (msg_payload_to_send.first[2] == '1') // Duplicated
    {
        double time_difference = 0.01;
        sendDelayed(msg_to_send->dup(), time_delay_of_msg + time_difference, "out_pair");
        transmissions_number++;
    }

    if (msg_payload_to_send.first[3] == '1') // Delay
    {
        time_delay_of_msg += par("delay_amount").doubleValue();
    }

    if (msg_payload_to_send.first[1] == '1') // Loss. Do not send
    {
    }
    else // Send
    {
        sendDelayed(msg_to_send, time_delay_of_msg, "out_pair");
    }
    string_to_print += "\n";
    EV << string_to_print;
    std::cout << string_to_print;
    out_file.open(file_name, std::ios_base::app);
    out_file << string_to_print;
    out_file.close();
    transmissions_number++;

    // Setting the timeout event
    timeoutEvent = new MyMessage_Base("timeout");

    scheduleAt(simTime() + par("timeout"), timeoutEvent);
}
void Node::receiveMessage(MyMessage_Base *received_msg)
{
    // Double msg check
    std::string payload = received_msg->getMessage_Payload();
    std::bitset<8> checkbits = received_msg->getTrailer();
    std::bitset<8> reminderRec = calculateCRC(payload + (char)checkbits.to_ulong());

    std::string string_to_print = "node";
    string_to_print += std::to_string(index);
    string_to_print += " received message with id=";
    string_to_print += std::to_string(received_msg->getMessage_ID());
    string_to_print += " and content='";
    string_to_print += payload;
    string_to_print += "' at ";
    string_to_print += std::to_string(simTime().dbl());

    // CRC check
    if (reminderRec != std::bitset<8>("00000000")) // No error. Send positive ack
    {
        string_to_print += " with modification";
    }
    string_to_print += "\n";
    EV << string_to_print;
    std::cout << string_to_print;
    out_file.open(file_name, std::ios_base::app);
    out_file << string_to_print.c_str();
    out_file.close();
    if (last_msg_id + 1 <= received_msg->getMessage_ID())
    {
        // CRC check
        if (reminderRec == std::bitset<8>("00000000")) // No error. Send positive ack
        {
            MyMessage_Base *msg_to_send = prepareMessageToSend("Ack", received_msg->getMessage_ID(), simTime().dbl(), 0, 0, 1);
            sendDelayed(msg_to_send, par("prop_delay"), "out_pair");
        }
        else // Error detected. Send negative ack
        {
            MyMessage_Base *msg_to_send = prepareMessageToSend("NAck", received_msg->getMessage_ID(), simTime().dbl(), 0, 0, -1);
            sendDelayed(msg_to_send, par("prop_delay"), "out_pair");
        }
        last_msg_id = received_msg->getMessage_ID();
    }
    else
    {

        std::string string_to_print = "node";
        string_to_print += std::to_string(index);
        string_to_print += " drops message with id=";
        string_to_print += std::to_string(received_msg->getMessage_ID());
        string_to_print += "\n";
        EV << string_to_print;
        std::cout << string_to_print;
        out_file.open(file_name, std::ios_base::app);
        out_file << string_to_print.c_str();
        out_file.close();
        MyMessage_Base *msg_to_send = prepareMessageToSend("Drop msg with id= " + std::to_string(received_msg->getMessage_ID()), received_msg->getMessage_ID(), simTime().dbl(), 0, 0, -2);
        sendDelayed(msg_to_send, par("prop_delay"), "out_pair");
    }
}
MyMessage_Base *Node::prepareMessageToSend(string payload, int message_id, double sending_time, bits trailer, int piggybacking_id, int piggybacking)
{
    MyMessage_Base *msg_to_send = new MyMessage_Base(payload.c_str());
    msg_to_send->setMessage_ID(message_id);
    msg_to_send->setMessage_Payload(payload.c_str());
    msg_to_send->setPiggybacking_ID(piggybacking_id);
    msg_to_send->setPiggybacking(piggybacking);
    msg_to_send->setSending_Time(sending_time);
    msg_to_send->setTrailer(trailer);
    return msg_to_send;
}

void Node::initializeMessageArray(string file_name)
{
    ifstream inFile;
    string line;
    inFile.open("../inputs_samples/" + file_name);
    if (!inFile)
    {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    string msg_content, error;
    while (std::getline(inFile, line))
    {
        error = line.substr(0, 4);
        msg_content = line.substr(5);
        messages.push(make_pair(error, msg_content));
    }
    inFile.close();
}

std::string Node::frameMessage(std::string str)
{
    std::string framedMsg = "";
    framedMsg += "$";
    for (std::string::size_type i = 0; i < str.size(); i++)
    {

        if (str[i] == '$' || str[i] == '/')
        {
            framedMsg += "/";
        }
        framedMsg += str[i];
    }
    framedMsg += "$";
    return framedMsg;
}

vector<string> Node::stringSplit(const string &str)
{
    std::vector<std::string> result;
    std::istringstream iss(str);
    for (std::string s; iss >> s;)
        result.push_back(s);
    return result;
}
void Node::initializeFile()
{
    file_name = "../output_examples/";
    if(index%2==0)
    {
        file_name += "pair"+std::to_string(index)+std::to_string(index+1)+".txt";
    }
    else
    {
        file_name += "pair"+std::to_string(index-1)+std::to_string(index)+".txt";
    }
    out_file.open(file_name);
    out_file.close();
}
std::string Node::introduceErrorToPayload(string payload)
{
    int char_count = payload.size() / sizeof(char);
    std::vector<std::bitset<8>> vec;
    for (int i = 0; i < char_count; i++)
    {
        vec.push_back(payload[i]);
    }
    int index_of_char = intuniform(0, char_count - 1);
    int index_of_bit = intuniform(0, 7);
    vec[index_of_char][index_of_bit] = vec[index_of_char][index_of_bit] xor 1;
    std::string final_string = "";
    for (std::vector<std::bitset<8>>::iterator it = vec.begin(); it != vec.end(); ++it)
    {
        final_string += (char)(*it).to_ulong();
    }
    return final_string;
}
// Returns XOR of 'a' and 'b'
// (both of same length)
string Node::xor1(string a, string b)
{

    // Initialize result
    string result = "";

    int n = b.length();

    // Traverse all bits, if bits are
    // same, then XOR is 0, else 1
    for (int i = 1; i < n; i++)
    {
        if (a[i] == b[i])
            result += "0";
        else
            result += "1";
    }
    return result;
}

// Performs Modulo-2 division
string Node::mod2div(string divident, string divisor)
{

    // Number of bits to be XORed at a time.
    int pick = divisor.length();

    // Slicing the divident to appropriate
    // length for particular step
    string tmp = divident.substr(0, pick);

    int n = divident.length();

    while (pick < n)
    {
        if (tmp[0] == '1')

            // Replace the divident by the result
            // of XOR and pull 1 bit down
            tmp = xor1(divisor, tmp) + divident[pick];
        else

            // If leftmost bit is '0'.
            // If the leftmost bit of the dividend (or the
            // part used in each step) is 0, the step cannot
            // use the regular divisor; we need to use an
            // all-0s divisor.
            tmp = xor1(std::string(pick, '0'), tmp) + divident[pick];

        // Increment pick to move further
        pick += 1;
    }

    // For the last n bits, we have to carry it out
    // normally as increased value of pick will cause
    // Index Out of Bounds.
    if (tmp[0] == '1')
        tmp = xor1(divisor, tmp);
    else
        tmp = xor1(std::string(pick, '0'), tmp);

    return tmp;
}

std::bitset<8> Node::calculateCRC(std::string msg)
{
    int char_count = msg.size() / sizeof(char);
    string data_in_bits = "";
    for (int i = 0; i < char_count; i++)
    {
        std::bitset<8> bits = msg[i];
        data_in_bits += bits.to_string();
    }

    string key = "100000001";
    int l_key = key.length();
    string appended_data = (data_in_bits + std::string(l_key - 1, '0'));

    string remainder = mod2div(appended_data, key);
    std::bitset<8> remainderBits = std::bitset<8>(remainder);

    return remainderBits;
}
