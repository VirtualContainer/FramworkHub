#ifndef MUX_H
#define MUX_H

#define concat(macro,id) macro ## id

#define Choose_Sec(P1,P2,...) P2

#define P_DEF_0 P,
#define P_DEF_1 P,

#define MUX_Judge(head,P1,P2) Choose_Sec(head P1,P2)
#define MUX_Exec(head,macro,P1,P2) MUX_Judge(concat(head,macro), P1, P2)

#define MUX_DEF(macro,P1,P2) MUX_Exec(P_DEF_, macro, P1,P2)

# define likely(x)  __builtin_expect((x),1)
# define unlikely(x)    __builtin_expect((x),0)


#endif