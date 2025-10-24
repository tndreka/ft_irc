/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 15:22:24 by tndreka           #+#    #+#             */
/*   Updated: 2025/10/22 12:52:22 by tndreka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include "../include/Server.hpp"
#include <string>
#include <unistd.h>

Server::Server() : _name("MalakaIRC"), _users() {}

Server::Server(const Server &other) {
	(void)other;
	std::cout << "Copy server not allowed !  !  !\n";
}

Server &Server::operator=(const Server &other) {
	(void)other;
	std::cout << "= operator_server not allowed !  !  !\n";
	return *this;
}

Server::~Server() {}

std::string Server::getName() const {
		return _name;
}

const std::string Server::getPass() const {
		return _password;
}

const std::map<int, User*>&	Server::getUsers(void) const {
	return (_users);
}

std::vector<Channel*>	Server::getChannels(void) const {
	return (_channels);
}

void	Server::addChannel(Channel* new_channel) {
	_channels.push_back(new_channel);
}

void	Server::deleteChannel(User* user, const std::string& name) {
	Channel*			channel_to_delete;
	const std::string	prefix = ":" + _name + " NOTICE " + user->getNickname() + ":";
	const std::string	postfix = "\r\n";
	std::string 		msg;

	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		if ((*it)->getName() == name)
			channel_to_delete = *it;
	}
	std::vector<Channel*>::iterator new_end = std::remove(_channels.begin(), _channels.end(), channel_to_delete);
    _channels.erase(new_end, _channels.end());
	msg = prefix + " Channel '" + channel_to_delete->getName() + "' deleted successfully!" + postfix; 
	send(user->getPoll().fd, msg.c_str(), msg.size(), 0);
	delete channel_to_delete;

}

std::ostream& operator<<(std::ostream& out, const Server& obj) {
	const std::map<int, User*>& activeMembers = obj.getUsers();
	typedef std::map<int, User*>::const_iterator iter; 

	out << "Server name: " << obj.getName() << "\nServer password: " << obj.getPass();
	out << "\nServer members: 'member_fd': 'member_username'" << std::endl;
	if (activeMembers.empty())
			out << "\tNo members in the server!\n";
	else {
			for (iter it = activeMembers.begin(); it != activeMembers.end(); ++it) {
					if (it->second != NULL)
							out << "\t'" << it->first << "': '" << it->second->getUsername() << "'\n";
					else
							out << "\t'" << it->first << "': '(NULL User Pointer)'\n";
			}
	}
	return out;
}

/*
						====== Socket_creation ======
	we use socket() function.
		socket(int Domain, int type, int protocol);
	1)First Param:
		-int Domain -> use AF_INET -> {is an addres family,
			that is used to designate the type of address that your socket
			can communicate in this case IPV4(internet protocol V4)}
	2)Second Param: -int
		-type -> SOCK_STREAM -> {Provides sequenced, reliable two way connection
	based byte streams. it make sure that the data is not dublicated, it doesnt
	get lost and its delivered on correct order} 3) Third param: -int protocol ->
	0 -Protocol specifies a particular protocol to be used with the socke.
			Normally a single protocol exit to support the socket type
			within the given protocol family wich most of the times is 0}
	RETURN:
		SUCCESS: A file descriptor for the new socket created is
			being returned.
		ERROR: -1
*/
bool Server::createSocket() {
	listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == -1) {
		std::cerr << " Create Socket() failed " << std::endl;
		return false;
	} else
		return true;
}

