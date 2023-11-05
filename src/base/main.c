#include"test.h"
#include"bit.h"

#define node_mask 0x1

int main(int argc,char* argv[]){
    #ifdef DEBUG_MODE
      printf("debug mode...\n");
    #else
      printf("normal mode...\n");
    #endif
  /*    
    struct container t_container;
    struct interface t_interface;
    InitInterface(&t_interface,&RegisterContainer,&WithdrawContainer);
    t_interface.m_registerfunc(&t_interface,&t_container);
    if(t_interface.m_container)
      printf("\033[32;1mDEBUG:\033[0mReigiste container successfully\n");
    struct interface t_newinterface;
    CopyInterface(&t_interface,&t_newinterface);
    printf("\033[32;1mDEBUG:\033[0mThe container of old interface address is %p\n",&(*(t_interface.m_container)));
    printf("\033[32;1mDEBUG:\033[0mThe container of new interface address is %p\n",&(*(t_newinterface.m_container)));
    t_interface.m_withdrawfunc(&t_interface,&t_container);
    if(t_interface.m_container)
      printf("\033[31;1mERROR:\033[0mWithdraw container failed\n");
    struct container* ptr_container=&t_container;
    struct container* copyptr_container=ptr_container;
    container_test(copyptr_container);
    if(t_container.tag)
       printf("\033[32;1mDEBUG:\033[0mCopy container successfully\n");
  */
    RadixTree root;
    BuildRadixTree(&root,4,8);    
    struct Item* item = (struct Item*)malloc(sizeof(struct Item));
    MountRadixTreeItem(&root,0x0000000f,item);
    MountRadixTreeItem(&root,0x00000000,item);
    return 0;
}