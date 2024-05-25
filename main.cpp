#include <array>
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <ostream>

using std::endl;
using std::cout;
using std::string;
using boost::asio::ip::tcp;

void server() {
	try {

		boost::asio::io_context io_context;
		tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

		while (1) {
			std::array <char, 128> buf{};

			cout << "\nAccepting connection on port 8080" << endl;
			tcp::socket socket(io_context);
			acceptor.accept(socket);

			cout << "Server got client" << endl;
			boost::system::error_code error;

			size_t len = socket.read_some(boost::asio::buffer(buf), error);
			cout << "\nServer received buf:" << endl;
			cout.write(buf.data(), len);

			if (memcmp(buf.data(), "Hello", 6) == 0)
				boost::asio::write(socket, boost::asio::buffer("World"), error);
			else {
				boost::asio::write(socket, boost::asio::buffer(buf), error);
			}
			cout << "\nServer wrote some data to Client:" << endl;
			if (memcmp(buf.data(), "Hello", 6) == 0)
				cout << "World\n";
			else
				cout.write(buf.data(), len);

			if (error == boost::asio::error::eof)
				break;
			else if (error)
				throw boost::system::system_error(error);
			
			//acceptor.close();
			//socket.close();
		}
	}
	catch (std::exception &ex) {
		std::cerr << "\n" << ex.what() << endl;
		return;
	}
}

void client(const bool flag) {
	try {

		boost::asio::io_context io_context;
		tcp::resolver resolver{ io_context };

		auto end_points = resolver.resolve("127.0.0.1", "8080");

		tcp::socket socket{ io_context };
		boost::asio::connect(socket, end_points);
		cout << "\nClient connected to Server with connection on port 8080" << endl;

		std::array <char, 128> buf{};

		boost::system::error_code error;
			
		if (flag) {
			cout << "\nInput some echo data: ";
			std::cin.getline(buf.data(), 128, '\n');
		}
		else
			memcpy(buf.data(), "Hello", 6);

		boost::asio::write(socket, boost::asio::buffer(buf), error);
		cout << "\nClient wrote buf to Server:" << endl;
		cout.write(buf.data(), buf.size());

		if (error)
			throw boost::system::system_error(error);

		size_t len = socket.read_some(boost::asio::buffer(buf), error);
		cout << "\nClient received buf:" << endl;
		cout.write(buf.data(), len);

		resolver.cancel();
		socket.close();
		cout << "\n\nClient disconected from Server\n" << endl;
	}
	catch (std::exception &ex) {
		std::cerr << "\n" << ex.what() << endl;
	}
}

int main(int argc, char *argv[]) {
	try {
		if (argv[1] != nullptr) {

			//input to *argv[]: [1]: "client_echo" or "client_hello" or "server"
			if (strcmp(argv[1], "client_echo") == 0)
				client(true);
			else if (strcmp(argv[1], "client_hello") == 0)
				client(false);
			else if (strcmp(argv[1], "server") == 0)
				server();

			else {
				std::cerr << "\nno client and no server" << endl;
				return 1;
			}

		}
		return 0;
	}
	catch (...) {
		std::cerr << "\nunknown error" << endl;
		return 1;
	}
}
