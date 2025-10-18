#include "../include/Server.hpp"
#include <iostream>
#include <sstream>
#include <string>

void Server::parse(User& user, std::string buff) {
	std::istringstream iss(buff);
	std::string line;

	std::cout << user << std::endl;
	std::cout << user.getUsername() + ": "<< "'" + buff + "'"<< std::endl;

	while (std::getline(iss, line)) {
		if (!line.rfind("PING ", 0)) {
			Server::sendPong(&user, line);
		} else if (!line.rfind("JOIN #")) {
			return; // Create Channel
		} else if (!line.rfind("NICK ")) {
			Server::cmdNick(&user, line);
		} else if (!line.rfind("userhost ")) {
			// Server::cmdUser(&user, line); // Maybe change userName
			return;
		} else if (!line.rfind("WHOIS ")) {
			Server::cmdWhois(&user, line);
		} else if (!line.rfind("OPER ")) {
			Server::cmdOper(&user, line);
		}
	}
}

std::string Server::authenticateNickname(User &user, std::string line) {

	std::istringstream iss(line);
	std::string arg, nickname;
	iss >> arg >> nickname;


	std::cout << arg << nickname << std::endl;
	if (nickname.empty() || nickname.length() > 32) {
		// Error msg
		return "";
	}

	if (nickname[0] > '}' || nickname[0] < 'A') {
		//Error
		return "";
	}

	for (size_t i = 1; nickname[i]; ++i) {
		if ((nickname[i] > '}' || nickname[i] < 'A') && (nickname[i] > '9' || nickname[i] < '0')) {
			//Error 
			return "";
		}
	}

	for (std::map<int, User*>::iterator it = _activeUsers.begin(); it != _activeUsers.end(); ++it) {
		if (it->second->getNickname() == nickname) {
			Errors::NICKNAMEINUSE(serverName, user.getPoll().fd, nickname);
			return "";
		}
	}

	return nickname;
}

int Server::authenticateParser(User& user) {

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
			user.setNickname(authenticateNickname(user, line));
		} else if (line.rfind("USER ", 0) == 0) {
			std::istringstream issUser(line.substr(5));
			std::string username, hostname, reallarg, firstName, lastName;
			issUser >> username >> hostname >> reallarg >> firstName >> lastName;
			
			if (firstName[0] == ':')
				firstName = firstName.substr(1);
			user.setRealname(firstName + " " + lastName);
			user.setUsername(username);
		}
	}
	if (pass.empty() || user.getNickname().empty() || user.getUsername().empty())
		return -1;
	user.setState(REGISTERED);
	return 0;
}


