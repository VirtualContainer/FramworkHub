#ifndef INIT_H
#define INIT_H
#include "log.h"
#include "timer.h"
#include "processgroup.h"
#include "threadpool.h"
#include "allocate.h"


extern Logger_t g_logger;
extern Time_t g_time;


extern void InitKernel();

#ifdef DEBUG_MODE
extern void TestKernel();
#endif
#endif