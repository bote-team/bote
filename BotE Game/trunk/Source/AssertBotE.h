/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once


#ifdef NDEBUG
#undef NDEBUG
#include "boost/shared_ptr.hpp"
#define NDEBUG
#else
#include "boost/shared_ptr.hpp"
#endif

#define AssertBotE(x) assert(x)

