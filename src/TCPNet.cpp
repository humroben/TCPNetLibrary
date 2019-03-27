#include "TCPNet.h"

#include <arpa/inet.h>

#include <iostream>
#include <sstream>
#include <utility>
#include <string.h>
#include <unistd.h>

namespace TCPNet {

// Initialising all private variables (objects)
TCPNet::TCPNet() : nfd(0), sfd(0), settings{0}, tcpSock{0} {}
TCPNet::TCPNet(const TCPNet &c_net): nfd(0), sfd(0), settings(c_net.settings), tcpSock(c_net.tcpSock) {}
TCPNet::TCPNet(TCPNet&& m_net) :
	nfd(std::exchange(m_net.nfd, 0)),
	sfd(std::exchange(m_net.sfd, 0)),
	settings(std::move(m_net.settings)),
	tcpSock(std::move(m_net.tcpSock))
{}

// Cleanly destroy this object, by first closing the socket, then freeing up the address information
TCPNet::~TCPNet() {
	close(sfd);
	freeaddrinfo(tcpSock);
}

/* Configure the IP and port for the TCP Library to bind to.
 *
 * Overloaded to allow a single argument of either custom port or IP address
 * and the ability to configure both.
 */
void TCPNet::NetConfig(std::string _net) {
	if (_net.find(":") != std::string::npos || _net.find(".") != std::string::npos) {
		ip = std::move(_net);
	} else {
		port = std::move(_net);
		SetConfig();
	}
}
void TCPNet::NetConfig(std::string _port, std::string _ip_addr) {
	port = std::move(_port);
	ip = std::move(_ip_addr);

	SetConfig();
}

int TCPNet::Start(){
	struct addrinfo *result;
	int err = 0;

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
		if (sfd == -1)
			continue;
		if ((err = bind(sfd, tcpSock->ai_addr, tcpSock->ai_addrlen)) == -1)
			continue;
		break;
	}

	freeaddrinfo(result);

	if (tcpSock == nullptr)
		return err;

	// Start listening on the derived socket
	if ((err = listen(sfd,1)) == -1)
		return err;

	return err;
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

	return nfd;
}

int TCPNet::RecvRequest(std::string *_request) const{
	std::vector<char> buffer(1024,0);
	int bytes = 0;

	if ((bytes = recv(nfd, &buffer[0], 1024, 0)) <= 0)
		return bytes;

	_request->assign(buffer.cbegin(), buffer.cbegin() + bytes);

	return bytes;
}

// Send a give response to the connected client
int TCPNet::SendResponse(std::string _message) const{
	int len = strlen(_message.c_str());
	return send(nfd, _message.c_str(), len, 0);
}

// Returns the Client's IP address
std::string TCPNet::GetClientAddr() {
	return std::string(addrLen.cbegin(), addrLen.cend());
}

std::string TCPNet::GetGaiError(int err) {
	return gai_strerror(err);
}

// Applies the configuration for the network from user defined values
void TCPNet::SetConfig() {
	if (ip.find(':') != std::string::npos) {
		settings.ai_family = AF_INET6;
	} else if (ip.find('.') != std::string::npos) {
		settings.ai_family = AF_INET;
	} else {
		settings.ai_family = AF_UNSPEC;
	}

	settings.ai_socktype = SOCK_STREAM;
	settings.ai_flags = AI_PASSIVE;
}

}
