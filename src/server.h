#pragma once
#include "bm_pose2d.h"
#include <pthread.h>
pthread_t tid;
int thread_stop;
void die(char *s);
void DB_unpack(char *pData);
void server(void);
void DB_unpack(char* pData);
void *thrd_func(void *arg);
pose2d pose_opt[20];
