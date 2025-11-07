/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 15:22:24 by tndreka           #+#    #+#             */
/*   Updated: 2025/10/28 22:48:41 by tndreka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <iostream>
#include <unistd.h>
#include <vector>

Server::Server() : _name("MalakaIRC"), _users(), _channels(), _pollFds() {}

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

const std::string&	Server::getName() const {
	return (_name);
}

const std::string&	Server::getPass() const {
	return (_password);
}

const std::map<int, User*>&	Server::getUsers(void) const {
	return (_users);
}

const std::vector<Channel*>&	Server::getChannels(void) const {
	return (_channels);
}

void	Server::addChannel(Channel* new_channel) {
	_channels.push_back(new_channel);
}

void	Server::deleteChannel(User* user, const std::string& name) {
	Channel*			channel_to_delete;
	const std::string	prefix = ":" + _name + " NOTICE " + user->getNickname();
	const std::string	postfix = "\r\n";
	std::string 		msg;

	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		if ((*it)->getName() == name)
			channel_to_delete = *it;
	}
	std::vector<Channel*>::iterator new_end = std::remove(_channels.begin(), _channels.end(), channel_to_delete);
    _channels.erase(new_end, _channels.end());
	msg = prefix + ": Channel '" + channel_to_delete->getName() + "' deleted successfully!" + postfix; 
	send(user->getPoll().fd, msg.c_str(), msg.size(), 0);
	delete channel_to_delete;
}

void	Server::clearChannels(void) {
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		delete *it;
		*it = NULL;
	}
	_channels.clear();
}

void	Server::clearUsers(void) {
	for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
		const std::string	msg = ":" + it->second->getNickname() + "!" + it->second->getUsername() + "@" + it->second->getHostname() + " QUIT\r\n";
		send(it->first, msg.c_str(), msg.size(), 0);
		close(it->first);
		delete it->second;
		it->second = NULL;
	}
	_users.clear();
}

std::ostream&	operator<<(std::ostream& out, const Server& obj) {
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

/**
			====== Socket_creation ======
	we use socket() function. socket(int Domain, int type, int protocol);
	1)First Param:
		-int Domain -> use AF_INET -> {is an addres family,
			that is used to designate the type of address that your socket
			can communicate in this case IPV4(internet protocol V4)}
	2)Second Param: -int
		-type -> SOCK_STREAM -> {Provides sequenced, reliable two way connection
		based byte streams. it make sure that the data is not dublicated, it doesnt
		get lost and its delivered on correct order}
	3) Third param: -int protocol -> 0 
		-Protocol specifies a particular protocol to be used with the socket.
			Normally a single protocol exit to support the socket type
			within the given protocol family wich most of the times is 0}
	RETURN:
		SUCCESS: A file descriptor for the new socket created is
			being returned.
		ERROR: -1
*/
bool	Server::createSocket() {
	listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == -1) {
		std::cerr << " Create Socket() failed " << std::endl;
		return false;
	} else
		return true;
}

/**
		================== BIND_SOCKET=================== 
	sockaddr_in this is a structure used  to represent IPv4
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
bool	Server::bindSocket()
{
	int opt = 1;
		if(setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::cerr << "setsockopt reuseaddr failed\n";
	}

	if(fcntl(listening, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "fcntl failed \n";
		close(listening);
		return false;
	}

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

/**
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
	}
	return true;
}

/**
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
  _pollFds.push_back(listening_fd);
}

/**
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
- RETURN:
- SUCCESS: Number of file descriptors with events
- TIMEOUT: 0 (if timeout occurred)
- ERROR: -1
*/
bool Server::init_poll() {
	int poll_count;

	if ((poll_count = poll(&_pollFds[0], _pollFds.size(), -1)) == -1) {
		std::cerr << "" << std::endl;
		return false;
	}
	return true;
}

void Server::event_state() {
	incoming_data = false;
	client_hungup = false;
	err = false;
	is_listening = false;
}

