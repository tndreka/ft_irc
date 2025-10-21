/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_helpers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 16:28:32 by tndreka           #+#    #+#             */
/*   Updated: 2025/10/01 16:46:07 by tndreka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void Server::removeUser(int fd) {

	shutdown(fd, SHUT_WR);


	for (size_t i = 0; i < poll_fds.size(); ++i) {
		if (poll_fds[i].fd == fd) {
			poll_fds.erase(poll_fds.begin() + i);
			break;
		}
	}

	std::map<int, User*>::iterator it = _activeUsers.find(fd);
	if (it != _activeUsers.end()) {
		delete it->second;
		_activeUsers.erase(it);
	}

	close(fd);
}

bool Server::isValidNick(std::string& attemptedNick) {

	if (attemptedNick.empty() || attemptedNick.length() > 32) {
		return false;
	}
	for (size_t i = 1; attemptedNick[i]; ++i) {
		if ((attemptedNick[i] > '}' || attemptedNick[i] < 'A') && (attemptedNick[i] > '9' || attemptedNick[i] < '0')) {
			return false;
		}
	}
	return true;
}

bool Server::isUserAlreadySigned(User& user) {
	for (std::map<int, User*>::iterator it = _activeUsers.begin(); it != _activeUsers.end(); ++it) {
    	User* u = it->second;
    	if (!u) continue;
    	if (u->getUsername() == user.getUsername() && u->getHostname() == user.getHostname())
			return true;
	}
	return false;
}

bool Server::isNickInUse (std::string& attemptedNick) {

    for (std::map<int, User*>::iterator it = _activeUsers.begin(); it != _activeUsers.end(); ++it) {
        User *existing = it->second;
        if (!existing)
            continue;

        if (existing->getNickname() == attemptedNick) {
			return true;
        }
    }
	return false;
}

void Server::remove_from_vector(size_t index) {
  if (index >= poll_fds.size())
    return;
  if (index < poll_fds.size() - 1)
    poll_fds[index] = poll_fds[poll_fds.size() - 1];
  poll_fds.pop_back();
}

bool Server::set_Port(const std::string &port) {
  int portNum;

  portNum = atoi(port.c_str());
  if (portNum < 1024 || portNum > 65535) {
    std::cerr << "Invalid range!! Port number must be between 1024 - 65535\n";
    return false;
  } else {
    this->port = portNum;
    return true;
  }
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
