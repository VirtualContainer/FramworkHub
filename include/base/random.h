#ifndef RANDOM_H
#define RANDOM_H
#include<stdbool.h>

struct RandomEngine{
    int m_fd;
    unsigned int m_random;
    bool m_renew;
};
typedef struct RandomEngine Random;

extern bool InitRandomEngine(struct RandomEngine*);
extern void CollectRandom(struct RandomEngine*);
extern void GetRandom(unsigned int* const,struct RandomEngine*);

#endif