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
using std::cerr;
using std::cout;
using std::endl;
using std::fstream;
using std::ofstream;
using std::string;
using namespace std;
bool no_nak = true;
Define_Module(Node);
void Node::initialize()
{
    // TODO - Generated method body
    nr_bufs = par("window_size").intValue();
    max_seq_number = 2 * nr_bufs - 1;
    ack_expected = 0;
    next_frame_to_send = 0;
    frame_expected = 0;
    too_far = nr_bufs;
    nbuffered = 0;
    for (int i = 0; i < nr_bufs; i++)
    {
        arrived.push_back(false);
        out_buf.push_back(std::pair<std::string, std::string>());
        in_buf.push_back(new MyMessage_Base());
        timeoutBuffer.push_back(nullptr);
    }
    timeoutEvent = new MyMessage_Base("timeout");
    timeoutEvent->setEvent(timeout);
    ack_timer = nullptr;
    //vector<int> msgBit = {1, 0, 1, 1, 0, 0, 1};
    //findHammingCode(msgBit);
    //std::cout<<endl<<"Hamming: "<<hamming("1011001")<<endl;
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

        initializeFile(); // initialize output file

        if (fields.size() > 2) // The pair which will start the simulation, so set the simulation start time
        {
            int start_time = std::atoi(fields[3].c_str());
            start_transmission_time = start_time;
            MyMessage_Base *msg_to_send = new MyMessage_Base();
            msg_to_send->setEvent(network_layer_ready);
            msg_to_send->setMessage_ID(0);
            scheduleAt(start_time, msg_to_send);
        }
    }
    else
    {
        receiveFrame(received_msg);
    }
}

void Node::receiveFrame(MyMessage_Base *mmsg)
{
    //EV << "On Enter: " << endl;
    //printNodeVariables();
    switch (mmsg->getEvent())
    {
    case frame_arrival:
        printNode(mmsg, receiver_, false);
        fromPhysicalLayer(mmsg);
        if (received_frame->getPiggybacking() == 0) // data
        {
            if (received_frame->getMessage_ID() != frame_expected && no_nak)
            {
                // We did NOT receive the lower end of receive window => Send NAK immediately
                sendFrame(-1, 0, frame_expected); // type Nak
            }
            else // Restart ACK timer every time we receive out of order frame (to be able to send cumulative ack)
            {
                startAckTimer();
            }
            if (between(frame_expected, received_frame->getMessage_ID(), too_far) && arrived[received_frame->getMessage_ID() % nr_bufs] == false) // Check if the msg is within the window and didn't arrive before
            {
                arrived[received_frame->getMessage_ID() % nr_bufs] = true; // Mark as arrived
                in_buf[received_frame->getMessage_ID() % nr_bufs] = mmsg;  // Add to the input buffer
                while (arrived[frame_expected % nr_bufs])                  // Deliver contiguous packet sequence to network layer starting from bottom of receiver window
                {
                    toNetworkLayer(received_frame);
                    no_nak = true; // Allow sending NAK because we send NAK for frame_expected and frame_expected will be incremented soon
                    arrived[frame_expected % nr_bufs] = false;
                    // Window Sliding
                    frame_expected++; // Advance lower edge of receiver's window
                    too_far++;        // Advance upper edge of receiver's window
                    startAckTimer();  // To see if a separate ack is needed
                }
            }
            else if (arrived[received_frame->getMessage_ID() % nr_bufs] == true)
            {
                printNode(received_frame, drop_, false);
            }
        }

        if (received_frame->getPiggybacking() == -1)
        {
            int resend_seq_num = (received_frame->getPiggybacking_ID() + 1) % (max_seq_number + 1);
            if (between(ack_expected, resend_seq_num, next_frame_to_send))
            {
                EV << "Inside resend Msg. \tresend_seq_num: " << resend_seq_num << endl;
                sendFrame(0, resend_seq_num, frame_expected); //resend frame
            }
        }

        while (between(ack_expected % (max_seq_number + 1), received_frame->getPiggybacking_ID(), next_frame_to_send % (max_seq_number + 1)))
        {
            nbuffered--;
            stopDataTimer(ack_expected);
            ack_expected++;
        }
        break;
    case cksum_err:
        if (no_nak)
        {
            sendFrame(-1, 0, frame_expected);
        }
        break;
    case timeout:
        printNode(mmsg, timeout_, false);
        sendFrame(0, mmsg->getMessage_ID(), frame_expected);
        break;
    case network_layer_ready:
        if (nbuffered < nr_bufs)
        {
            if (fromNetworkLayer(&out_buf[next_frame_to_send % nr_bufs]))
            {
                nbuffered++;
                sendFrame(0, next_frame_to_send, frame_expected); // type data
                next_frame_to_send++;
            }
        }
        break;
    case ack_timeout:
        sendFrame(1, 0, frame_expected);
        break;
    case data_finished:
        peer_finished = true;
        break;
    default:
        std::cout << "Sad" << endl;
    }
    if (nbuffered < nr_bufs && messages.empty() == false)
    {
        MyMessage_Base *msg_to_send = new MyMessage_Base();
        msg_to_send->setEvent(network_layer_ready);
        scheduleAt(simTime() + par("interval_between_msgs").doubleValue(), msg_to_send);
    }
    else
    {
    }

    //EV << "On Leave: " << endl;
    //printNodeVariables();

    if (ack_expected == expected_total_transmissions && peer_finished == false)
    {
        EV << "Here" << endl;
        finishMsg();
    }
    else if (ack_expected == expected_total_transmissions && peer_finished == true)
    {
        printStatistics();
        endSimulation();
    }
}

