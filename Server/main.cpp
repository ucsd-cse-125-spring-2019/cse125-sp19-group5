#define BOOST_ALL_NO_LIB
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <Shared/GameMessage.hpp>

using boost::asio::ip::tcp;
using std::string;
using std::cout;
using std::endl;

boost::mutex global_stream_lock;

boost::asio::io_service ioservice;
tcp::endpoint tcp_endpoint{ tcp::v4(), 1234 };
tcp::acceptor tcp_acceptor{ ioservice, tcp_endpoint };
tcp::socket tcp_socket{ ioservice };

class tcp_connection
	: public boost::enable_shared_from_this<tcp_connection>
{
public:
	typedef boost::shared_ptr<tcp_connection> pointer;

	static pointer create(boost::asio::io_service& io_service)
	{
		return pointer(new tcp_connection(io_service));
	}

	tcp::socket& socket()
	{
		return socket_;
	}

private:
	tcp_connection(boost::asio::io_service& io_service)
		: socket_(io_service)
	{
	}

	void handle_write(const boost::system::error_code& /*error*/,
		size_t /*bytes_transferred*/)
	{
	}

	tcp::socket socket_;
	std::string message_;
};

class tcp_server
{
public:
	tcp_server(boost::asio::io_service& io_service)
		:acceptor_(io_service, tcp::endpoint(tcp::v4(), 12343))
	{
		start_accept();
	}

private:
	void start_accept()
	{
		tcp_connection::pointer new_connection =
			tcp_connection::create(acceptor_.io_service());

		acceptor_.async_accept(new_connection->socket(),
			boost::bind(&tcp_server::handle_accept, this, new_connection,
				boost::asio::placeholders::error));
	}

	void handle_accept(tcp_connection::pointer new_connection,
		const boost::system::error_code& error)
	{
		if (!error)
		{
			new_connection->start();
			start_accept();
		}
	}

	tcp::acceptor acceptor_;
};

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

void OnAccept(const boost::system::error_code & ec, boost::shared_ptr< boost::asio::ip::tcp::socket > sock)
{
	if (ec)
	{
		global_stream_lock.lock();
		std::cout << "[" << boost::this_thread::get_id()
			<< "] Error: " << ec << std::endl;
		global_stream_lock.unlock();
	}
	else
	{
		global_stream_lock.lock();
		std::cout << "[" << boost::this_thread::get_id()
			<< "] Accepted!" << std::endl;
		global_stream_lock.unlock();
	}
}

int server() {
	try {
		game_message msg;
		boost::asio::io_service io_service;
		boost::system::error_code err;
		//listen for new connection
		tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 1234));
		for (;;) {
			//socket creation 
			tcp::socket socket_(io_service);

			//waiting for connection
			acceptor_.accept(socket_);
			cout << "Accept Returned" << endl;

			//read operation
			boost::asio::read(socket_, boost::asio::buffer(msg.data(), game_message::header_length), err);
			msg.decode_header();
			boost::asio::read(socket_, boost::asio::buffer(msg.body(), msg.body_length()), err);
			cout << msg.body() << endl;
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