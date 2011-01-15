/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once

#include <ctime>
#include "mytrace.h"
 
class CMyTimer
{
public:
    CMyTimer(int number = 0) : n(number), start(clock()) { }
    ~CMyTimer(void);

private:
    int n;
    clock_t start;
};
 
inline CMyTimer::~CMyTimer()
{
	MYTRACE(MT::LEVEL_INFO, "Timer %d: %.3lf FPS\n", n, 1.0/((double)(clock()-start)/CLOCKS_PER_SEC));
}