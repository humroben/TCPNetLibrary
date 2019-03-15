#ifndef SRC_TCPNET_H_
#define SRC_TCPNET_H_

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <vector>
#include <string>

namespace TCPNet {

class TCPNet {
public:
	TCPNet();

	// configure defaults for net configuration
	void NetConfig();
	// Configures port/IP address depending on structure
	void NetConfig(std::string _net);
	// Configures port and IP with user definitions
	void NetConfig(std::string _port, std::string _ip_addr);

	// Start Listening on the given configuration
	int Start();
	// Stop Listening
	int Stop();

	// Start accepting connections.
	int Accept();

	// returns a string of the client's address
	std::string GetClientAddr();

	// Handle the transmission and reception of messages
	int SendResponse(std::string _message);
	int RecvRequest(std::string *_request);

private:
	// Set the configuration specified in NetConfig()
	void SetConfig();

private:
	// Used to store the network configuration
	std::string ip, port, addrLen;
	// File Descriptors for the main socket, and accepted connections
	int nfd, sfd, err;
	// Storage for the address structure to connect with
	struct addrinfo settings;
};

}

#endif
