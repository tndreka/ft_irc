#include "../include/Server.hpp"

static bool isAllDigits(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!std::isdigit(static_cast<unsigned char>(*it)))
            return false;
    }
    return true;
}

void Server::channelMode(const User *user, const std::string& line) {

	std::istringstream iss(line);
	std::string channel, modes;

	iss >> channel >> modes;

	// std::cout << "channel : '" << channel << "'" << std::endl;
	// if (!line.find("MODE #" + channel))
	// 	return;

	Channel* c = server::getChannelFromList(_channels, channel);
	if (!c) {
		error::channel::NOSUCHCHANNEL(user, _name, channel);
		return;
	}
	
	if (line == c->getName()) { return; }

	if (!user->getIsAdmin() && !c->isChannelAdmin(*user)) {
		error::common::CHANOPRIVSNEEDED(user, _name, channel);
		return;
	}

	if (!user::isAlreadyConnected(*c, *user)) {
		error::channel::NOTONCHANNEL(user, _name, c->getName());
		return;
	}

	std::cout << "this is the mode requested: " << modes<< std::endl;

	if (modes[0] != '+' && modes[0] != '-') {
		error::channel::WRONGMODE(user, _name, '+');
		return;
	}

	bool isPositive;

	for (size_t i = 0; i < modes.size(); ++i) {
		std::string param;
		if (modes[i] == '+' || modes[i] == '-') {
			isPositive = (modes[i] == '+');
			continue;
		}

		if (modes[i] == 'k' || modes[i] == 'l' || modes[i] == 'o') {
			iss >> param;
			//if (isPositive) iss >> param;
			
			if (isPositive && param.empty()) {
				error::common::NEEDMOREPARAMS(user, _name, "MODE +/-" + modes);
				continue;
			}
			if (modes[i] == 'k')
			{
				//isPositive ? c->setPassword(param) : c->setPassword("");
				if(isPositive)
				{
					c->setPassword(param);
					c->addMode('k');
				}
				else
				{
					c->setPassword("");
					c->removeMode('k');
				}
				Server::sendMode(user, c, isPositive, 'k');
			} 
			else if (modes[i] == 'l')
			{
				if (!isPositive)
				{
					c->setSize(DEFAULT_SIZE);
					c->removeMode('l');
					Server::sendMode(user, c, isPositive, 'l');
				} 
				else
				{
					if (!isAllDigits(param))
					{
						error::channel::WRONGMODE(user, _name, 'l');
						//return;
						continue;
					}
					int num = std::atoi(param.c_str());
					if (num > 2 && num <= MAX_SIZE)
					{
						c->setSize(num);
						c->addMode('l');
						Server::sendMode(user, c, isPositive, 'l');
					}
					else
					{
						error::channel::WRONGMODE(user, _name, 'l');
						//return;
					}
				}

			}
			else if (modes[i] == 'o')
			{
				User* target = server::getUserFromList(_users, param);
			
				if (!target)
				{
					error::common::NOSUCHNICK(user, _name, param);
					continue;
				}
				if (!user::isAlreadyConnected(*c, *target))
				{
					error::channel::USERNOTINCHANNEL(user, _name, param, c->getName());
					continue;
				}
				//set operator status
				std::map<User*, bool> &members = const_cast<std::map<User*, bool>&>(c->getMembers());
				std::map<User*, bool>::iterator it = members.find(target);
				if(it != members.end())
				{
					it->second = isPositive;
				}
				target->setAdmin(isPositive);
				if(isPositive)
					Server::sendPermisions(target);
				std::string msg = ":" + user->getNickname() + "!" + user->getUsername() + "@" + user->getHostname() + " MODE #" + c->getName() + " " + (isPositive ? "+" : "-") + "o " + target->getNickname() + "\r\n";
				Server::broadcastChannel(*c, msg);



				// if (!target || !user::isAlreadyConnected(*c, *target)) {
				// 	error::channel::USERNOTINCHANNEL(user, _name, param, c->getName());
				// 	continue;
				// }
				// target->setAdmin(isPositive);
				// if (isPositive) {
				// 	Server::sendPermisions(target);
				// }
			}
		} 
		else if(modes[i] == 't' || modes[i] == 'i')
		{
			if(isPositive)
			{
				c->addMode(modes[i]);
			}
			else
			{
				c->removeMode(modes[i]);
			}
			Server::sendMode(user, c, isPositive, modes[i]);
		}
		else
		{
			error::channel::WRONGMODE(user, _name, modes[i]);
		}
		// else if (modes[i] != 't' && modes[i] != 'i') 
		// {
		// 	error::channel::WRONGMODE(user, _name, modes[i]);
		// 	return;
		// }
		// isPositive ? c->addMode(modes[i]) : c->removeMode(modes[i]);
		// std::cout << "The channel: " <<  c->getName() << "has these modes: " << c->getModes() << std::endl;
		// Server::sendMode(user, c, isPositive, modes[i]);
	}
}

