//
// Generated file, do not edit! Created by nedtool 5.6 from MyMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "MyMessage_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp


// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

MyMessage_Base::MyMessage_Base(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->Message_ID = 0;
    this->Sending_Time = 0;
    this->Piggybacking = 0;
    this->Piggybacking_ID = 0;
    this->event = 0;
}

MyMessage_Base::MyMessage_Base(const MyMessage_Base& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

MyMessage_Base::~MyMessage_Base()
{
}

MyMessage_Base& MyMessage_Base::operator=(const MyMessage_Base& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void MyMessage_Base::copy(const MyMessage_Base& other)
{
    this->Message_ID = other.Message_ID;
    this->Sending_Time = other.Sending_Time;
    this->Trailer = other.Trailer;
    this->Piggybacking = other.Piggybacking;
    this->Piggybacking_ID = other.Piggybacking_ID;
    this->Message_Payload = other.Message_Payload;
    this->event = other.event;
}

void MyMessage_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->Message_ID);
    doParsimPacking(b,this->Sending_Time);
    doParsimPacking(b,this->Trailer);
    doParsimPacking(b,this->Piggybacking);
    doParsimPacking(b,this->Piggybacking_ID);
    doParsimPacking(b,this->Message_Payload);
    doParsimPacking(b,this->event);
}

void MyMessage_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->Message_ID);
    doParsimUnpacking(b,this->Sending_Time);
    doParsimUnpacking(b,this->Trailer);
    doParsimUnpacking(b,this->Piggybacking);
    doParsimUnpacking(b,this->Piggybacking_ID);
    doParsimUnpacking(b,this->Message_Payload);
    doParsimUnpacking(b,this->event);
}

int MyMessage_Base::getMessage_ID() const
{
    return this->Message_ID;
}

void MyMessage_Base::setMessage_ID(int Message_ID)
{
    this->Message_ID = Message_ID;
}

double MyMessage_Base::getSending_Time() const
{
    return this->Sending_Time;
}

void MyMessage_Base::setSending_Time(double Sending_Time)
{
    this->Sending_Time = Sending_Time;
}

bits& MyMessage_Base::getTrailer()
{
    return this->Trailer;
}

void MyMessage_Base::setTrailer(const bits& Trailer)
{
    this->Trailer = Trailer;
}

int MyMessage_Base::getPiggybacking() const
{
    return this->Piggybacking;
}

void MyMessage_Base::setPiggybacking(int Piggybacking)
{
    this->Piggybacking = Piggybacking;
}

int MyMessage_Base::getPiggybacking_ID() const
{
    return this->Piggybacking_ID;
}

void MyMessage_Base::setPiggybacking_ID(int Piggybacking_ID)
{
    this->Piggybacking_ID = Piggybacking_ID;
}

const char * MyMessage_Base::getMessage_Payload() const
{
    return this->Message_Payload.c_str();
}

void MyMessage_Base::setMessage_Payload(const char * Message_Payload)
{
    this->Message_Payload = Message_Payload;
}

int MyMessage_Base::getEvent() const
{
    return this->event;
}

void MyMessage_Base::setEvent(int event)
{
    this->event = event;
}

class MyMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    MyMessageDescriptor();
    virtual ~MyMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(MyMessageDescriptor)

MyMessageDescriptor::MyMessageDescriptor() : omnetpp::cClassDescriptor("MyMessage", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

MyMessageDescriptor::~MyMessageDescriptor()
{
    delete[] propertynames;
}

bool MyMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MyMessage_Base *>(obj)!=nullptr;
}

const char **MyMessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "customize",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MyMessageDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"customize")) return "true";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MyMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 7+basedesc->getFieldCount() : 7;
}

unsigned int MyMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<7) ? fieldTypeFlags[field] : 0;
}

const char *MyMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "Message_ID",
        "Sending_Time",
        "Trailer",
        "Piggybacking",
        "Piggybacking_ID",
        "Message_Payload",
        "event",
    };
    return (field>=0 && field<7) ? fieldNames[field] : nullptr;
}

int MyMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='M' && strcmp(fieldName, "Message_ID")==0) return base+0;
    if (fieldName[0]=='S' && strcmp(fieldName, "Sending_Time")==0) return base+1;
    if (fieldName[0]=='T' && strcmp(fieldName, "Trailer")==0) return base+2;
    if (fieldName[0]=='P' && strcmp(fieldName, "Piggybacking")==0) return base+3;
    if (fieldName[0]=='P' && strcmp(fieldName, "Piggybacking_ID")==0) return base+4;
    if (fieldName[0]=='M' && strcmp(fieldName, "Message_Payload")==0) return base+5;
    if (fieldName[0]=='e' && strcmp(fieldName, "event")==0) return base+6;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MyMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "double",
        "bits",
        "int",
        "int",
        "string",
        "int",
    };
    return (field>=0 && field<7) ? fieldTypeStrings[field] : nullptr;
}

const char **MyMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MyMessageDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MyMessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    MyMessage_Base *pp = (MyMessage_Base *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *MyMessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MyMessage_Base *pp = (MyMessage_Base *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MyMessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MyMessage_Base *pp = (MyMessage_Base *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getMessage_ID());
        case 1: return double2string(pp->getSending_Time());
        //case 2: {std::stringstream out; out << pp->getTrailer(); return out.str();}
        case 3: return long2string(pp->getPiggybacking());
        case 4: return long2string(pp->getPiggybacking_ID());
        case 5: return oppstring2string(pp->getMessage_Payload());
        case 6: return long2string(pp->getEvent());
        default: return "";
    }
}

bool MyMessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    MyMessage_Base *pp = (MyMessage_Base *)object; (void)pp;
    switch (field) {
        case 0: pp->setMessage_ID(string2long(value)); return true;
        case 1: pp->setSending_Time(string2double(value)); return true;
        case 3: pp->setPiggybacking(string2long(value)); return true;
        case 4: pp->setPiggybacking_ID(string2long(value)); return true;
        case 5: pp->setMessage_Payload((value)); return true;
        case 6: pp->setEvent(string2long(value)); return true;
        default: return false;
    }
}

const char *MyMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 2: return omnetpp::opp_typename(typeid(bits));
        default: return nullptr;
    };
}

void *MyMessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    MyMessage_Base *pp = (MyMessage_Base *)object; (void)pp;
    switch (field) {
        case 2: return (void *)(&pp->getTrailer()); break;
        default: return nullptr;
    }
}


