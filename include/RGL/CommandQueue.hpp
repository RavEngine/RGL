#pragma once
#include <RGL/Types.hpp>
#include <memory>

namespace RGL {

	enum class QueueType {
		AllCommands, CopyOnly, ComputeOnly
	};

	struct ICommandBuffer;

	struct ICommandQueue {
		virtual RGLCommandBufferPtr CreateCommandBuffer() = 0;
		virtual void WaitUntilCompleted() = 0;
	};
}
