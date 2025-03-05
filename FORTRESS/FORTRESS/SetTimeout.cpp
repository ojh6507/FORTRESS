#include "SetTimeout.h"

void setTimeout(std::function<void()> callback, int msDelay)
{
	std::thread([callback, msDelay]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(msDelay));
		callback();
	}).detach();
}
