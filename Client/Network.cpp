#include "Network.h"
#include <sstream>

boost::asio::io_service Network::ioService;
TcpSocket *Network::socket = nullptr;

void Network::init(const std::string &address, int port) {
	try {
		boost::asio::ip::tcp::endpoint endpoint(
			boost::asio::ip::address::from_string(address),
			port
		);

		socket = new TcpSocket(ioService);
		socket->connect(endpoint);
		/* HELP: commented out code
		 * ----
		 * It seemed that this was causing issues with the read() function, 
		 * but we aren't too sure actually. If it's necessary, uncomment
		 */
		//socket->non_blocking(true);
	} catch (std::runtime_error e) {
		std::cerr << "Failed to connect to server!" << std::endl;
		std::cerr << e.what() << std::endl;
	}
}


/*
 * This function will read in the server's response in a two-phase process.
 * First, socket->receive will put the incoming data onto a Boost::asio buffer, 
 * then from that buffer, we will transfer the data onto a "normal" buffer where
 * finally we write it into a normal looking string that we can print out.
 */
void Network::read() {
	//the eventual place where data will go
	boost::asio::streambuf inputBuffer;

	//this is where the socket writes the input data onto
	size_t incomingMessageLength = 11;
	boost::asio::streambuf::mutable_buffers_type theMessage = inputBuffer.prepare(incomingMessageLength);
	
	size_t numBytes = 0;//number of bytes read in
	try 
	{
		/* WARNING: socket->receive()
		 * -------
		 * Sending char* text data from the server doesn't handle spaces well
		 * Initially we sent "Game state!" as the message from the server
		 * but only "Game" gets printed out to the screen. 
		 */

		//synchronously read in data
		numBytes = socket->receive(theMessage);
	}
	catch(const boost::system::system_error &errorFromBoost) 
	{
		/* FIXME:
		 * -----
		 * basically it will complain that there was no work 
		 * it was allowed to do. It said: 
		 * "A non-blocking socket operation could not be completed immediately"
		 * See slack #troubleshooting for a screencap
		 */

		//cout << errorFromBoost.what() << endl;
	}

	if (numBytes > 0) {
		
		//the data from the message gets "commited" to the input buffer
		inputBuffer.commit(numBytes);

		//here we put it into a message string
		std::istream is(&inputBuffer);
		std::string messageString;
		is >> messageString;

		//the printing of the server's message 
		cout << "the server sent: '" << messageString << "'" << endl;

	}//else there is nothing to read?
}

void Network::poll() {
	ioService.poll();
	Network::read();
}

void Network::send(const std::string &data) {
	if (socket) {
		socket->send(
			boost::asio::buffer(data + "\n")
		);
	}
}

void Network::cleanUp() {
	if (socket) {
		socket->close();
		delete socket;
		socket = nullptr;
	}
}