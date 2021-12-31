//
// Generated file, do not edit! Created by nedtool 5.6 from MyMessage.msg.
//

#ifndef __MYMESSAGE_M_H
#define __MYMESSAGE_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include <bitset>
#include <bits/stdc++.h>
using namespace std;
typedef  std::bitset<8> bits;
// }}

/**
 * Class generated from <tt>MyMessage.msg:23</tt> by nedtool.
 * <pre>
 * packet MyMessage
 * {
 *     \@customize(true);  // see the generated C++ header for more info
 *     int Message_ID;
 *     double Sending_Time;
 *     bits Trailer; //CRC checksum
 *     int Piggybacking;
 *     int Piggybacking_ID;
 *     string Message_Payload;
 *     int event;
 * }
 * </pre>
 *
 * MyMessage_Base is only useful if it gets subclassed, and MyMessage is derived from it.
 * The minimum code to be written for MyMessage is the following:
 *
 * <pre>
 * class MyMessage : public MyMessage_Base
 * {
 *   private:
 *     void copy(const MyMessage& other) { ... }

 *   public:
 *     MyMessage(const char *name=nullptr, short kind=0) : MyMessage_Base(name,kind) {}
 *     MyMessage(const MyMessage& other) : MyMessage_Base(other) {copy(other);}
 *     MyMessage& operator=(const MyMessage& other) {if (this==&other) return *this; MyMessage_Base::operator=(other); copy(other); return *this;}
 *     virtual MyMessage *dup() const override {return new MyMessage(*this);}
 *     // ADD CODE HERE to redefine and implement pure virtual functions from MyMessage_Base
 * };
 * </pre>
 *
 * The following should go into a .cc (.cpp) file:
 *
 * <pre>
 * Register_Class(MyMessage)
 * </pre>
 */
class MyMessage_Base : public ::omnetpp::cPacket
{
  protected:
    int Message_ID;
    double Sending_Time;
    bits Trailer;
    int Piggybacking;
    int Piggybacking_ID;
    ::omnetpp::opp_string Message_Payload;
    int event;

  private:
    void copy(const MyMessage_Base& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const MyMessage_Base&);
    // make assignment operator protected to force the user override it
    MyMessage_Base& operator=(const MyMessage_Base& other);

  public:
  // make constructors protected to avoid instantiation
    MyMessage_Base(const char *name=nullptr, short kind=0);
    MyMessage_Base(const MyMessage_Base& other);

    virtual ~MyMessage_Base();
    virtual MyMessage_Base *dup() const override {return new MyMessage_Base(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getMessage_ID() const;
    virtual void setMessage_ID(int Message_ID);
    virtual double getSending_Time() const;
    virtual void setSending_Time(double Sending_Time);
    virtual bits& getTrailer();
    virtual const bits& getTrailer() const {return const_cast<MyMessage_Base*>(this)->getTrailer();}
    virtual void setTrailer(const bits& Trailer);
    virtual int getPiggybacking() const;
    virtual void setPiggybacking(int Piggybacking);
    virtual int getPiggybacking_ID() const;
    virtual void setPiggybacking_ID(int Piggybacking_ID);
    virtual const char * getMessage_Payload() const;
    virtual void setMessage_Payload(const char * Message_Payload);
    virtual int getEvent() const;
    virtual void setEvent(int event);
};


#endif // ifndef __MYMESSAGE_M_H

