#include "../include/Server.hpp"

void Server::parse(User& user, std::string buff) {
	std::cout << "DEBUG: parse() started for user: " << user.getNickname() << std::endl;
    
	std::istringstream iss(buff);
	std::string line;

	if(user.getState() == WAITING_AUTH)
	{
		 std::cout << "DEBUG: User in WAITING_AUTH state" << std::endl;
    
		int auth_res = authenticateParser(user, buff);
		std::cout << "DEBUG: authenticateParser returned: " << auth_res << std::endl;
        
		if(auth_res == 0)
		{
			user.setState(VERIFIED);
			sendWelcome(user);
		}
		else if(auth_res == -1)
		{
			std::cout << "DEBUG: Authentication failed, cleaning up" << std::endl;

			sendWrongPassword(user);
			// server::handleQuit(*this, )
			int fd = user.getPoll().fd;

			//close(user.getPoll().fd);
			for(size_t i = 0; i < poll_fds.size(); ++i)
			{
				if(poll_fds[i].fd == fd)
				{
					poll_fds.erase(poll_fds.begin() + i);
					break;
				}
			}
			close(fd);
			_users.erase(fd);
			return;
		}
		std::cout << "DEBUG: parse() authentication complete" << std::endl;
 
		return;
	}

	if(user.getState() != VERIFIED && user.getState() != REGISTERED)
	{
   std::cout << "DEBUG: User not verified/registered, returning" << std::endl;
   
		return;
	}
	
	 std::cout << "DEBUG: Processing verified user messages" << std::endl;

	server::printUsers(_users);
	while (std::getline(iss, line)) {
         std::cout << "Line: '" << line << "'" << std::endl;
		if (!line.find("PING ", 0)) {
			Server::sendPong(&user, line);
		} else if (!line.find("JOIN #")) {
			server::handleJoin(*this, &user, line);
        } else if (!line.find("PART ")) {
			server::handlePart(*this, &user, line);
		} else if (!line.find("NICK ")) {
			Server::cmdNick(&user, line);
		} else if (!line.find("WHOIS ")) {
			Server::cmdWhois(&user, line);
		} else if (!line.find("OPER ")) {
			Server::cmdOper(&user, line);
		} else if (!line.find("KICK ")) {
			Server::channelKick(&user, line.substr(5));
		} else if (!line.find("PRIVMSG ")) {
            server::handlePrivMsg(*this, user, line);
        } else if (!line.find("QUIT")) {
            server::handleQuit(*this, user);
        } else if (!line.find("TOPIC #")) {
			Server::channelTopic(&user, line.substr(7));
		} else if (!line.find("MODE #")) {
			Server::channelMode(&user, line.substr(6));
		}
	}
	 std::cout << "DEBUG: parse() completed successfully" << std::endl;
}

std::string Server::authenticateNickname(User &user, std::string line) {

	std::istringstream iss(line);
	std::string arg, nickname;
	iss >> arg >> nickname;


	if (nickname.empty() || nickname.length() > 32) {
		Error::ERRONEUSNICKNAME(_name, user.getPoll().fd, nickname);
		return "";
	}
	if (nickname[0] > '}' || nickname[0] < 'A') {
		Error::ERRONEUSNICKNAME(_name, user.getPoll().fd, nickname);
		return "";
	}

	for (size_t i = 1; i < nickname.size(); ++i) {
		if ((nickname[i] > '}' || nickname[i] < 'A') && (nickname[i] > '9' || nickname[i] < '0')) {
			Error::ERRONEUSNICKNAME(_name, user.getPoll().fd, nickname);
			return "";
		}
	}

	for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
		if (it->second->getNickname() == nickname) {
			Error::NICKNAMEINUSE(user, _name, nickname);
			return "";
		}
	}

	return nickname;
}

int Server::authenticateParser(User& user, std::string buff) {

	std::istringstream iss(buff);
	std::string line;
	std::string pass;


	while (std::getline(iss, line)) {
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.find("PASS ") == 0) {
			pass = line.substr(5);
			if (pass != _password) {
				return -1;
			}
			user.setPassVerified(true);
		} else if (line.find("NICK ", 0) == 0) {
			user.setNickname(authenticateNickname(user, line));
		} else if (line.find("USER ", 0) == 0) {
			std::istringstream issUser(line.substr(5));
			std::string username, hostname, realarg, firstName, lastName;
			issUser >> username >> hostname >> realarg >> firstName >> lastName;
			
			if (firstName[0] == ':')
				firstName = firstName.substr(1);
			user.setRealname(firstName + " " + lastName);
			user.setUsername(username);
		}
	}
	if (!user.isPassVerified() || user.getNickname().empty() || user.getUsername().empty())
		return 1;
	user.setState(REGISTERED);
	return 0;
}