/*

																================== BIND_SOCKET
	 =================== sockaddr_in this is a structure used  to represent IPv4
	 internet domain socket address. it is designed for IPv4 only and it contains
	 mambers for the family addres as:
				->address_family(sin_family) = should be set as AF_INET to use the IPv4
				->port_number(sin_port) = holdsthe portnumber in the network byte order
	 requiring conversion from the host byte order using fucntion hstons().
				->and IPv4 adress = this is the 32 bit IPv4 addres which it can be
	 assigned manually or by using functions like inet_addr()
				====>htons(uint16_t hostshort);
								->this function converts the unsigned short int hostshort from
	 host byte order to network byte order.
		===>bind() function assigns the address specified by the hinf to the socked
	 referred to by the file descriptor listening. bind(int sockfd, const struct
	 sockaddr *addr, socklen_t addrlen) Parameter: 1) sockfd - socket file
	 descriptor (listening) 2) addr - pointer to sockaddr structure (cast hint to
	 sockaddr *) 3) addrlen - size of the address structure (sizeof(hint)) RETURN:
								Success => 0
								ERROR => -1

*/


bool Server::bindSocket()
{
  //quick restarts
  int opt = 1;
  if(setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
  {
   std::cerr << "setsockopt reuseaddr failed\n"; 
  }
  // socket non-block
  if(fcntl(listening, F_SETFL, O_NONBLOCK) == -1)
  {
    std::cerr << "fcntl failed \n";
    close(listening);
    return false;
  }
  // bind socket to IP
  hint.sin_family = AF_INET;
  hint.sin_port = htons(port);
  hint.sin_addr.s_addr = INADDR_ANY;
  if ((bind(listening, (sockaddr *)&hint, sizeof(hint)) == -1)) {
    std::cerr << "Port failed to bind with an IP !" << std::endl;
    close(listening);
    return false;
  }
  return true;
}

/*
==================== LISTEN ====================
listen() -> this function marks the socket referred by sockedfd as a
passivesocket that will be used to accept incoming connection requests.

int listen(int sockfd, int backlog)
Parameters:
sockfd - the socket file descriptor (in my case listening vriable)
backlog - the maximum number of pending connections that can be queued

RETURN:
		success => 0
		ERROR => -1
*/
bool Server::listenSocket() {
	if (listen(listening, SOMAXCONN) == -1) {
		std::cerr << "Can't listen the socket" << std::endl;
		close(listening);
		return false;
	} else
		return true;
}

/*
===================== ACCEPT======================
the accept() function first extracts the first
connection request on the queue of pending connections for the listening
socket, creates a new connected socket, and returns the new fd referring to
that socket.

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen); Parameters:
	1) sockfd - listening socket file descriptor 2) addr -pointer to sockaddr
	struct to store client info 3) addrlen - pointer to size of addr struct

	RETURN:
					success => New socket file descriptor for the connection
					ERROR => -1
*/

void Server::accept_connection() {
  clientSize = sizeof(client);
  listening_fd.fd = listening;
  listening_fd.events = POLLIN;
  listening_fd.revents = 0;
  poll_fds.push_back(listening_fd);
}

/*
==================== POLL ======================
poll() function allows monitoring multiple file descriptors to see if I/O is
possible on any of them. It's essential for handling multiple clients
simultaneously. int poll(struct pollfd *fds, nfds_t nfds, int timeout);
poll() function allows monitoring multiple file descriptors to see if I/O is
possible on any of them. It's essential for handling multiple clients
simultaneously. int poll(struct pollfd *fds, nfds_t nfds, int timeout);
struct pollfd {
int   fd;         // file descriptor to monitor
hort events;     // events to monitor (POLLIN, POLLOUT, etc.)
hort revents;    // events that actually occurred
};
Parameters:
1) fds -> array of pollfd structures (file descriptors to monitor)
2) nfds -> number of file descriptors in the array
3) timeout -> timeout in milliseconds (-1 = infinite, 0 = don't block)
Events:
- POLLIN: Data available for reading
- POLLIN: Data available for reading
- POLLOUT: Ready for writing
- POLLHUP: Hang up (connection closed)
- POLLERR: Error condition
RETURN:
SUCCESS: Number of file descriptors with events
TIMEOUT: 0 (if timeout occurred)
ERROR: -1
*/
bool Server::init_poll() {
  if ((poll_count = poll(&poll_fds[0], poll_fds.size(), -1)) == -1) {
    std::cerr << "" << std::endl;
    return false;
  }
  return true;
}

/*
		=== EVENTS ===
*/
void Server::event_state() {
  incoming_data = false;
  client_hungup = false;
  err = false;
  is_listening = false;
}

void Server::event_check(size_t index) {
  // flags
  	event_state();

  if (poll_fds[index].revents & POLLIN)
    incoming_data = true;
  if (poll_fds[index].revents & POLLHUP)
    client_hungup = true;
  if (poll_fds[index].revents & POLLERR)
    err = true;
  if (poll_fds[index].fd == listening)
    is_listening = true;
}

void Server::handle_new_host()
{
  clientSize = sizeof(client);
	new_connection = accept(listening, (sockaddr *)&client, &clientSize);
	if (new_connection != -1)
  {
		if (fcntl(new_connection, F_SETFL, O_NONBLOCK) != -1)
    {
			User *user = new User(new_connection, std::string(inet_ntoa(client.sin_addr)));
			_users[new_connection] = user;
			poll_fds.push_back(user->getPoll());
			user->setState(WAITING_AUTH); // waiting state
      Server::sendCapabilities(*user);
    //  int auth_res = Server::authenticateParser(*user);
			//if (Server::authenticateParser(*user) == -1)
      // if(auth_res == -1)
      // {
			// 	// poll_fds.pop_back();
			// 	Server::closeConnection(user->getPoll().fd);
			// 	return;
			// }
      // else if(auth_res == 1){
      //   return; //need more data client didnt send everything yet we keep connection open
      // }
			// user->setState(VERIFIED);
			// Server::sendWelcome(*user);
		}
    else{
      std::cerr << "handle_new_host() making new_connection non-blocking failed" << std::endl;
      close(new_connection);
    }
	}
  else
  {
		std::cerr << "Failed accepting new connections" << std::endl;
		//close(new_connection);
	}
}

void Server::handle_messages(size_t index)
{ 
	User *user = _users[poll_fds[index].fd];
	memset(buff, 0, MAX_BUFF);
	bytes_recived = recv(poll_fds[index].fd, buff, MAX_BUFF - 1, 0);
	if (bytes_recived <= 0) {
		std::cout << "Client " << user->getPoll().fd << "(" << user->getHostname() << ") disconnected" << std::endl;
		close(user->getPoll().fd);
		poll_fds.erase(poll_fds.begin() + index);
		user->setHostname("_DISCONNECTED_");
    _users.erase(user->getPoll().fd);
		remove_from_vector(index);
	}
	buff[bytes_recived] = '\0';
	// std::cout << "Buff: '" << buff << "'" << std::endl;
	Server::parse(*user, buff);
	server::printChannels(_channels);
}

void Server::handle_disconn_err_hungup(size_t index) {
	User user = *_users[index];
	std::cout << "Client " << poll_fds[index].fd << "(" << user.getHostname() << ") error/hungup " << std::endl;
	close(poll_fds[index].fd);
	user.setHostname("_DISCONNECTED_");
	remove_from_vector(index);
}

bool Server::init_Server() {
	if (createSocket() == false) {
		std::cerr << "Failed to create socket\n";
    	return false;
	}
	if (bindSocket() == false) {
		std::cerr << "Failed to bind socket\n";
    	return false;
	}
	if (listenSocket() == false) {
    	std::cerr << "Failed to listen socket\n";
    	return false;
	}
	accept_connection();
	return true;
}

void Server::run_Server() {
	while (true) {
		if (init_poll() == false)
			break;
		for (size_t i = 0; i < poll_fds.size(); i++) {
			event_check(i);
			if (incoming_data) {
				if (is_listening)
					handle_new_host();
				else {
					handle_messages(i);
					poll_fds[i].revents = 0;
					i--;
					// server::printChannels(_channels);
				}
			}
			if ((client_hungup || err) && !is_listening) {
				handle_disconn_err_hungup(i);
				i--;
			}
		}
	}
}