void Node::sendFrame(int frame_kind, int frame_num, int frame_exp)
{
    std::string payload;
    std::bitset<8> CRC_byte;
    if (frame_kind == 0) //if data, then set the payload string
    {
        payload = frameMessage(out_buf[frame_num % nr_bufs].second);

        //payload = getHammingCode(stringInBits(payload));

        CRC_byte = calculateCRC(payload);

        if (out_buf[frame_num % nr_bufs].first[0] == '1')
        {
            payload = introduceErrorToPayload(payload);
        }
    }
    MyMessage_Base *msg_to_send = new MyMessage_Base(payload.c_str());

    msg_to_send->setPiggybacking(frame_kind); /*frame_kind\piggybacking == data, ack, or nak*/

    if (frame_kind == 0) //if data, then set the payload string
    {
        msg_to_send->setMessage_Payload(payload.c_str());
        msg_to_send->setTrailer(CRC_byte);
    }

    msg_to_send->setMessage_ID(frame_num);

    msg_to_send->setPiggybacking_ID((frame_exp + max_seq_number) % (max_seq_number + 1)); /*msg_to_send.//  ggybacking_ID contains the sequence number of the last received frame in the contiguous sequence of frames*/
    /*msg_to_send.Piggybacking_ID === circular(frame_expected-1, MAX_SEQ) */

    if (frame_kind == -1) //nack
    {
        /*This way if the next packet is NOT frame_expected, we will start the ACK timer instead of sending a NAK   e //more time */
        no_nak = false;
    }

    msg_to_send->setEvent(frame_arrival);

    toPhysicalLayer(msg_to_send, out_buf[frame_num % nr_bufs].first); // transmit the frame

    if (frame_kind == 0) //data
    {
        if (out_buf[frame_num % nr_bufs].first[0] == '1')
            printNode(msg_to_send, sender_, true);
        else
            printNode(msg_to_send, sender_, false);
        out_buf[frame_num % nr_bufs].first = "0000";
        startDataTimer(frame_num);
    }
    else
    {
        printNode(msg_to_send, sender_, false);
    }
    stopAckTimer(); // No need to send separate ack, i.e already sent
    EV << "Sender: \nEvent: " << msg_to_send->getEvent() << "\t Message ID: " << msg_to_send->getMessage_ID() << "\t Payload: " << msg_to_send->getMessage_Payload() << "\t Piggybacking: " << msg_to_send->getPiggybacking() << "\t Piggybacking ID: " << msg_to_send->getPiggybacking_ID() << "\t Trailer: " << msg_to_send->getTrailer() << endl;
}

