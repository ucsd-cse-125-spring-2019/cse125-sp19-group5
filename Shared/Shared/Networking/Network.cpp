#include "Network.h"
#include <sstream>

boost::asio::io_service Network::ioService;

void Network::poll() {
	ioService.poll();
}
