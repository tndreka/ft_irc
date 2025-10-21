#include "../include/Server.hpp"
#include <iostream>
#include <string>

// void Server::cmdUser(&user, line) {
//
// 	std
// }

void Server::cmdOper(User *user, std::string line) {

	std::istringstream iss(line);
    std::string cmd, name, password;
    iss >> cmd >> name >> password;

    if (name.empty() || password.empty()) {
        std::string err = ":" + serverName + " 461 " + user->getNickname() + " OPER :Not enough parameters\r\n";
        send(user->getPoll().fd, err.c_str(), err.size(), 0);
        return;
    }

	if (name != IRC_OPER_NAME || password != IRC_OPER_PASS) {
		std::string err = ":" + serverName + " 464 " + user->getNickname() + " :Incorrect credentials\r\n";
        send(user->getPoll().fd, err.c_str(), err.size(), 0);
		return;
	}

	user->setAdmin(true);

	std::string msg = ":" + serverName + " 381 " + user->getNickname() + " :You are now an IRC operator\r\n";
	send(user->getPoll().fd, msg.c_str(), msg.size(), 0);

	std::string notice = ":" + serverName + " NOTICE * :Operator " + user->getNickname() + " has authenticated\r\n";
	broadcast_message(notice, *user);
}


void Server::cmdWhois(User *user, std::string line) {
	std::istringstream iss(line);
    std::string cmd, targetNick;
    iss >> cmd >> targetNick;

    if (targetNick.empty()) {
        std::string err = ":" + serverName + " 431 " + user->getNickname() + " :No nickname given\r\n"; // ERR_NONICKNAMEGIVEN
        send(user->getPoll().fd, err.c_str(), err.size(), 0);
        return;
    }

    // Find target user
    for (std::map<int, User*>::iterator it = _activeUsers.begin(); it != _activeUsers.end(); ++it) {
        User *target = it->second;
        if (target->getNickname() == targetNick) {
            std::string msg;

            // 311 - WHOISUSER
            msg = ":" + serverName + " 311 " + user->getNickname() + " " + targetNick + " "
                + target->getUsername() + " " + target->getHostname() + " * :"
                + target->getRealname() + "\r\n";
            send(user->getPoll().fd, msg.c_str(), msg.size(), 0);

            // 312 - WHOISSERVER
            msg = ":" + serverName + " 312 " + user->getNickname() + " " + targetNick + " " + serverName + " :The BIGGEST MALAKA Server\r\n";
            send(user->getPoll().fd, msg.c_str(), msg.size(), 0);


            // 318 - ENDOFWHOIS
            msg = ":" + serverName + " 318 " + user->getNickname() + " " + targetNick + " :End of /WHOIS list\r\n";
            send(user->getPoll().fd, msg.c_str(), msg.size(), 0);

            return;
        }
    }

    // ERR_NOSUCHNICK if not found
    std::string err = ":" + serverName + " 401 " + user->getNickname() + " " + targetNick + " :No such nick/channel\r\n";
    send(user->getPoll().fd, err.c_str(), err.size(), 0);
}


void Server::cmdNick(User *user, std::string line) {

	std::string oldNick = user->getNickname();
	std::string command, newNick;
	std::istringstream iss(line);

	iss >> command >> newNick;
	if (newNick.empty()) {
		return;
	}

	if (newNick[0] == ':')
		newNick = newNick.substr(1);
	
	user->setNickname(newNick);

	std::string msg = ":" + oldNick + "!" + user->getUsername() + "@" + user->getHostname()
		+ " NICK :" + newNick + "\r\n";

	std::cout <<msg.c_str() << std::endl;
	send(user->getPoll().fd, msg.c_str(), msg.size(), 0);
}
