#include <growl/plugins/dummy/dummy_system.h>

#include <signal.h>

using Growl::DummySystemAPI;

// Fuck it. Signal handling in C++ is mad.
volatile bool dummy_running = true;

void DummySystemAPI::init() {
	signal(SIGINT, [](int signal) { dummy_running = false; });
}

void DummySystemAPI::dispose() {}

bool DummySystemAPI::isRunning() {
	return dummy_running;
}
