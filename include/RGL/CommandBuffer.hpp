#pragma once

namespace RGL {

	struct ICommandBuffer {
		// clear the command buffer, to encode new commands
		virtual void Reset() = 0;

		// signal that new commands are arriving
		virtual void Begin() = 0;

		// signal that all commands ahve been encoded
		virtual void End() = 0;

		// submit onto the queue that created this command buffer
		virtual void Commit() = 0;
	};
}