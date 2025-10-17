#include "../include/Server.hpp"

void Server::parse(User& user, std::string buff) {
	std::istringstream iss(buff);
	std::string line;

	// std::cout << user.getUsername() + ": "<< "'" + buff + "'"<< std::endl;

	while (std::getline(iss, line)) {
		if (line.rfind("PING ", 0) == 0)
		Server::sendPong(&user, line);
		else if (line.rfind("NICK #") == 0) {
			// Server::cmdNick(&user, line);
			std::cout << "Nickname TODO" << std::endl;
			return;
		}
		else if (line.rfind("JOIN #") == 0)
			channels::handleJoin(_channels, &user, line);
			return;
	}
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
			if (pass != _password) {
				Server::sendWronPassword(user);
				return -1;
			}
		} else if (line.rfind("NICK ", 0) == 0) {
			user.setNickname(line.substr(5));
			// std::cout << "===>Grabed Nickname" << std::endl;
		} else if (line.rfind("USER ", 0) == 0) {
			std::istringstream issUser(line.substr(5));
			std::string username;
			issUser >> username;
			user.setUsername(username);
			// std::cout << "===>Grabed USer" << std::endl;
		}
	}
	if (pass.empty() || user.getNickname().empty() || user.getUsername().empty())
		return -1;
	user.setState(REGISTERED);
	return 0;
}


