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
}