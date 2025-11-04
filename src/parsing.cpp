#include "../include/Server.hpp"

void Server::parse(User& user, const std::string& line) {
 
	if(user.getState() == WAITING_AUTH)
	{
		int auth_res = authenticateParser(user, line);
		if(auth_res == 0) {
			sendWelcome(user);
		}
		else if(auth_res == -1) {
			sendWrongPassword(user);
			int fd = user.getPoll().fd;
			close(fd);
			return;
		}
		return;
	}

	if(user.getState() != REGISTERED) {
		return;
	}

    if (!line.compare(0, 5, "PING "))
        Server::sendPong(&user, line);
    else if (!line.compare(0, 5, "JOIN "))
        server::handleJoin(*this, &user, line);
    else if (!line.compare(0, 5, "PART "))
        server::handlePart(*this, &user, line);
    else if (!line.compare(0, 5, "NICK "))
        Server::cmdNick(&user, line);
    else if (!line.compare(0, 6, "WHOIS "))
        Server::cmdWhois(&user, line);
    else if (!line.compare(0, 5, "OPER "))
        Server::cmdOper(&user, line);
    else if (!line.compare(0, 5, "KICK "))
        Server::channelKick(&user, line.substr(5));
    else if (!line.compare(0, 8, "PRIVMSG ") || line.compare(0, 7, "NOTICE "))
        server::handlePrivMsg(*this, user, line);
    else if (!line.compare(0, 4, "QUIT"))
        server::handleQuit(*this, user);
    else if (!line.compare(0, 6, "TOPIC "))
        Server::channelTopic(&user, line.substr(6));
    else if (!line.compare(0, 5, "MODE "))
        Server::channelMode(&user, line.substr(5));
    else if (!line.compare(0, 7, "INVITE "))
        Server::cmdInvite(&user, line.substr(7));
}


std::string Server::authenticateNickname(User &user, std::string line) {

	std::istringstream iss(line);
	std::string arg, nickname;
	iss >> arg >> nickname;


	if (nickname.empty() || nickname.length() > 32) {
		error::registration::ERRONEUSNICKNAME(_name, user.getPoll().fd, nickname);
		return "";
	}
	if (nickname[0] > '}' || nickname[0] < 'A') {
		error::registration::ERRONEUSNICKNAME(_name, user.getPoll().fd, nickname);
		return "";
	}

	for (size_t i = 1; i < nickname.size(); ++i) {
		if ((nickname[i] > '}' || nickname[i] < 'A') && (nickname[i] > '9' || nickname[i] < '0')) {
			error::registration::ERRONEUSNICKNAME(_name, user.getPoll().fd, nickname);
			return "";
		}
	}

	for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
		if (it->second->getNickname() == nickname) {
			error::registration::NICKNAMEINUSE(user, _name, nickname);
			return "";
		}
	}

	return nickname;
}

int Server::authenticateParser(User& user, const std::string& line) {


	std::string pass;
	if (!line.find("PASS ")) {
		pass = line.substr(5);
		if (pass != _password) {
			return -1;
		}
		user.setPassVerified(true);
	} else if (!line.find("NICK ", 0)) {
		std::string nick = authenticateNickname(user, line);
		if (!nick.empty()) {
			user.setNickname(nick);
		}
	} else if (!line.find("USER ", 0)) {
		std::istringstream issUser(line.substr(5));
		std::string username, hostname, realarg, firstName, lastName;
		issUser >> username >> hostname >> realarg >> firstName >> lastName;
		
		if (firstName[0] == ':')
			firstName = firstName.substr(1);
		user.setRealname(firstName + " " + lastName);
		user.setUsername(username);
	}
	if (user.getIsPassVerified() && !user.getNickname().empty() && !user.getUsername().empty()) {
		user.setState(REGISTERED);
		return 0;
	}
	return 1;
}
