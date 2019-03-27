#include "TCPNet.h"
#include <unistd.h>
#include <iostream>

int main() {
	TCPNet::TCPNet net;
	net.NetConfig("127.0.0.1");
	TCPNet::TCPNet copy = net;
	net.NetConfig("8080");
	copy.NetConfig("8081");

	std::cout << net.Start() << std::endl;
	std::cout << copy.Start() << std::endl;

	return 0;
}

//int main() {
//	TCPNet::TCPNet net;
//
//	std::cout << "Configuring Network" << std::endl;
//	net.NetConfig("8080","127.0.0.1");
//	std::cout << "Starting Network" << std::endl;
//
//	int err;
//	if ((err = net.Start()) > 0) { // Error with getaddrinfo
//		std::cout << net.GetGaiError(err) << std::endl;
//		return errno;
//	} else if (err == -1) { // error with bind or listen
//		return errno;
//	}
//
//	while (true) {
//		if ((net.Accept()) == -1)
//			continue;
//
//		std::cout << "Connection From: " << net.GetClientAddr() << std::endl;
//
//		while (true) {
//			int err = 0;
//			std::string request;
//
//			if ((err = net.RecvRequest(&request)) <= 0) {
//				break;
//			} else {
//				if (((int)request.at(0) == -1) && request.back() == 6){
//					// Close client connection goes here
//					break;
//				}
//
//				auto trim = request.find("\r");
//				if (trim != std::string::npos)
//					request.erase(request.begin() + trim, request.end());
//
//				std::cout << request << std::endl;
//				if ((err = net.SendResponse("Connected!\n")) == -1){
//					break;
//				}
//			}
//		}
//	}
//	return 0;
//}
