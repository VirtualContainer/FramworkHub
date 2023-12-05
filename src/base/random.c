#include"random.h"
#include<unistd.h>
#include<fcntl.h>


 bool InitRandomEngine(struct RandomEngine* randomer)
 {
    randomer->m_fd = open("/dev/urandom",O_RDONLY);
    if(randomer->m_fd<0)
        return false;
    randomer->m_random = -1;
    randomer->m_renew = false;    
    return true;
 }

 void CollectRandom(struct RandomEngine* randomer)
 {
    unsigned int value;
    size_t byte = read(randomer->m_fd,&value,sizeof(value));
    if(byte!=sizeof(value))
        return;
    randomer->m_random = value;
    randomer->m_renew = true;
 }

void GetRandom(unsigned int* const rv,struct RandomEngine* randomer)
{
    if(randomer->m_renew==false)
        return;
    *rv = randomer->m_random;
    randomer->m_renew = false;    
}