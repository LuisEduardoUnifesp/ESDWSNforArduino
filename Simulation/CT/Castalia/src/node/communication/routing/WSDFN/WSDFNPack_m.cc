//
// Generated file, do not edit! Created by nedtool 4.6 from src/node/communication/routing/WSDFN/WSDFNPack.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "WSDFNPack_m.h"

USING_NAMESPACE


// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




// Template rule for outputting std::vector<T> types
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

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

EXECUTE_ON_STARTUP(
    cEnum *e = cEnum::find("WSDFBPackDef");
    if (!e) enums.getInstance()->add(e = new cEnum("WSDFBPackDef"));
    e->insert(DATA, "DATA");
    e->insert(CONTROLLER_DISCOVERY, "CONTROLLER_DISCOVERY");
    e->insert(RECOVERY_ROUTER, "RECOVERY_ROUTER");
    e->insert(REPORT, "REPORT");
    e->insert(PROGRAMMING, "PROGRAMMING");
    e->insert(ACTION_TABLE_PACKET, "ACTION_TABLE_PACKET");
    e->insert(FLOW_TABLE_PACKET, "FLOW_TABLE_PACKET");
    e->insert(TIMER_TABLE_PACKET, "TIMER_TABLE_PACKET");
    e->insert(REPORT_NEIGHBORHOOD_PACKET, "REPORT_NEIGHBORHOOD_PACKET");
    e->insert(SENSOR_TABLE_PACKET, "SENSOR_TABLE_PACKET");
    e->insert(PROGRAMMING_CONFIRMATION, "PROGRAMMING_CONFIRMATION");
    e->insert(REPORT_CONFIRMATION, "REPORT_CONFIRMATION");
);

Register_Class(WSDFNPack);

WSDFNPack::WSDFNPack(const char *name, int kind) : ::RoutingPacket(name,kind)
{
    this->WSDFNPackKind_var = 0;
    this->TTL_var = 0;
    this->level_var = 0;
    for (unsigned int i=0; i<30; i++)
        this->userData_var[i] = 0;
}

WSDFNPack::WSDFNPack(const WSDFNPack& other) : ::RoutingPacket(other)
{
    copy(other);
}

WSDFNPack::~WSDFNPack()
{
}

WSDFNPack& WSDFNPack::operator=(const WSDFNPack& other)
{
    if (this==&other) return *this;
    ::RoutingPacket::operator=(other);
    copy(other);
    return *this;
}

void WSDFNPack::copy(const WSDFNPack& other)
{
    this->WSDFNPackKind_var = other.WSDFNPackKind_var;
    this->TTL_var = other.TTL_var;
    this->level_var = other.level_var;
    for (unsigned int i=0; i<30; i++)
        this->userData_var[i] = other.userData_var[i];
}

void WSDFNPack::parsimPack(cCommBuffer *b)
{
    ::RoutingPacket::parsimPack(b);
    doPacking(b,this->WSDFNPackKind_var);
    doPacking(b,this->TTL_var);
    doPacking(b,this->level_var);
    doPacking(b,this->userData_var,30);
}

void WSDFNPack::parsimUnpack(cCommBuffer *b)
{
    ::RoutingPacket::parsimUnpack(b);
    doUnpacking(b,this->WSDFNPackKind_var);
    doUnpacking(b,this->TTL_var);
    doUnpacking(b,this->level_var);
    doUnpacking(b,this->userData_var,30);
}

int WSDFNPack::getWSDFNPackKind() const
{
    return WSDFNPackKind_var;
}

void WSDFNPack::setWSDFNPackKind(int WSDFNPackKind)
{
    this->WSDFNPackKind_var = WSDFNPackKind;
}

int WSDFNPack::getTTL() const
{
    return TTL_var;
}

void WSDFNPack::setTTL(int TTL)
{
    this->TTL_var = TTL;
}

int WSDFNPack::getLevel() const
{
    return level_var;
}

void WSDFNPack::setLevel(int level)
{
    this->level_var = level;
}

unsigned int WSDFNPack::getUserDataArraySize() const
{
    return 30;
}

char WSDFNPack::getUserData(unsigned int k) const
{
    if (k>=30) throw cRuntimeError("Array of size 30 indexed by %lu", (unsigned long)k);
    return userData_var[k];
}

void WSDFNPack::setUserData(unsigned int k, char userData)
{
    if (k>=30) throw cRuntimeError("Array of size 30 indexed by %lu", (unsigned long)k);
    this->userData_var[k] = userData;
}

class WSDFNPackDescriptor : public cClassDescriptor
{
  public:
    WSDFNPackDescriptor();
    virtual ~WSDFNPackDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(WSDFNPackDescriptor);

WSDFNPackDescriptor::WSDFNPackDescriptor() : cClassDescriptor("WSDFNPack", "RoutingPacket")
{
}

WSDFNPackDescriptor::~WSDFNPackDescriptor()
{
}

bool WSDFNPackDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<WSDFNPack *>(obj)!=NULL;
}

const char *WSDFNPackDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int WSDFNPackDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int WSDFNPackDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *WSDFNPackDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "WSDFNPackKind",
        "TTL",
        "level",
        "userData",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int WSDFNPackDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='W' && strcmp(fieldName, "WSDFNPackKind")==0) return base+0;
    if (fieldName[0]=='T' && strcmp(fieldName, "TTL")==0) return base+1;
    if (fieldName[0]=='l' && strcmp(fieldName, "level")==0) return base+2;
    if (fieldName[0]=='u' && strcmp(fieldName, "userData")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *WSDFNPackDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "char",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *WSDFNPackDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "WSDFBPackDef";
            return NULL;
        default: return NULL;
    }
}

int WSDFNPackDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    WSDFNPack *pp = (WSDFNPack *)object; (void)pp;
    switch (field) {
        case 3: return 30;
        default: return 0;
    }
}

std::string WSDFNPackDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    WSDFNPack *pp = (WSDFNPack *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getWSDFNPackKind());
        case 1: return long2string(pp->getTTL());
        case 2: return long2string(pp->getLevel());
        case 3: return long2string(pp->getUserData(i));
        default: return "";
    }
}

bool WSDFNPackDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    WSDFNPack *pp = (WSDFNPack *)object; (void)pp;
    switch (field) {
        case 0: pp->setWSDFNPackKind(string2long(value)); return true;
        case 1: pp->setTTL(string2long(value)); return true;
        case 2: pp->setLevel(string2long(value)); return true;
        case 3: pp->setUserData(i,string2long(value)); return true;
        default: return false;
    }
}

const char *WSDFNPackDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *WSDFNPackDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    WSDFNPack *pp = (WSDFNPack *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


