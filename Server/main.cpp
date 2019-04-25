#define BOOST_ALL_NO_LIB
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <Shared/GameMessage.hpp>

using boost::asio::ip::tcp;
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
		game_message msg;
		boost::asio::io_service io_service;
		boost::system::error_code err;
		//listen for new connection
		tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 1234));
					//socket creation 
			tcp::socket socket_(io_service);

			//waiting for connection
			acceptor_.accept(socket_);
			cout << "Accept Returned" << endl;
		for (;;) {

			//read operation
			boost::asio::streambuf buff;
			boost::asio::read_until(socket_, buff, '\n');  // for example

			std::string msg(std::istreambuf_iterator<char>(&buff), {});
			cout << msg << endl;
			io_service.run();
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