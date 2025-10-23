#include "../include/Server.hpp"
#include <iostream>
#include <sstream>
#include <string>

void Server::parse(User& user, std::string buff) {
	std::istringstream iss(buff);
	std::string line;

	// std::cout << user << std::endl;
	// std::cout << user.getUsername() + ": "<< "'" + buff + "'"<< std::endl;

	while (std::getline(iss, line)) {
        std::cout << "Line: '" << line << "'" << std::endl;
		if (!line.rfind("PING ", 0)) {
			Server::sendPong(&user, line);
		} else if (!line.rfind("JOIN #")) {
			server::handleJoin(_name, _channels, &user, line);
        } else if (!line.rfind("PART ")) {
			server::handlePart(_name, _channels, &user, line);
		} else if (!line.rfind("NICK ")) {
			Server::cmdNick(&user, line);
		} else if (!line.rfind("userhost ")) {
			// Server::cmdUser(&user, line); // Maybe change userName
			return;
		} else if (!line.rfind("WHOIS ")) {
			Server::cmdWhois(&user, line);
		} else if (!line.rfind("OPER ")) {
			Server::cmdOper(&user, line);
		} else if (!line.rfind("PRIVMSG ")) {
            server::handlePrivMsg(_channels, _users);
        }
	}
}

int Server::authenticateParser(User &user) {
    static char buffer[MAX_BUFF];
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
            line.erase(line.size() - 1, 1);
        if (!line.rfind("PASS ", 0)) {
            pass = line.substr(5);
            if (pass != _password) {
				Server::sendWrongPassword(user);
                return -1;
            }
        }
        else if (!line.rfind("NICK ", 0)) {
            std::string attemptedNick = line.substr(5);
            if (!isValidNick(attemptedNick)) {
                Error::ERRONEUSNICKNAME("myserver", user.getPoll().fd, attemptedNick);
                return -1;
			}

            if (isNickInUse(attemptedNick)) {
				Error::USERALREADYEXISTS(&user);
                return -1;
            }
            user.setNickname(attemptedNick);
        }
        else if (!line.rfind("USER ", 0)) {
            std::istringstream issUser(line.substr(5));
            std::string username, hostname, servername, realname;
            issUser >> username >> hostname >> servername;
            std::getline(issUser, realname); // realname can have spaces
            if (!realname.empty() && realname[0] == ':')
                realname.erase(0, 1);
            user.setUsername(username);
            user.setRealname(realname);
        }
    }

    if (pass.empty() || user.getNickname().empty() || user.getUsername().empty())
        return -1;

    user.setState(REGISTERED);
	Server::sendWelcome(user);

    return 0;
}
