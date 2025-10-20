/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_helpers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 16:28:32 by tndreka           #+#    #+#             */
/*   Updated: 2025/10/20 23:34:04 by tndreka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cerrno>

void Server::closeConnection(int fd) {
	std::map<int, User*>::iterator it = _activeUsers.find(fd);
	if (it != _activeUsers.end()) {
		User *user = it->second;
		std::cout << "Closing connection for " << user->getNickname()
					<< " (" << user->getHostname() << ")\n";

		user->setHostname("_DISCONNECTED_");

		for (size_t i = 0; i < poll_fds.size(); ++i) {
			if (poll_fds[i].fd == fd) {
				poll_fds.erase(poll_fds.begin() + i);
				break;
			}
		}
		close(fd);
		delete user;
		_activeUsers.erase(it);
	} else {
		close(fd);
	}
}

void Server::remove_from_vector(size_t index) {
  if (index >= poll_fds.size())
    return;
  if (index < poll_fds.size() - 1)
    poll_fds[index] = poll_fds[poll_fds.size() - 1];
  poll_fds.pop_back();
}

bool Server::set_Port(const std::string &port)
{
  int portNum;
  char *end = 0;
  errno = 0;
  
  portNum = std::strtol(port.c_str(), &end, 10);
  //portNum = atoi(port.c_str());
  if(*end != '\0')
  {
    std::cerr << "Invalid port: contains non digit charcters\n";
    return false;
  }
  if (errno == ERANGE || portNum < 1024 || portNum > 65535)
  {
    std::cerr << "Invalid range!! Port number must be between 1024 - 65535\n";
    return false;
  }
  this->port = portNum;
  return true;
}

bool Server::set_Pass(const std::string &pass) {
  if (pass.empty()) {
    std::cerr << "Password can not be empty\n";
    return false;
  } else {
    this->password = pass;
    return true;
  }
}
