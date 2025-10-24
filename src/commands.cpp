#include "../include/Server.hpp"

void Server::channelKick(const User* u, const std::string& line) {

	std::istringstream iss(line);
	std::string channel, target;
	iss >> channel >> target;

	if (!u->getIsAdmin()) {
		Error::CHANOPRIVSNEEDED(u, _name, channel);
		return;
	}

	channel.erase(0,1);
	Channel *c = server::getChannelFromList(_channels,  channel);
	User *t = server::getUserFromList(_users, target);

	if (!user::isAlreadyConnected(*c, *t)) {
		Error::USERNOTINCHANNEL(u, _name, target, channel);
		return;
	}

	std::string message = ":" + u->getNickname() + "!" + u->getUsername() + "@"
		+ u->getHostname() + " KICK #" + channel + " " + target + " :Bye bye Malaka\r\n";
	Server::broadcastChannel(*c, message);

	// channel::goodbyeUser(*c, *t);
}

void Server::cmdOper(User *user, std::string line) {

	std::istringstream iss(line);
    std::string cmd, name, _password;
    iss >> cmd >> name >> _password;

    if (name.empty() || _password.empty()) {
        std::string err = ":" + _name + " 461 " + user->getNickname() + " OPER :Not enough parameters\r\n";
        send(user->getPoll().fd, err.c_str(), err.size(), 0);
        return;
    }

	if (name != IRC_OPER_NAME || _password != IRC_OPER_PASS) {
		std::string err = ":" + _name + " 464 " + user->getNickname() + " :Incorrect credentials\r\n";
        send(user->getPoll().fd, err.c_str(), err.size(), 0);
		return;
	}

	user->setAdmin(true);

	std::string msg = ":" + _name + " 381 " + user->getNickname() + " :You are now an IRC operator\r\n";
	send(user->getPoll().fd, msg.c_str(), msg.size(), 0);

	std::string notice = ":" + _name + " NOTICE * :Operator " + user->getNickname() + " has authenticated\r\n";
	broadcast_message(notice, *user);
}


void Server::cmdWhois(User *user, std::string line) {
	std::istringstream iss(line);
    std::string cmd, targetNick;
    iss >> cmd >> targetNick;

    if (targetNick.empty()) {
        std::string err = ":" + _name + " 431 " + user->getNickname() + " :No nickname given\r\n"; // ERR_NONICKNAMEGIVEN
        send(user->getPoll().fd, err.c_str(), err.size(), 0);
        return;
    }

    // Find target user
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        User *target = it->second;
        if (target->getNickname() == targetNick) {
            std::string msg;

            // 311 - WHOISUSER
            msg = ":" + _name + " 311 " + user->getNickname() + " " + targetNick + " "
                + target->getUsername() + " " + target->getHostname() + " * :"
                + target->getRealname() + "\r\n";
            send(user->getPoll().fd, msg.c_str(), msg.size(), 0);

            // 312 - WHOISSERVER
            msg = ":" + _name + " 312 " + user->getNickname() + " " + targetNick + " " + _name + " :The BIGGEST MALAKA Server\r\n";
            send(user->getPoll().fd, msg.c_str(), msg.size(), 0);


            // 318 - ENDOFWHOIS
            msg = ":" + _name + " 318 " + user->getNickname() + " " + targetNick + " :End of /WHOIS list\r\n";
            send(user->getPoll().fd, msg.c_str(), msg.size(), 0);

            return;
        }
    }

    // ERR_NOSUCHNICK if not found
    std::string err = ":" + _name + " 401 " + user->getNickname() + " " + targetNick + " :No such nick/channel\r\n";
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
