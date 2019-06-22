#include "TCPNet.h"

#include <arpa/inet.h>

#include <iostream>
#include <sstream>
#include <utility>
#include <string.h>
#include <unistd.h>

namespace TCPNet {

// Default Constructor (object initialisation)
TCPNet::TCPNet() : nfd(-1), sfd(-1), settings{0}, tcpSock{0} {}
// Copy Constructor
TCPNet::TCPNet(const TCPNet &c_net): ip(c_net.ip), port(c_net.port), nfd(-1), sfd(-1), settings(c_net.settings), tcpSock(c_net.tcpSock) {}
// Move Constructor
TCPNet::TCPNet(TCPNet&& m_net) :
	ip(std::move(m_net.ip)),
	port(std::move(m_net.port)),
	nfd(std::exchange(m_net.nfd, -1)),
	sfd(std::exchange(m_net.sfd, -1)),
	settings(std::move(m_net.settings)),
	tcpSock(std::move(m_net.tcpSock))
{}

// Close client connection before destroying object
TCPNet::~TCPNet() {
	if (sfd != -1)
		close(sfd);
}

// Configure Port or IP for the TCP socket
void TCPNet::NetConfig(std::string _net) {
	if (_net.find(':') != std::string::npos || _net.find('.') != std::string::npos) {
		ip = std::move(_net);
	} else {
		port = std::move(_net);
		SetConfig();
	}
}
// Configure Port and IP for the TCP socket
void TCPNet::NetConfig(std::string _port, std::string _ip_addr) {
	port = std::move(_port);
	ip = std::move(_ip_addr);

	SetConfig();
}

// Start listening on a TCP Socket
int TCPNet::Start(){
	struct addrinfo *result;
	int err = 0;

	// using the settings, create address structure, store in &result, and check for success
	if (ip.empty()){
		if ((err = getaddrinfo(nullptr, port.c_str(), &settings, &result)) != 0) {
			freeaddrinfo(result);
			return err;
		}
	} else {
		if ((err = getaddrinfo(ip.c_str(), port.c_str(), &settings, &result)) != 0) {
			freeaddrinfo(result);
			return err;
		}
	}

	// Create the socket using the address structure created from the settings
	for (tcpSock = result; tcpSock != nullptr; tcpSock = tcpSock->ai_next) {
		sfd = socket(tcpSock->ai_family, tcpSock->ai_socktype, tcpSock->ai_protocol);
		if (sfd == -1)
			continue;
		int opt = 1;
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
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

// Accept next client connections, return file descriptor
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

// Receives string from client socket
int TCPNet::RecvRequest(std::string *_request, int size) const{
	int bytes = 0;

	char *buffer = new char[size];
	while (size > bytes) {
		int bytes_r = 0;
		if ((bytes_r = recv(nfd, buffer + bytes, size - bytes, 0)) <= 0)
			return bytes;
		else
			bytes += bytes_r;
	}

	_request->assign(buffer);

	return bytes;
}

// Receives File data from client socket
int TCPNet::RecvRequest(char* fData, int size) const{
	int bytes = 0;

	char *buffer = new char[size];
	while (size > bytes) {
		int bytes_r = 0;
		if ((bytes_r = recv(nfd, buffer + bytes, size - bytes, 0)) <= 0)
			return bytes;
		else
			bytes += bytes_r;
	}

	fData = buffer;

	return bytes;
}

// Send a give response to the connected client
int TCPNet::SendResponse(std::string _message){
	int bytes = send(nfd, _message.c_str(), strlen(_message.c_str()), 0);
	return bytes;
}

// Returns IP address of client connection
std::string TCPNet::GetClientAddr() {
	return std::string(addrLen.cbegin(), addrLen.cend());
}

// Converts GAI error to error string
std::string TCPNet::GetGaiError(int err){
	return gai_strerror(err);
}

// Set the configuration specified in NetConfig()
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
