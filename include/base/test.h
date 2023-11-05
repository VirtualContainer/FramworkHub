#ifndef TEST_H
#define TEST_H

#include"link.h"
#include"hash.h"
#include"directionary.h"

struct container{
     int* tag;
     unsigned int m_counts;
     int m_value;
     linklist_node m_link;
     hashlist_node m_hash;
};

struct interface;
 
typedef void (*RegisterContainerFunc)(struct interface*,struct container*);
typedef void (*WithdrawContainerFunc)(struct interface*,struct container*);

struct interface{
     struct container* m_container;
     RegisterContainerFunc m_registerfunc;
     WithdrawContainerFunc m_withdrawfunc;
};

extern void InitInterface(struct interface*,RegisterContainerFunc,WithdrawContainerFunc);
extern void DestroyInterface(struct interface*);
extern void CopyInterface(const struct interface*,struct interface*);
extern void RegisterContainer(struct interface*,struct container*);
extern void WithdrawContainer(struct interface*,struct container*);

#endif