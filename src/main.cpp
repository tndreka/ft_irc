/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 17:53:43 by tndreka           #+#    #+#             */
/*   Updated: 2025/09/26 23:01:54 by tndreka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string.h>
#include <unistd.h> 
#include <sys/socket.h> // for socket liberary
#include <netinet/in.h> // for socketaddr
#include <arpa/inet.h> // for inet_addr
#include <netdb.h> // defines NI_MAXHOST 
#include <unistd.h>
#include <fcntl.h> // fcntl()
#include <poll.h> // for poll()
#include <vector>
#include <map>

/*====== TEST ========*/

/*
	External Functions
	socket √
	close √
	setsockopt
	getsockname
	getprotobyname
 	gethostbyname
	getaddrinfo
	freeaddrinfo
 	bind √
  	connect
	listen  
	accept
	htons √
	htonl 
	ntohs
	ntohl
	inet_addr √
	inet_ntoa
	send
	recv
	signal
	sigaction
	lseek
	fstat
	fcntl
	poll
*/



int main(int ac, char *av[])
{
	/*
				=================== SOCKET =========================
		we use socket() function.
		socket(int Domain, int type, int protocol);
		1)First Param: 
		-int Domain -> use AF_INET -> {is an addres family,
		that is used to designate the type of address that your socket can communicate in this case IPV4(internet protocol V4)} 
		2)Second Param:
		-int type -> SOCK_STREAM -> {Provides sequenced, reliable two way connection based byte streams.
		it make sure that the data is not dublicated, it doesnt get lost and its delivered on correct order}
		3) Third param:
		-int protocol -> 0 -> {Protocol specifies a particular protocol to be used with the socke.
		Normally a single protocol exit to support the socket type within the given protocol family wich most of the times is 0}
		RETURN:
			SUCCESS: A file descriptor for the new socket created is being returned.
			ERROR: -1
	*/
	//create socket
	int listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == -1)
		return std::cerr << "Can't create Socket" << std::endl, -1;
	fcntl(listening, F_SETFL, O_NONBLOCK);	
	/*
				================== sockaddr_in ===================
		this is a structure used  to represent IPv4 internet domain socket address.
		it is designed for IPv4 only and it contains mambers for the family addres as: 
		->address_family(sin_family) = should be set as AF_INET to use the IPv4
		->port_number(sin_port) = holdsthe portnumber in the network byte order requiring 
		conversion from the host byte order using fucntion hstons().
		->and IPv4 adress = this is the 32 bit IPv4 addres which it can be assigned manually or by using
		functions like inet_addr()
		
		htons(uint16_t hostshort);
		->this function converts the unsigned short int hostshort from host byte order to network byte order.

		inet_addr(const char *p)
		
	*/
		
	//bind the socket to an IP
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	//hint.sin_addr.s_addr = inet_addr("127.0.0.1"); // this listen to one IP
	hint.sin_addr.s_addr = INADDR_ANY;
	/*
				=================== BIND ======================
		bind() function assigns the address specified by the hinf to the socked referred to by the
		file descriptor listening.
		
		bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
		Parameter:
		1) sockfd - socket file descriptor (listening) 
		2) addr - pointer to sockaddr structure (cast hint to sockaddr *)
		3) addrlen - size of the address structure (sizeof(hint))
		
		RETURN:
			Success => 0
			ERROR => -1
	*/
	if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1)
	{
		std::cerr << "Cant bind IP to PORT" << std::endl;
		close(listening);
		return -1;
	}
	std::cout << "Binding IP to port is √ !  !  !" << std::endl;
	// Mark socket for listening in
	/*
				==================== LISTEN ====================
			listen() -> this function marks the socket referred by sockedfd as a passivesocket
			that will be used to accept incoming connection requests.
			
			 int listen(int sockfd, int backlog)
			Parameters:
			sockfd - the socket file descriptor (in my case listening vriable)
			backlog - the maximum number of pending connections that can be queued

			RETURN:
				success => 0
				ERROR => -1
			
				backlog values : SOMAXCONN (system maximum usually 128)
				          		 5 to 10 for  small servers.
						 
	*/
	if(listen(listening, SOMAXCONN) == -1)
	{
		std::cerr << "Can't listen in the socker" << std::endl;
		close(listening);
		return -1;
	}
	std::cout << "Server is listening on 0.0.0.0:54000" << std::endl;
	// ACCEPTING CONNECTION , this is where we will do the client. 
	sockaddr_in client;
	socklen_t	clientSize = sizeof(client);
	char		host[NI_MAXHOST]; // 1025 max num of hosts
	char		service[NI_MAXSERV]; // 32 max num of serv
	
	/*
				===================== ACCEPT ======================
		the accept() function first extracts the first connection request on the
		queue of pending connections for the listening socket, creates a new connected socket, 
		and returns the new fd referring to that socket.
	
		int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
		Parameters:
		1) sockfd - listening socket file descriptor
		2) addr - pointer to sockaddr struct to store client info
		3) addrlen - pointer to size of addr struct

		RETURN:
			success => New socket file descriptor for the connection
			ERROR => -1
	*/

	/*
		=============== HANDLING ONE CLIENT CONNECTION ===============
	*/
				// int clientSocket;
				// if((clientSocket = accept(listening, (sockaddr*)&client, &clientSize)) == -1)
				// {
				// 	std::cerr << "Failed in connecting the client !  !  !" << std::endl;
				// 	close(listening);
				// 	return -1;
				// }
				// std::cout << "Clinet connected successfully √ √ √ ! ! ! " << std::endl;
				// //here we will be the client info
				// memset(host, 0, 1025); // clear the arr
				// memset(service, 0, 32); 
				
				// /*
				// 			========= IRC =============
				// 	recive client name through the socket connection and display in 
				// 	the format  ClientName connected on ServiceName
					
				// */

				// //Welcome message
				// std::string welcomeMsg = "Welcome! Please enter your name: ";
				// /*
				// 			================= SEND ========================
				// 	the send() dunction is able to transmit data to a connected socket.

				// 	ssize_t send(int sockfd, const void* buf, size_t len, int flags);
				// 	Parameters:
				// 	1) sockfd - socked fd
				// 	2) buf - pointer to the data that will be send
				// 	3) len - num of bytes that will be send
				// 	4) flags - control flags usually 0
					
				// 	RETURN: 
				// 		success -> the number of bytes sent
				// 		ERROR => -1
				// */
				// // This was sending the client msg
				// int byte_sent;
				// if((byte_sent = send(clientSocket, welcomeMsg.c_str(), welcomeMsg.length(), 0)) == -1)
				// {
				// 	std::cerr << "Failed on welcome msg " << std::endl;
				// 	close(clientSocket);
				// 	close(listening);
				// 	return -1;
				// }
				// //Recive client response
				// 	/*
				// 			==================== RECV ======================
				// 		recv() function recives data from a connected socket.
					
				// 		ssize_t send(int sockfd, boid *buf, size_t len, int flags);
				// 	Parameters:
				// 	1)sockfd -> socked fd
				// 	2) buf -> to store the recived data.
				// 	3) len -> max number of bytes to recive
				// 	4) flag ->  control flags usually 0

				// 	RETUER:
				// 		SUCCESS: Number if bytes recived.
				// 		ERROR: -1
				// 		Connection CLOSED  = 0;
				// 	*/
				// char buff[1024] = {0};
				// int byteRecived;
				// if((byteRecived = recv(clientSocket, buff, 1024, 0)) == -1)
				// {
				// 	std::cerr << "Failed to recive data from CLIENT !  !  ! " << std::endl;
				// 	close(clientSocket);
				// 	close(listening);
				// 	return -1;
				// }
				// else if(byteRecived == 0)
				// {
				// 	std::cout << "client disconnected"<< std::endl;
				// }
				// else
				// {
				// 	//Null-terminate and displayed the recived data
				// 	buff[byteRecived] = '\0';
					
				// 	//client ip
				// 	char* client_ip = inet_ntoa(client.sin_addr);
					
				// 	std::cout << buff << "connected from " << client_ip << std::endl;
				// 	std::string reply = "Connected\n";
				// 	send(clientSocket, reply.c_str(), reply.length(), 0);
				// }
				// char buf[4444];
				// while (true)
				// {
				// 	//clear the buf
				// 	memset(buf, 0, 4444);
				// 	//waiting for a msg
				// 	int byte_recv = recv(clientSocket, buf, 4444, 0); 
				// 	if(byte_recv == -1)
				// 	{
				// 		std::cerr << "There was a connection issue ! ! !\n";
				// 		break;
				// 	}
				// 	if (byte_recv == 0)
				// 	{
				// 		std::cout << "The client disconected ! ! !\n";
				// 		break;
				// 	}
				// 	std::cout << "Recived: " << std::string(buf, 0, byte_recv);
				// 	send(clientSocket, buf, byte_recv, 0);
				// }
				// close(clientSocket);
				// close(listening);

	/*
			=============== HANDLING MULTIPLE CLIENTS ==============
	*/
		
		/*
            ==================== POLL ======================
        poll() function allows monitoring multiple file descriptors to see if I/O is 
        possible on any of them. It's essential for handling multiple clients simultaneously.
        int poll(struct pollfd *fds, nfds_t nfds, int timeout);
        struct pollfd {
            int   fd;         // file descriptor to monitor
            short events;     // events to monitor (POLLIN, POLLOUT, etc.)
            short revents;    // events that actually occurred
        };
        Parameters:
        1) fds -> array of pollfd structures (file descriptors to monitor)
        2) nfds -> number of file descriptors in the array
        3) timeout -> timeout in milliseconds (-1 = infinite, 0 = don't block)
        Events:
        - POLLIN: Data available for reading  
        - POLLOUT: Ready for writing
        - POLLHUP: Hang up (connection closed)
        - POLLERR: Error condition
        RETURN:
            SUCCESS: Number of file descriptors with events
            TIMEOUT: 0 (if timeout occurred)
            ERROR: -1
*/
		
	std::vector<pollfd> poll_fds;
	pollfd listening_fd;
	listening_fd.fd = listening; //watch the listening socket
	listening_fd.events = POLLIN; // check for incoming events
	listening_fd.revents = 0; // clear reevents
	poll_fds.push_back(listening_fd);
	
	//store client info
	
	std::map<int, std::string> clients;
	
	std::cout << "READY TO GET CONNECTED > > > . . .\n";
	
	while(true)
	{
		//init poll
		int poll_count = poll(&poll_fds[0], poll_fds.size(), -1);
		if(-1 == poll_count)
		{
			std::cerr<<"Poll Failed ! ! !\n";
			break;
		}
		//check for events
		for(size_t i = 0; i < poll_fds.size(); i++)
		{
			//flags
			bool incoming_data = false;
			bool client_hungup = false;
			bool is_listening = false;
			bool err = false;
			
			//cheks for each event POLLIN,POLLERR, POLLHUP,
			if (poll_fds[i].revents & POLLIN)
				incoming_data = true;
			if (poll_fds[i].revents & POLLHUP)
				client_hungup = true;
			if (poll_fds[i].revents & POLLERR)
				err = true;
			if (poll_fds[i].fd == listening)
				is_listening = true;
			//here handle client incoming data [new_connection or messages]
					
			if(incoming_data || client_hungup || err)
				std::cout << "socket: " << poll_fds[i].fd << " data: " << incoming_data << " listening: " << is_listening << " size: " << poll_fds.size() << std::endl;	
			if (incoming_data)
			{
				//d::cout << "=========== LISTENIN SOCKET>>> ==================> \n";
				if(is_listening)
				{
				//accept new connection
					int new_connection = accept(listening, (sockaddr*)&client, &clientSize);
					//check if connection is handle successfully
					std::cout << " ======= aceppt passed ==================> \n";
					if(new_connection != -1)
					{
							std::cout << "============== connection aceppted ==================> \n";
					//configure the new client as non-blocking
						if(fcntl(new_connection, F_SETFL, O_NONBLOCK)!= -1)
						{
						//configure client
						//add the client to the vector
							pollfd new_client_fd; //new structor for the new accepted host
							new_client_fd.fd = new_connection;
							new_client_fd.events = POLLIN;
							new_client_fd.revents = 0;

						//add it in the vector
							poll_fds.push_back(new_client_fd);
								std::cout << "SIZE: " << poll_fds.size() << std::endl;
						//store client info
							char *client_ip = inet_ntoa(client.sin_addr);
							clients[new_connection] = std::string(client_ip);
						//welcome msg from the server
							std::string welcome = "Welcome to IRC \n";
							send(new_connection, welcome.c_str(), welcome.length(), 0);
						}
						else
						{
							std::cout << "Failed to set non-blocking for the host" << std::endl;
							close(new_connection);
						}
					}
				}
				else
				{
					char buff[4444] = {0};
					int	bytes_recived = recv(poll_fds[i].fd, buff, 4444, 0);
					
					//if we recived bytes 
					if (bytes_recived > 0)
					{
						buff[bytes_recived] = '\0';
						send(poll_fds[i].fd, buff, bytes_recived, 0);
					}
					// else if (bytes_recived 
					// {
					// 	/* code */
					// }	
					
				}
			}
		}
	}
	return 0;
}