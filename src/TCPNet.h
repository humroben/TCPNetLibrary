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

	/* Configure Port or IP for the TCP socket
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

	/* Start listening on a TCP Socket
	 *
	 * @return			- Error returned from startup
	 * 					- Zero on success
	 * 					- Positive for GAI error
	 * 					- Negative for error, errno is set
	 */
	int Start();

	/* Accept next client connection
	 *
	 * @return			- Positive for valid file descriptor
	 * 					- Negative for error, errno is set
	 */
	int Accept();

	/* Returns IP address of client connection
	 *
	 * @return			- String containing Client IP (IPv4 or IPv6)
	 */
	std::string GetClientAddr();

	/* Converts GAI error to error string
	 *
	 * @param err		- Error returned from TCPNet::Start()
	 * @return			- String related to given error code
	 */
	std::string GetGaiError(int err);

	/* Send string data to client socket
	 *
	 * @param _message  - Buffer containing data
	 * @return			- Positive/zero Number of bytes sent.
	 * 					- Negative for error, errno is set
	 */
	int SendResponse(std::string _message);

	/* Receive string from client socket
	 *
	 * @param _request  - Buffer to store received data.
	 * @return 		    - Positive, number of bytes received.
	 * 					- Zero, Client disconnection.
	 * 					- Negative for error, errno is set
	 */
	int RecvRequest(std::string *_request) const;

	/* Receive string from client socket
	 *
	 * @param _request  - Buffer to store received data.
	 * @param size		- Size of data expected.
	 * @return 		    - Positive, number of bytes received.
	 * 					- Zero, Client disconnection.
	 * 					- Negative for error, errno is set
	 */
	int RecvRequest(int size, std::string *_request) const;

	/* Receive file from client socket
     *
	 * @param fData     - Buffer to store received data.
	 * @param size		- Size of data expected. (default: 1024 bytes)
	 * @return 		    - Positive, number of bytes received.
	 * 					- Zero, Client disconnection.
	 * 					- Negative for error, errno is set
	 */
	int RecvRequest(int size, char *fData) const;

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