void Server::event_check(size_t index) {
	event_state();

	if (_pollFds[index].revents & POLLIN)
		incoming_data = true;
	if (_pollFds[index].revents & POLLHUP)
		client_hungup = true;
	if (_pollFds[index].revents & POLLERR)
		err = true;
	if (_pollFds[index].fd == listening)
		is_listening = true;
}

void Server::handle_new_host()
{
	int new_connection;

	clientSize = sizeof(client);
	new_connection = accept(listening, (sockaddr *)&client, &clientSize);

	if (new_connection == -1) {
		std::cerr << "Failed accepting new connections" << std::endl;
		close(new_connection);
		return;
	}

	if (fcntl(new_connection, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "handle_new_host() making new_connection non-blocking failed" << std::endl;
		close(new_connection);
		return;
	}

	User *user = new User(new_connection, std::string(inet_ntoa(client.sin_addr)));
	_users[new_connection] = user;
	_pollFds.push_back(user->getPoll());
	user->setState(WAITING_AUTH);
	Server::sendCapabilities(*user);
}

void Server::handle_messages(size_t index)
{
	if (index >= _pollFds.size())
		return;

	int fd = _pollFds[index].fd;
	User* user = _users[fd];
	if (!user)
		return;

	char local_buf[MAX_BUFF];
	ssize_t bytes_received = recv(fd, local_buf, sizeof(local_buf) - 1, MSG_DONTWAIT);

	if (bytes_received <= 0) {
		// Server::handle_disconn_err_hungup(index);
		return;
	}

	local_buf[bytes_received] = '\0';
	user->appendToBuffer(local_buf);

	std::string& userBuffer = user->getUserBuffer();
	size_t start = 0;
	size_t pos;

	while (!userBuffer.empty() && (pos = userBuffer.find("\r\n", start)) != std::string::npos) {
		std::string command = userBuffer.substr(start, pos - start);
		if (!command.compare(0, 4, "QUIT"))
			return (server::handleQuit(*this, *user));
		if (!command.empty())
			Server::parse(*user, command);
		start = pos + 2;
	}

	if (start > 0)
		userBuffer.erase(0, start);
}

void Server::handle_disconn_err_hungup(size_t index) {
	if (index >= _pollFds.size())
		return;
	int fd = _pollFds[index].fd;
	User *user = _users[fd];
	if(!user)
		return;
	std::cout << "Handle disc" << std::endl;
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ) {
		if (user::isAlreadyConnected(**it, *user)) {
			(*it)->removeMember(*user, _name, false);
			channel::goodbyeUser(**it, *user);
			if ((*it)->getNumOfUsers() == 0) {
				Channel* ch = *it;
				it = _channels.erase(it);
				delete ch;
				continue;
			}
		}
		++it;
	}
	removeUser(fd);
	// close(fd);
	// _users.erase(fd);
	// delete user;
	// if (index != _pollFds.size() - 1) {
	// 	_pollFds[index] = _pollFds.back();
	// }
	// _pollFds.pop_back();
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

void Server::shutdownCleanly() {
    clearChannels();
    clearUsers();
    if (listening != -1) {
        close(listening);
        listening = -1;
    }
}

void Server::run_Server() {
	while (!signal_flag) {
		int poll_count = poll(_pollFds.data(), _pollFds.size(), -1);
		if (poll_count == -1) {
			if (errno == EINTR)
				continue;
			perror("poll");
			break;
		}



		for (size_t i = 0; i < _pollFds.size(); ++i) {
			int fd = _pollFds[i].fd;
			short revents = _pollFds[i].revents;
			
			std::cout << "Events (" << fd << "): " << revents << std::endl;
			if (revents == 0)
				continue;

			if (fd == listening && (revents & POLLIN)) {
				handle_new_host();
				continue;
			}

			// if (revents & (POLLERR | POLLHUP | POLLNVAL)) {
			// 	std::cout << "Disc loop" << std::endl;
			// 	handle_disconn_err_hungup(i);
			// 	--i;
			// 	continue;
			// }

			if (revents & POLLIN) {
				std::cout << "handle msgs" << std::endl;
				handle_messages(i);
			}
		}
	}

	Server::shutdownCleanly();
}
