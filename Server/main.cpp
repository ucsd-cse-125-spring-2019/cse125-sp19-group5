#define BOOST_ALL_NO_LIB
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <Shared/Common.h>
#include <Shared/GameMessage.hpp>
#include "GameEngine.h"
#include <chrono>

using boost::asio::ip::tcp;

boost::asio::io_service ioService;
tcp::endpoint endpoint{ tcp::v4(), 1234 };
tcp::acceptor acceptor{ ioService, endpoint };
std::vector<tcp::socket*> sockets;		// list of all connected sockets
tcp::socket *newSocket = nullptr;

constexpr auto TICKS_PER_SECOND = 60;//HELP: what number should this be? 

char messageData[128];
boost::asio::streambuf buffer;

void receiveHandler(std::error_code ec, std::size_t bytes) {
	std::cout << ec.message() << std::endl;
	std::cout << "Received message of size " << bytes << std::endl;
	std::string blah;
}

void handleIncomingConnections();
void readMessageHeader(tcp::socket *socket);

void onReceiveMessageHeader(
	tcp::socket *socket,
	const boost::system::error_code& error,
	std::size_t bytes
) {
	if (error) {
		std::cerr << error.message() << std::endl;
		return;
	}

	std::istream is(&buffer);
	std::string s;

	//s is the message that was sent
	is >> s;

	std::cout << s << std::endl;
	readMessageHeader(socket);
}

void readMessageHeader(tcp::socket *socket) {
	/*
	// TODO (bhang): change this to support a header size
	socket->async_read_some(
		boost::asio::buffer(messageData, 8),
		[socket](const boost::system::error_code &error, std::size_t bytes) {
			onReceiveMessageHeader(socket, error, bytes);
		}
	);
	*/

	boost::asio::async_read_until(
		*socket,
		buffer,
		"\n",
		[socket](const boost::system::error_code &error, std::size_t bytes) 
		{
		onReceiveMessageHeader(socket, error, bytes);
		}
	);
}

void onAccept(const std::error_code& error) {
	if (error) 
	{
		std::cout << "AN ERROR OCCURRED: " << error.message() << std::endl;
	}

	//TODO: Assign an player ID to the client and send it to client

	//FUTURE: read the IP and Port and map to an ID

	//read the message that came in
	readMessageHeader(newSocket);

	//list of currently added sockets
	sockets.push_back(newSocket);

	//keep listening (this is done recursive)
	handleIncomingConnections();
}

void handleIncomingConnections() {
	newSocket = new tcp::socket(ioService);
	acceptor.async_accept(
		*newSocket,
		onAccept
	);
}

int main(int argc, char **argv) {
	GameEngine gameEngine;
	gameEngine.addGameObject(new Player(vec3(0, 0, 0), vec3(1, 0, 0), vec3(1, 0, 0), "player_1", 1));
	gameEngine.addGameObject(new Ball(vec3(5, 0, 0), vec3(-1, 0, 0), "ball_1", 1));

	handleIncomingConnections();

	while (true) {
		ioService.poll();

		vector<PlayerInputs> playerInputs;
		for (int i = 0; i < 4; i++) {
			gameEngine.updateGameState(playerInputs);
		}

		/* NOTE:  
		 * ----
		 * Here is how we figured out how to send (broadcast) 
		 * the game state data out to each of the clients. We 
		 * have not considered client disconnects or anything 
		 * like that. Not even sure if asynchronous send is 
		 * appropriate considering the server is on a synchronous
		 * loop to begin with. 
		 */

		//broadcast to each connected client
		for (auto client : sockets) {

			/* -------
			 * WARNING
			 * -------
			 * Sending spaces in data doesn't seem to work.  
			 * The client cuts off the message before the space.
			 */
			//This is the message
			const char* data = "GAME STATE!";
			size_t data_length = 11;

			/* -------
			 * WARNING
			 * -------
			 * C++ string doesn't play nice with boost::asio::buffer 
			 * Use the char* instead! 
			 */
			client->async_send(boost::asio::buffer(data, data_length),[](const boost::system::error_code& ec,
				std::size_t bytes_transferred)
			{
				/*TODO: handle errors*/
			});
		}

		std::this_thread::sleep_for(
			std::chrono::milliseconds(1000 / TICKS_PER_SECOND)
		);
	}

	return 0;
}