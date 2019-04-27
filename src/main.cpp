#include "TCPNet.h"
#include <unistd.h>
#include <iostream>

//int main() {
//	{
//		TCPNet::TCPNet net;
//		net.NetConfig("127.0.0.1");
//		net.NetConfig("8080");
//	}
//	//std::cout << net.Start() << std::endl;
//
//	std::string input;
//	std::cout << "Enter a word: " << std::endl;
//	std::cin >> input;
//
//	return errno;
//}

int main() {
		TCPNet::TCPNet net;

		std::cout << "Configuring Network" << std::endl;
		net.NetConfig("8080", "127.0.0.1");
		std::cout << "Starting Network" << std::endl;

		int err;
		if ((err = net.Start()) > 0) { // Error with getaddrinfo
			std::cout << net.GetGaiError(err) << std::endl;
			return errno;
		} else if (err == -1) { // error with bind or listen
			return errno;
		}

		bool online = true;

		while (online) {
			if ((net.Accept()) == -1)
				continue;

			std::cout << "Connection From: " << net.GetClientAddr()
					<< std::endl;

			while (true) {
				int err = 0;
				std::string request;

				if ((err = net.RecvRequest(&request)) <= 0) {
					break;
				} else {
					if (((int) request.at(0) == -1) && request.back() == 6) {
						// Ctrl-C: Close client connection goes here
						break;
					}

					// Strip out \r\n for testing purposes only
					//auto trim = request.find("\r");
					//if (trim != std::string::npos)
					//	request.erase(request.begin() + trim, request.end());

					// Strip out \r\n separating the string into lines
					std::vector<std::string> reqLines;
					auto trim = request.find("\r");

					while (trim != std::string::npos) {
						std::string line(request.begin(), request.begin() + trim);

						reqLines.emplace_back(line);
						request.erase(request.begin(), request.begin() + (trim + 2));
						trim = request.find("\r");
					}

					// Stop the application if "stop" is sent
					if (request.find("stop") != std::string::npos) {
						online = false;
						break;
					}

					if (reqLines.back().empty()) {
						//std::string resp = "HTTP/1.1 200 OK\r\n";
						//resp += "Server: TCPNet/0.1\r\n";
						//resp += "Content-Type: text/html\r\n";
						//resp += "Connection: close\r\n";
						//resp += "Hello\r\n";

						//std::cout << resp << std::endl;

						std::cout << "responding" << std::endl;
						if ((err = net.SendResponse("Connected!\r\n")) == -1) {
							break;
						}
					}
				}
			}
		}

	return 0;
}
