#define BOOST_ALL_NO_LIB
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

string read_(tcp::socket & socket) {
	boost::asio::streambuf buf;
	boost::asio::read_until(socket, buf, "\n");
	string data = boost::asio::buffer_cast<const char*>(buf.data());
	return data;
}

void send_(tcp::socket & socket, const string& message) {
	const string msg = message + "\n";
	boost::asio::write(socket, boost::asio::buffer(message));
}

int server() {
	try {
		boost::asio::io_service io_service;
		//listen for new connection
		tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 1234));
		for (;;) {
			//socket creation 
			tcp::socket socket_(io_service);

			//waiting for connection
			acceptor_.accept(socket_);
			cout << "Accept Returned" << endl;

			//read operation
			string message = read_(socket_);
			cout << message << endl;
			//write operation
			send_(socket_, "Hello From Server!");
			cout << "Servent sent Hello message to Client!" << endl;
		}
	}
	catch (std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

int main(int argc, char **argv) {
	std::cout << "Hello world!" << std::endl;

	server();

	return 0;
}