void	Server::cmdInvite(const User* user, const std::string& line) {
	std::istringstream iss(line);
	std::string target, channel;
	iss >> target >> channel;

	if (target.empty() || channel.empty()) {
		error::common::NEEDMOREPARAMS(user, _name, "Invite");
		return;
	}

	if (channel[0] == '#') {
		channel.erase(0, 1);
	}

	Channel* c = server::getChannelFromList(_channels, channel);
	if (!c) {
		error::channel::NOSUCHCHANNEL(user, _name, channel);
		return;
	}

	if (!user::isAlreadyConnected(*c, *user)) {
		error::channel::NOTONCHANNEL(user, _name, channel);
		return;
	}

	if (c->hasMode('i') && !user->getIsAdmin() && !c->isChannelAdmin(*user)) {
		error::common::CHANOPRIVSNEEDED(user, _name, channel);
		return;
	}

	User* t = server::getUserFromList(_users, target);
	if (!t) {
		error::common::NOSUCHNICK(user, _name, target);
		return;
	}

	Server::sendInvToTarget(user, c, t);
	Server::sendInvConfirm(user, c, _name, t);
	c->addInvitedUser(t);
}

void Server::channelTopic(const User* u, const std::string& line) {

	std::istringstream iss(line);
	std::string channel, topic;

	iss >> channel;
	std::getline(iss, topic);

	Channel* c = server::getChannelFromList(_channels, channel);
	if (!c) {
		error::channel::NOSUCHCHANNEL(u, _name, channel);
		return;
	}
	if (!user::isAlreadyConnected(*c, *u)) {
		error::channel::NOTONCHANNEL(u, _name, channel);
		return;
	}
	if (c->hasMode('t') && !u->getIsAdmin() && !c->isChannelAdmin(*u)) {
		error::common::CHANOPRIVSNEEDED(u, _name, channel);
		return;
	}
	if (!topic.empty() && topic[0] == ' ') {
		topic.erase(0 , 1);
	}
	if (!topic.empty() && topic[0] == ':') {
		topic.erase(0 , 1);
	}
	if (!topic.empty()) {
		c->setTopic(topic);
		const std::string msg = ":" + u->getNickname() + "!" + u->getUsername() + "@"
			+ u->getHostname() + " TOPIC #" + channel + " :" + topic + "\n";
		Server::broadcastChannel(*c, msg);
	}
}

void Server::channelKick(const User* u, const std::string& line) {

	std::istringstream iss(line);
	std::string channel, target, msg;
	iss >> channel >> target;
	std::getline(iss, msg);

	if (channel.empty() || target.empty()) {
		error::common::NEEDMOREPARAMS(u, channel, "KICK");
		return;
	}

	if (!channel.empty() && channel[0] == '#') {
		channel.erase(0,1);
	}

	Channel *c = server::getChannelFromList(_channels,  channel);
	if (!c) {
		error::channel::NOSUCHCHANNEL(u, _name, channel);
		return;
	}

	if (!u->getIsAdmin() && !c->isChannelAdmin(*u)) {
		error::common::CHANOPRIVSNEEDED(u, _name, channel);
		return;
	}

	User *t = server::getUserFromList(_users, target);
	if (!t || !user::isAlreadyConnected(*c, *t)) {
		error::channel::USERNOTINCHANNEL(u, _name, target, channel);
		return;
	}

	if (t == u) {
		error::common::CANTKICKYOURSELF(u, _name, channel);
		return;
	}

	Server::sendKick(u, c, target, msg);
	c->removeMember(*t, _name, false);
	c->setKickedUser(t);
}

void Server::cmdOper(User *user, std::string line) {

	std::istringstream iss(line);
    std::string cmd, name, _password;
    iss >> cmd >> name >> _password;

    if (name.empty() || _password.empty()) {
		error::common::NEEDMOREPARAMS(user, _name, cmd);
        return;
    }

	if (name != IRC_OPER_NAME || _password != IRC_OPER_PASS) {
		error::common::NOCREDENTIALS(user, _name);
		return;
	}
	
	user->setAdmin(true);
	Server::sendPermisions(user);
}


void Server::cmdWhois(User *user, std::string line) {
	std::istringstream iss(line);
    std::string cmd, targetNick;
    iss >> cmd >> targetNick;

    if (targetNick.empty()) {
        std::string err = ":" + _name + " 431 " + user->getNickname() + " :No nickname given\r\n";
        send(user->getPoll().fd, err.c_str(), err.size(), 0);
        return;
    }

    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        User *target = it->second;
        if (target->getNickname() == targetNick) {
            std::string msg;

            msg = ":" + _name + " 311 " + user->getNickname() + " " + targetNick + " "
                + target->getUsername() + " " + target->getHostname() + " * :"
                + target->getRealname() + "\r\n";
            send(user->getPoll().fd, msg.c_str(), msg.size(), 0);

            msg = ":" + _name + " 312 " + user->getNickname() + " " + targetNick + " " + _name + " :The BIGGEST MALAKA Server\r\n";
            send(user->getPoll().fd, msg.c_str(), msg.size(), 0);

            msg = ":" + _name + " 318 " + user->getNickname() + " " + targetNick + " :End of /WHOIS list\r\n";
            send(user->getPoll().fd, msg.c_str(), msg.size(), 0);

            return;
        }
    }

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

	std::cout << msg.c_str() << std::endl;
	send(user->getPoll().fd, msg.c_str(), msg.size(), 0);
}
