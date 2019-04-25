#define BOOST_ALL_NO_LIB
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <Shared/Common.h>
#include <Shared/GameMessage.hpp>
#include "GameEngine.h"

using boost::asio::ip::tcp;
using std::string;
using std::cout;
using std::endl;

//typedef boost::shared_ptr<tcp::socket> socket_ptr;
//
//class session{
//session(boost::asio::io_service& io_service)
//	: socket_(io_service)
//{
//}
//};

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

		/*for (;;)
		{
			socket_ptr sock(new tcp::socket(io_service));
			acceptor_.async_accept(*sock);
			boost::thread t(boost::bind(session, sock));
		}*/
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
	GameEngine gameEngine;
	gameEngine.addGameObject(new Player(vec3(0, 0, 0), vec3(1, 0, 0), vec3(1, 0, 0), "player_1", 1));
	gameEngine.addGameObject(new Ball(vec3(5, 0, 0), vec3(-1, 0, 0), "ball_1", 1));

	vector<PlayerInputs> playerInputs;
	for (int i = 0; i < 4; i++) {
		gameEngine.updateGameState(playerInputs);
	}
  server();
	return 0;
}