bool Node::fromNetworkLayer(std::pair<std::string, std::string> *msg)
{
    if (messages.empty())
    {
        return false;
    }
    msg->first = (messages.front()).first;
    msg->second = (messages.front()).second;
    messages.pop();
    return true;
}

void Node::toNetworkLayer(MyMessage_Base *msg_received)
{
    return;
}

void Node::fromPhysicalLayer(MyMessage_Base *msg_received)
{
    received_frame = msg_received->dup();
}

void Node::toPhysicalLayer(MyMessage_Base *msg_to_send, std::string error_bits)
{
    double time_delay_of_msg = par("prop_delay").doubleValue();

    if (error_bits[2] == '1') // Duplicated
    {
        double time_difference = 0.01;
        sendDelayed(msg_to_send->dup(), time_delay_of_msg + time_difference, "out_pair");
        transmissions_number++;
    }

    if (error_bits[3] == '1') // Delay
    {
        time_delay_of_msg += par("delay_amount").doubleValue();
    }

    if (error_bits[1] == '1') // Loss. Do not send
    {
    }
    else // Send
    {
        sendDelayed(msg_to_send, par("prop_delay"), "out_pair");
    }
}

void Node::startAckTimer()
{
    EV << "Restart ack timer" << endl;
    stopAckTimer();
    EV << "Start ack timer" << endl;
    ack_timer = new MyMessage_Base();
    ack_timer->setEvent(ack_timeout);
    scheduleAt(simTime() + par("timeout_ack").doubleValue(), ack_timer);
}

void Node::stopAckTimer()
{
    if (ack_timer != nullptr)
    {
        EV << "Stop current ack timer" << endl;
        cancelAndDelete(ack_timer);
    }
    ack_timer = nullptr;
}

void Node::startDataTimer(int frame_num)
{
    EV << "Start data timer for msg with frame number = " << frame_num << endl;

    MyMessage_Base *currentTimeout = new MyMessage_Base("timeout");

    currentTimeout->setEvent(timeout);
    currentTimeout->setMessage_ID(frame_num);

    scheduleAt(simTime() + par("timeout").doubleValue(), currentTimeout);

    timeoutBuffer[frame_num % nr_bufs] = currentTimeout;
}

void Node::stopDataTimer(int frame_num)
{
    EV << "Stop data timer for msg with frame number = " << frame_num << endl;
    cancelAndDelete(timeoutBuffer[frame_num % nr_bufs]);
}

void Node::finishMsg()
{
    MyMessage_Base *msg_to_send = new MyMessage_Base("Finished");
    msg_to_send->setEvent(data_finished);
    sendDelayed(msg_to_send, 0.01, "out_pair");
}

void Node::inc(int &seq, int op)
{
    switch (op) // 0:next_frame_to_send , 1:ack_expected , 2:frame_expected
    {
    case 0:
        if (seq < par("MAX_SEQ").intValue())
            seq = (seq + 1);

        break;
    case 1:
        seq = (seq + 1); //% (buffer.size());
        break;
    case 2:
        seq = (seq + 1); // % (buffer.size());
        break;
    }
}

vector<string> Node::stringSplit(const string &str)
{
    std::vector<std::string> result;
    std::istringstream iss(str);
    for (std::string s; iss >> s;)
        result.push_back(s);
    return result;
}

