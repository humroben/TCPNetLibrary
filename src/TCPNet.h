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
	// Default constructor
	TCPNet();
	// Copy Constructor
	TCPNet(const TCPNet &c_net);
	// Move Constructor
	TCPNet(TCPNet&& m_net);

	~TCPNet();

	/* Configured to Port or IP for the socket
	 *
	 * @param _net		- String of port or IP address
	 */
	void NetConfig(std::string _net);

	/* Configure the Port and IP address for the socket
	 *
	 * @param _port		- String with port
	 * @param _ip_addr	- String with IP address
	 */
	void NetConfig(std::string _port, std::string _ip_addr);

	// Start Listening on the given configuration
	int Start();
	// Stop Listening
	int Stop();

	// Start accepting connections.
	int Accept();

	// returns a string of the client's address
	std::string GetClientAddr();
	std::string GetGaiError(int err);

	/* Send string data to client socket
	 *
	 * @param _message  - Buffer containing data
	 * @return			- Number of bytes sent. Negative = error.
	 */
	int SendResponse(std::string _message) const;

	/* Receives string from client socket
	 *
	 * @param _request  - Buffer to store received data.
	 * @return 		    - Number of bytes received. 0 = disconnect, negative = error.
	 */
	int RecvRequest(std::string *_request) const;

private:
	// Set the configuration specified in NetConfig()
	void SetConfig();

private:
	// Used to store the network configuration
	std::string ip, port, addrLen;
	// File Descriptors for the main socket, and accepted connections
	int nfd, sfd;
	// Storage for the address structure to connect with
	struct addrinfo settings, *tcpSock;
};

}

#endif
