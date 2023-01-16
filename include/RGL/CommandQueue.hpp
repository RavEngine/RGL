#pragma once
#include <memory>

namespace RGL {

	enum class QueueType {
		AllCommands, CopyOnly, ComputeOnly
	};

	struct ICommandBuffer;

	struct ICommandQueue {
		virtual std::shared_ptr<ICommandBuffer> CreateCommandBuffer() = 0;
		virtual void WaitUntilCompleted() = 0;
	};
}