#pragma once
#include <thread>
#include <functional>

void setTimeout(std::function<void()> callback, int msDelay);