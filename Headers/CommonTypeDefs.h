#pragma once
#ifndef CommonTypeDefs_h
#define CommonTypeDefs_h

#include <cstring>

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// 
//		CommonTypeDefs
// 
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// 
//		File:			CommonTypeDefs.cpp
//		Author:			John G.Lynch
//		Legal:			Copyright @2026, per MIT License below
//		Date:			October 24, 2025 ( file creation )
//


//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// Apologies to purists, but I want simpler, clearer, shorter variable
// declarations (no "unsigned long long", etc.) 
// Type aliases:

typedef unsigned _int64 u64;
typedef unsigned int u32;
typedef unsigned long u32l;
typedef unsigned short u16;
typedef char u8;
typedef _int64 s64;
typedef int s32;
typedef short s16;

// Useful constants:

#define u64_Max UINT64_MAX
#define u32_Max UINT32_MAX
#define u16_Max UINT16_MAX

// 64 byte alignment macro and 512 bit (8 QWORD) aligned variable declaration
#define ALIGN64 __declspec(alignas(64))
#define _UI512(name) alignas(64) u64 name[8] /* Big-endian: name[0]=MSB qword, name[7]=LSB */

// Macro helper to construct and pass message for Assert
#define _MSGW(msg) [&]			\
	{							\
		std::wstringstream _s;	\
		 _s << msg;				\
		 return _s.str();		\
	 }().c_str()

#define _MSGA(msg) [&]			\
	{							\
		std::stringstream _s;	\
		_s << msg;				\
		return _s.str();		\
	}().c_str()

// Macro to convert a ui512 variable to a hex string for messaging
#define _MtoHexString(ui512var) [&]						\
	{													\
		std::stringstream _s2;							\
		_s2 << std::hex << std::uppercase;				\
		for (int _i = 7; _i >= 0; _i--) {				\
			_s2.width(16);								\
			_s2.fill(L'0');								\
			_s2 << ui512var[_i];						\
			if (_i > 0) {								\
				_s2 << " ";								\
			}											\
		}												\
		return _s2.str();								\
	}().c_str()

#endif