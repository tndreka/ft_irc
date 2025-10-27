#include "../include/Server.hpp"

/**
 * @note why rfind and not find: /join '#ch1' ????
 */
void Server::parse(User& user, std::string buff) {
	std::istringstream iss(buff);
	std::string line;

	// std::cout << user << std::endl;
	// std::cout << user.getUsername() + ": "<< "'" + buff + "'"<< std::endl;
	if(user.getState() == WAITING_AUTH)
	{
		int auth_res = authenticateParser(user, buff);
		if(auth_res == 0)
		{
			user.setState(VERIFIED);
			sendWelcome(user);
		}
		else if(auth_res == -1)
		{
			sendWrongPassword(user);

			// closeConnection(user.getPoll().fd);
			close(user.getPoll().fd);
			for(size_t i = 0; i < poll_fds.size(); ++i)
			{
				if(poll_fds[i].fd == user.getPoll().fd)
				{
					poll_fds.erase(poll_fds.begin() + i);
					break;
				}
			}
			_users.erase(user.getPoll().fd);
			return;
		}
		return;
	}
	if(user.getState() != VERIFIED && user.getState() != REGISTERED)
		return;
	
	while (std::getline(iss, line)) {
        // std::cout << "Line: '" << line << "'" << std::endl;
		if (!line.rfind("PING ", 0)) {
			Server::sendPong(&user, line);
		} else if (!line.rfind("JOIN #")) {
			server::handleJoin(*this, &user, line);
        } else if (!line.rfind("PART ")) {
			server::handlePart(*this, &user, line);
		} else if (!line.rfind("NICK ")) {
			Server::cmdNick(&user, line);
		} else if (!line.rfind("userhost ")) {
			// Server::cmdUser(&user, line); // Maybe change userName
			return;
		} else if (!line.rfind("WHOIS ")) {
			Server::cmdWhois(&user, line);
		} else if (!line.rfind("OPER ")) {
			Server::cmdOper(&user, line);
		} else if (!line.rfind("KICK ")) {
			Server::channelKick(&user, line.substr(5));
		} else if (!line.rfind("PRIVMSG ")) {
            server::handlePrivMsg(*this, user, line);
        } else if (!line.rfind("QUIT")) {
            server::handleQuit(*this, user);
        }
	}
	for (std::vector<Channel*>::const_iterator it = _channels.begin(); it != _channels.end(); ++it) {
		channel::printMembers(**it);
	}
	std::cout << std::endl;
}

std::string Server::authenticateNickname(User &user, std::string line) {

	std::istringstream iss(line);
	std::string arg, nickname;
	iss >> arg >> nickname;


	// std::cout << arg << nickname << std::endl;
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

	for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
		if (it->second->getNickname() == nickname) {
			// Error::NICKNAMEINUSE(_name, user.getPoll().fd, nickname);
			Error::NICKNAMEINUSE(user, _name, nickname);
			return "";
		}
	}

	return nickname;
}

int Server::authenticateParser(User& user, std::string buff) {

	// char buffer[MAX_BUFF];
	// memset(buffer, 0, MAX_BUFF);
	// int bytes_received = recv(user.getPoll().fd, buffer, MAX_BUFF - 1, 0);
	// if (bytes_received <= 0)
	// 	return -1;
	// if(bytes_received == 0){
	// 	return -1;
	// }
	//buffer[bytes_received] = '\0';
	//std::string msg(buffer);
	std::istringstream iss(buff);
	std::string line;
	std::string pass;


	while (std::getline(iss, line)) {
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.rfind("PASS ") == 0) {
			pass = line.substr(5);
			if (pass != _password) {
				Server::sendWrongPassword(user);
				return -1;
			}
			user.setPassVerified(true);
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
	if (!user.isPassVerified() || user.getNickname().empty() || user.getUsername().empty())
		return 1;
//	std::cout<< "=====================> yep\n";
	user.setState(REGISTERED);
	return 0;
}
