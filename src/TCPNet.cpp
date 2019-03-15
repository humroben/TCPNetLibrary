#include "TCPNet.h"

#include <arpa/inet.h>

#include <iostream>
#include <sstream>
#include <string.h>
#include <unistd.h>

namespace TCPNet {

// Initialising all private variables (objects)
TCPNet::TCPNet() : nfd(0), sfd(0), err(0), settings{0} {}

/* Configure the IP and port for the TCP Library to bind to.
 *
 * Overloaded to allow a default configuration, a single argument
 * of either custom port or IP address, and the ability to configure
 * both.
 */
void TCPNet::NetConfig() {
	port = "8080";
	SetConfig();
}
void TCPNet::NetConfig(std::string _net) {
	if (_net.find(":") != std::string::npos || _net.find(".") != std::string::npos) {
		ip = _net;
		port = "8080";
	} else {
		port = _net;
	}

	SetConfig();
}
void TCPNet::NetConfig(std::string _port, std::string _ip_addr) {
	port = _port;
	ip = _ip_addr;

	SetConfig();
}

int TCPNet::Start(){
	struct addrinfo *result, *tcpSock;

	// using the settings, create address structure, store in &result, and check for success
	if (ip.empty()){
		if ((err = getaddrinfo(nullptr, port.c_str(), &settings, &result)) != 0) {
			std::cerr << "getaddrinfo: " << gai_strerror(err) << std::endl;
			freeaddrinfo(result);
			return err;
		}
	} else {
		if ((err = getaddrinfo(ip.c_str(), port.c_str(), &settings, &result)) != 0) {
			std::cerr << "getaddrinfo: " << gai_strerror(err) << std::endl;
			freeaddrinfo(result);
			return err;
		}
	}

	// Create the socket using the address structure created from the settings
	for (tcpSock = result; tcpSock != nullptr; tcpSock = tcpSock->ai_next) {
		sfd = socket(tcpSock->ai_family, tcpSock->ai_socktype, tcpSock->ai_protocol);
		// If socket could not be created, move onto next structure
		if (sfd == -1)
			continue;
		// If unable to bind to address, move onto next structure
		if ((err = bind(sfd, tcpSock->ai_addr, tcpSock->ai_addrlen)) == -1)
			continue;
		// If all successful, break out of loop
		break;
	}

	// Clear the full address information list as no longer required
	freeaddrinfo(result);

	// Ensure address structure created by socket was successful
	if (tcpSock == nullptr)
		return err;

	// Start listening on the derived socket
	if ((err = listen(sfd,1)) == -1)
		return err;

	return err;
}

// Close all connections to the server socket
int TCPNet::Stop() {
	return close(sfd);
}

// Initiate accepting of TCP connections
int TCPNet::Accept(){
	struct sockaddr_storage clientAddr;
	socklen_t addrSize;
	addrSize = sizeof(clientAddr);

	// Start accepting connections, storing information about client connection
	nfd = accept(sfd, (struct sockaddr*)&clientAddr, &addrSize);
	std::stringstream ss;

	// Pull Client IP address from the address structure, taking care with IPv4/6
	if (((struct sockaddr*)&clientAddr)->sa_family == AF_INET) {
		std::vector<char> addr(INET_ADDRSTRLEN,0);
		ss << inet_ntop(clientAddr.ss_family, &(((struct sockaddr_in*)&clientAddr)->sin_addr), &addr[0], sizeof(addr));
	} else {
		std::vector<char> addr(INET6_ADDRSTRLEN,0);
		ss << inet_ntop(clientAddr.ss_family, &(((struct sockaddr_in6*)&clientAddr)->sin6_addr), &addr[0], sizeof(addr));
	}

	ss >> addrLen;

	// Return FD for client connection (error state is included)
	return nfd;
}

int TCPNet::RecvRequest(std::string *_request) {
	std::vector<char> buffer(1024,0);

	int bytes = 0;
	// Wait for the client to send data before continuing
	if ((bytes = recv(nfd, &buffer[0], 1024, 0)) <= 0)
		return bytes;

	_request->assign(buffer.cbegin(), buffer.cbegin() + bytes);

	/* Due to the number of elements in the vector for the buffer,
	 * all the empty elements at the end of the buffer needs to be
	 * stripped out.
	 */
	auto trim = _request->find("\r");
	if (trim != std::string::npos)
		_request->erase(_request->begin() + trim, _request->end());

	return bytes;
}

// Send a give response to the connected client
int TCPNet::SendResponse(std::string _message) {
	int len = strlen(_message.c_str());
	err = send(nfd, _message.c_str(), len, 0);

	return err;
}

// Returns the Client's IP address
std::string TCPNet::GetClientAddr() {
	return std::string(addrLen.cbegin(), addrLen.cend());
}

// Applies the configuration for the network from user defined values
void TCPNet::SetConfig() {
	auto search = ip;

	if (search.find(":") != std::string::npos) {
		settings.ai_family = AF_INET6;
	} else if (search.find(".") != std::string::npos) {
		settings.ai_family = AF_INET;
	} else {
		settings.ai_family = AF_UNSPEC;
	}

	settings.ai_socktype = SOCK_STREAM;
	settings.ai_flags = AI_PASSIVE;
}

}
