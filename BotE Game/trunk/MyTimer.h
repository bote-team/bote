/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once

#include <ctime>
 
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
	TRACE("timer %d: %.3lf FPS\n", n, 1.0/((double)(clock()-start)/CLOCKS_PER_SEC));
/*	CString s;
	s.Format("timer %d took %lf seconds\n", n, double(clock()-start)/CLOCKS_PER_SEC);
	AfxMessageBox(s);
*/
}