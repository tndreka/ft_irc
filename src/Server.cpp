/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 15:22:24 by tndreka           #+#    #+#             */
/*   Updated: 2025/10/19 22:41:05 by tndreka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <string>
#include <unistd.h>
#include "../include/Server.hpp"
#include <string>
#include <unistd.h>

Server::Server() : serverName("MalakaIRC") {}

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

std::string Server::getServerName() const {
    return serverName;
}

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


bool Server::bindSocket() {
  // socket non-block
  fcntl(listening, F_SETFL, O_NONBLOCK);
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

int Server::parser_irc(User& user) {

	char buffer[MAX_BUFF];
	memset(buffer, 0, MAX_BUFF);
	int bytes_received = recv(user.getPoll().fd, buffer, MAX_BUFF - 1, 0);
	if (bytes_received <= 0)
	return -1;

	buffer[bytes_received] = '\0';

	std::string msg(buffer);
	std::istringstream iss(msg);
	std::string line;
	std::string pass;

	// std::cout << "Received from " << user.getPoll().fd << ": " << msg << std::endl;

	while (std::getline(iss, line)) {
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.rfind("PASS ") == 0) {
			pass = line.substr(5);
			if (pass != password) {
				Server::sendWronPassword(user);
				return -1;
			}
		} else if (line.rfind("NICK ", 0) == 0) {
			user.setNickname(line.substr(5));
			std::cout << "===>Grabed Nickname" << std::endl;
		} else if (line.rfind("USER ", 0) == 0) {
			user.setUsername(line.substr(5));
			std::cout << "===>Grabed USer" << std::endl;
		} else if (line.rfind("PING ", 0) == 0) {
			std::string pong = "PONG " + line.substr(5) + "\r\n";
			std::cout << pong << std::endl;
			std::cout << "===>Send PONG" << std::endl;
			send(user.getPoll().fd, pong.c_str(), pong.size(), 0);
		}
	}
	if (pass.empty() || user.getNickname().empty() || user.getUsername().empty())
		return -1;
	user.setState(REGISTERED);
	return 0;
}

void Server::handle_new_host() {
	new_connection = accept(listening, (sockaddr *)&client, &clientSize);
	if (new_connection != -1) {
		if (fcntl(new_connection, F_SETFL, O_NONBLOCK) != -1) {
			User *user = new User(new_connection, std::string(inet_ntoa(client.sin_addr))); //allocation
			_activeUsers[new_connection] = *user;
			poll_fds.push_back(user->getPoll());
			Server::sendCapabilities(*user);
			if (Server::parser_irc(*user) == -1) {
				poll_fds.pop_back();
				return;
			}
			user->setState(VERIFIED);
			Server::sendWelcome(*user);
		} else
			std::cerr << "handle_new_host() making new_connection non-blocking failed"
					<< std::endl;
	} else {
		std::cerr << "Failed accepting new connections" << std::endl;
		close(new_connection);
	}
}

void Server::handle_messages(size_t index) {
	//User user = _activeUsers[index];
	User user = _activeUsers[poll_fds[index].fd]; // line above accessing wrong user
  memset(buff, 0, MAX_BUFF);
	bytes_recived = recv(poll_fds[index].fd, buff, MAX_BUFF - 1, 0);
	if (bytes_recived > 0) {
		std::cout << "Recived from " << user.getHostname() << ": " << buff;
		buff[bytes_recived] = '\0';
		// send(poll_fds[index].fd, buff, bytes_recived, 0);
		std::string message = user.getHostname() + ": " + std::string(buff);
		std::cout << message << std::endl;
		// broadcast_message(message, poll_fds[index].fd);
	} else if (bytes_recived <= 0) {
		std::cout << "Client " << user.getPoll().fd << "("
					<< user.getHostname() << ") disconnected" << std::endl;
		close(user.getPoll().fd);
		poll_fds.erase(poll_fds.begin() + index);
		user.setHostname("_DISCONNECTED_");
		remove_from_vector(index);
	}
}

void Server::handle_disconn_err_hungup(size_t index) {
	//User user = _activeUsers[index];
  User user = _activeUsers[poll_fds[index].fd];
	std::cout << "Client " << poll_fds[index].fd << "(" << user.getHostname() << ") error/hungup " << std::endl;
	close(poll_fds[index].fd);
	user.setHostname("_DISCONNECTED_");
	remove_from_vector(index);
}

int Server::init_Server() {
  if (createSocket() == false) {
    std::cerr << "Failed to create socket\n";
    return 1;
  }
  if (bindSocket() == false) {
    std::cerr << "Failed to bind socket\n";
    return 1;
  }
  if (listenSocket() == false) {
    std::cerr << "Failed to listen socket\n";
    return 1;
  }
  accept_connection();
  return 0;
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
        }
      }
      if ((client_hungup || err) && !is_listening) {
        handle_disconn_err_hungup(i);
        i--;
      }
    }
  }
}
