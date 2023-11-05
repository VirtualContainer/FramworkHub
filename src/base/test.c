#include"test.h"
#include<string.h>

void RegisterContainer(struct interface* p_interface,struct container* p_container){
    p_interface->m_container=p_container;
}

void WithdrawContainer(struct interface* p_interface,struct container* p_container){
    p_interface->m_container=NULL;
}

void InitInterface(struct interface* p_interface,RegisterContainerFunc p_registerfunc,WithdrawContainerFunc p_withdrawfunc){
    p_interface->m_container=NULL;
    p_interface->m_registerfunc=p_registerfunc;
    p_interface->m_withdrawfunc=p_withdrawfunc;
}

void DestroyInterface(struct interface* p_interface){
    p_interface->m_container=NULL;
    p_interface->m_registerfunc=NULL;    
    p_interface=NULL;
}

void CopyInterface(const struct interface* p_src,struct interface* p_dest){
    if(!p_src)
        return;
    if(!p_dest)
        p_dest=(struct interface*)malloc(sizeof(struct interface));
    if(p_src==p_dest)
        return;
    InitInterface(p_dest,NULL,NULL);
    if(p_src->m_container){
        if(!(p_dest->m_container))
            p_dest->m_container=(struct container*)malloc(sizeof(struct container));
        memcpy(p_dest->m_container,p_src->m_container,sizeof(struct container));
    }
    p_dest->m_registerfunc=p_src->m_registerfunc;
    p_dest->m_withdrawfunc=p_src->m_withdrawfunc;    
}