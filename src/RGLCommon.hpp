#pragma once
#include "RGL.hpp"

#define STATIC(a) decltype(a) a
#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))

namespace RGL {
	extern API currentAPI;

	void LogMessage(MessageSeverity, const std::string&);
	void FatalError(const std::string&);
	static void Assert(bool cond, const std::string& errormsg) {
		if (!cond) {
			FatalError(errormsg);
		}
	}

#if defined __APPLE__ || __STDC_VERSION__ >= 199901L    //check for C99
#define stackarray(name, type, size) type name[size]    //prefer C VLA on supported systems
#else
#define stackarray(name, type, size) type* name = (type*)alloca(sizeof(type) * size) //warning: alloca may not be supported in the future
#endif
}