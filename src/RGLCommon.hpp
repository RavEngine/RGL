#pragma once
#include "RGL.hpp"

namespace RGL {
	struct RGLGlobals {
		static API currentAPI;
	};

	void LogMessage(DebugSeverity, const std::string&);
	void FatalError(const std::string&);
	static void Assert(bool cond, const std::string& errormsg) {
		if (!cond) {
			FatalError(errormsg);
		}
	}
}