std::string Node::stringInBits(std::string str)
{
    int char_count = str.size() / sizeof(char);
    string data_in_bits = "";
    for (int i = 0; i < char_count; i++)
    {
        std::bitset<8> bits = str[i];
        data_in_bits += bits.to_string();
    }

    return data_in_bits;
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

bool Node::between(int sf, int si, int sn)
{
    return (((sf <= si) && (si < sn)) || ((sn < sf) && (sf <= si)) || ((si < sn) && (sn < sf)));
}

void Node::initializeFile()
{
    file_name = "../output_examples/";
    if (index % 2 == 0)
    {
        file_name += "pair" + std::to_string(index) + std::to_string(index + 1) + ".txt";
    }
    else
    {
        file_name += "pair" + std::to_string(index - 1) + std::to_string(index) + ".txt";
    }
    out_file.open(file_name);
    out_file.close();
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

void Node::printNodeVariables()
{
    EV << "---------------------------------------------" << endl;
    EV << "Node " << index << endl
       << "------------------" << endl;

    EV << "Sender Side Variables: " << endl;
    EV << "ack_expected: " << ack_expected << "\t next_frame_to_send: " << next_frame_to_send << endl;
    EV << "nbuffered: " << nbuffered << endl
       << "out_buf: " << endl;
    for (int j = 0; j < nbuffered; j++)
        EV << out_buf[j].second << "\t";
    EV << endl;

    EV << "Receiver Side Variables: " << endl;
    EV << "frame_expected: " << frame_expected << "\t too_far: " << too_far << endl
       << "in_buf: " << endl;
    for (int j = 0; j < nr_bufs; j++)
        EV << in_buf[j]->getMessage_Payload() << "\t";
    EV << "arrived: " << endl;
    for (int j = 0; j < nr_bufs; j++)
        EV << arrived[j] << "\t";
    EV << endl;
    EV << "---------------------------------------------" << endl;
}

void Node::printStatistics()
{
    double total_time = simTime().dbl() - start_transmission_time;
    string str_1;
    if(index%2==0)
    {
        str_1 = "node" + std::to_string(index) + " end of input file\n";
        str_1 += "node" + std::to_string(index+1) + " end of input file\n";
    }
    else
    {
        str_1 = "node" + std::to_string(index-1) + " end of input file\n";
        str_1 += "node" + std::to_string(index) + " end of input file\n";
    }
    string str_2 = "Total transmission time = " + std::to_string(simTime().dbl() - start_transmission_time) + "\n";
    string str_3 = "Total number of transmissions = " + std::to_string(transmissions_number) + "\n";
    string str_4 = "The network throughput = " + std::to_string(expected_total_transmissions / total_time) + "\n";

    EV << "..............................................." << endl;
    EV << str_1;
    EV << str_2;
    EV << str_3;
    EV << str_4 << endl
       << endl;
    //-------------------------------------------------
    std::cout << "..............................................." << endl;
    std::cout << str_1;
    std::cout << str_2;
    std::cout << str_3;
    std::cout << str_4 << endl
              << endl;
    //--------------------------------------------------
    out_file.open(file_name, std::ios_base::app);
    out_file << "...............................................\n";
    out_file << str_1.c_str();
    out_file << str_2.c_str();
    out_file << str_3.c_str();
    out_file << str_4.c_str();
    out_file.close();

    endSimulation();
}

void Node::printNode(MyMessage_Base *msg, int print_mode, bool error)
{
    std::string string_to_print = "";
    switch (print_mode)
    {
    case (receiver_):
        string_to_print = "node";
        string_to_print += std::to_string(index);
        string_to_print += " received message with id=";
        string_to_print += std::to_string(msg->getMessage_ID());
        string_to_print += " and content='";
        string_to_print += msg->getMessage_Payload();
        string_to_print += "' at ";
        string_to_print += std::to_string(simTime().dbl());
        if (error)
        {
            string_to_print += " with modification";
        }
        if (msg->getPiggybacking() == -1)
        {
            string_to_print += ", and NACK number ";
            string_to_print += std::to_string(msg->getPiggybacking_ID());
        }
        else
        {
            string_to_print += ", and piggybacking Ack number ";
            string_to_print += std::to_string(msg->getPiggybacking_ID());
        }
        break;
    case (sender_):
        string_to_print = "node";
        string_to_print += std::to_string(index);
        string_to_print += " sends message with id=";
        string_to_print += std::to_string(msg->getMessage_ID());
        string_to_print += " and content='";
        string_to_print += msg->getMessage_Payload();
        string_to_print += "' at ";
        string_to_print += std::to_string(simTime().dbl());
        if (error)
        {
            string_to_print += " with modification";
        }
        if (msg->getPiggybacking() == -1)
        {
            string_to_print += ", and NACK number ";
            string_to_print += std::to_string(msg->getPiggybacking_ID());
        }
        else
        {
            string_to_print += ", and piggybacking Ack number ";
            string_to_print += std::to_string(msg->getPiggybacking_ID());
        }
        break;
    case (timeout_):
        string_to_print = "node";
        string_to_print += std::to_string(index);
        string_to_print += " drops message with id=";
        string_to_print += std::to_string(msg->getMessage_ID());
        break;
    case (drop_):
        string_to_print = "node";
        string_to_print += std::to_string(index);
        string_to_print += " timeout for message id=";
        string_to_print += std::to_string(msg->getMessage_ID());
        string_to_print += " at t=";
        string_to_print += std::to_string(simTime().dbl());
        break;
    default:
        break;
    }
    EV << string_to_print << endl
       << endl;
    //-------------------------------------------------
    std::cout << string_to_print << endl
              << endl;
    //--------------------------------------------------
    out_file.open(file_name, std::ios_base::app);
    out_file << string_to_print.c_str() << endl;
    out_file.close();
}

/*
std::string Node::getHammingCode(std::string msgBits)
{

    // Message bit size
    int m = msgBits.size();

    // r is the number of redundant bits
    int r = 0;

    // Find no. of redundant bits
    while (pow(2, r) < (m + r + 1))
    {
        r++;
    }

    // Stores the Hamming Code
    int hammingCode[r + m];

    // Find positions of redundant bits
    for (int i = 0; i < r; ++i)
    {
        // Placing -1 at redundant bits place to identify it later
        hammingCode[int(pow(2, i) - 1)] = -1;
    }

    int j = 0;

    // Iterate to update the code
    for (int i = 0; i < (r + m); i++)
    {
        // Placing msgBits where -1 is absent
        if (hammingCode[i] != -1)
        {
            hammingCode[i] = msgBits[j];
            j++;
        }
    }

    for (int i = 0; i < (r + m); i++)
    {
        // If current bit is not redundant bit then continue
        if (hammingCode[i] != -1)
            continue;

        int x = log2(i + 1);
        int parity = 0;

        // Find msg bits containing set bit at x'th position
        for (int j = i + 2; j <= (r + m); ++j)
        {
            if (j & (1 << x))
            {
                if (hammingCode[j - 1] == 1)
                {
                    parity++;
                }
            }
        }

        // Generating hamming code for even parity
        if (parity % 2 == 0)
        {
            hammingCode[i] = 0;
        }
        else
        {
            hammingCode[i] = 1;
        }
    }

    std::stringstream temp;
    for (int i = 1; i <= m + r; i++)
    {
        temp << hammingCode[i];
    }
    return temp.str();
}
*/
vector<int> Node::generateHammingCode(vector<int> msgBits, int m, int r)
{
    // Stores the Hamming Code
    vector<int> hammingCode(r + m);

    // Find positions of redundant bits
    for (int i = 0; i < r; ++i)
    {

        // Placing -1 at redundant bits
        // place to identify it later
        hammingCode[pow(2, i) - 1] = -1;
    }

    int j = 0;

    // Iterate to update the code
    for (int i = 0; i < (r + m); i++)
    {

        // Placing msgBits where -1 is
        // absent i.e., except redundant
        // bits all positions are msgBits
        if (hammingCode[i] != -1)
        {
            hammingCode[i] = msgBits[j];
            j++;
        }
    }

    for (int i = 0; i < (r + m); i++)
    {

        // If current bit is not redundant
        // bit then continue
        if (hammingCode[i] != -1)
            continue;

        int x = log2(i + 1);
        int one_count = 0;

        // Find msg bits containing
        // set bit at x'th position
        for (int j = i + 2;
             j <= (r + m); ++j)
        {

            if (j & (1 << x))
            {
                if (hammingCode[j - 1] == 1)
                {
                    one_count++;
                }
            }
        }

        // Generating hamming code for
        // even parity
        if (one_count % 2 == 0)
        {
            hammingCode[i] = 0;
        }
        else
        {
            hammingCode[i] = 1;
        }
    }

    // Return the generated code
    return hammingCode;
}

// Function to find the hamming code
// of the given message bit msgBit[]
void Node::findHammingCode(vector<int> &msgBit)
{

    // Message bit size
    int m = msgBit.size();

    // r is the number of redundant bits
    int r = 1;

    // Find no. of redundant bits
    while (pow(2, r) < (m + r + 1))
    {
        r++;
    }

    // Generating Code
    vector<int> ans = generateHammingCode(msgBit, m, r);

    // Print the code
    cout << "Message bits are: ";
    for (int i = 0; i < msgBit.size(); i++)
        cout << msgBit[i] << " ";

    cout << "\nHamming code is: ";
    for (int i = 0; i < ans.size(); i++)
        cout << ans[i] << " ";
}

string Node::hamming(string payload)
{

    cout << "payload size is  " << payload.size() << "\n";
    int r = 0;
    int parity;
    while (((payload.size()) + 1 + r) >= pow(2, r))
    { ////finding the redundant bits
        r++;
    }
    cout << " r is " << r << "\n";

    int Pbits_position[r]; //// identifying the p bits location
    for (int i = 0; i < r; i++)
    {

        Pbits_position[i] = pow(2, i);
    }

    int msg_plus_parity[payload.size() + r];
    int j = 0;
    int k = 0;
    for (int i = 1; i <= payload.size() + r; i++)
    { ///compose a message of data + parity (parity still not calculated)
        if (i == Pbits_position[j])
        {
            msg_plus_parity[i] = -1;
            j++;
        }
        else
        {
            msg_plus_parity[i] = (int)payload[k] - 48;
            k++;
        }
    }

    k = 0;
    int x, min, max = 0;
    //finding parity bit
    for (int i = 1; i <= payload.size() + r; i = pow(2, k))
    {
        k++;
        parity = 0;
        j = i;
        x = i;
        min = 1;
        max = i;
        while (j <= payload.size() + r)
        {
            for (x = j; max >= min && x <= payload.size() + r; min++, x++)
            {
                if (msg_plus_parity[x] == 1)
                    parity = parity + 1;
                ;
            }
            j = x + i;
            min = 1;
        }

        //checking for even parity
        if (parity % 2 == 0)
        {
            msg_plus_parity[i] = 0;
        }
        else
        {
            msg_plus_parity[i] = 1;
        }
    }

    std::stringstream temp;
    for (int i = 1; i <= payload.size() + r; i++)
    {
        temp << msg_plus_parity[i];
    }
    return temp.str();
}

void Node::sendNextMessage(int msg_id)
{
    if (messages.empty())
    {
        double total_time = simTime().dbl() - start_transmission_time;
        string str_1 = "node" + std::to_string(index) + " end of input file\n";
        string str_2 = "Total transmission time = " + std::to_string(simTime().dbl() - start_transmission_time) + "\n";
        string str_3 = "Total number of transmissions = " + std::to_string(transmissions_number) + "\n";
        string str_4 = "The network throughput = " + std::to_string(expected_total_transmissions / total_time) + "\n";

        EV << "..............................................." << endl;
        EV << str_1;
        EV << str_2;
        EV << str_3;
        EV << str_4 << endl
           << endl;
        //-------------------------------------------------
        std::cout << "..............................................." << endl;
        std::cout << str_1;
        std::cout << str_2;
        std::cout << str_3;
        std::cout << str_4 << endl
                  << endl;
        //--------------------------------------------------
        out_file.open(file_name, std::ios_base::app);
        out_file << "...............................................\n";
        out_file << str_1.c_str();
        out_file << str_2.c_str();
        out_file << str_3.c_str();
        out_file << str_4.c_str();
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
