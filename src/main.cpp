#include "TCPNet.h"
#include <unistd.h>
#include <iostream>

int main() {
	TCPNet::TCPNet net;

	std::cout << "Configuring Network" << std::endl;
	net.NetConfig();
	std::cout << "Starting Network" << std::endl;
	net.Start();

	while (true) {
		if ((net.Accept()) == -1)
			continue;

		std::cout << "Connection From: " << net.GetClientAddr() << std::endl;

		while (true) {
			int err = 0;
			std::string request;

			if ((err = net.RecvRequest(&request)) <= 0) {
				break;
			} else {
				std::cout << request << std::endl;
				if ((err = net.SendResponse("Connected!\n")) == -1){
					break;
				}
			}
		}
	}
	return 0;
}
