#pragma once

namespace scheduler
{
	enum pipeline
	{
		// Asynchronuous pipeline, disconnected from the game
		async = 0,

		count,
	};

	static const bool cond_continue = false;
	static const bool cond_end = true;

	void execute(const pipeline type);

	void schedule(const std::function<bool()>& callback, pipeline type,
	              std::chrono::milliseconds delay = 0ms);
	void loop(const std::function<void()>& callback, pipeline type,
	          std::chrono::milliseconds delay = 0ms);
	void once(const std::function<void()>& callback, pipeline type,
	          std::chrono::milliseconds delay = 0ms);
}
