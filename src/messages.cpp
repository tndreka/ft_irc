#include "../include/Server.hpp"

void Server::broadcastChannel(const Channel& channel, const std::string& msg) {
	std::map<User*, bool> usersInChannel = channel.getMembers();

	for (std::map<User*, bool>::iterator it = usersInChannel.begin(); it != usersInChannel.end(); ++it) {
		send(it->first->getPoll().fd, msg.c_str(), msg.length(), 0);
	}
}

void Server::broadcast_message(const std::string &message, const User& user) {
	for (size_t i = 0; i < _pollFds.size(); i++) {
		if (_pollFds[i].fd != listening && _pollFds[i].fd != user.getPoll().fd)
			send(_pollFds[i].fd, message.c_str(), message.length(), 0);
	}
}

void Server::sendWelcome(User& user) {
	std::string nick = user.getNickname();
	std::vector<std::string> replies;

	replies.push_back(":" + _name + " 001 " + nick + " :Welcome to the " +
		_name + " IRC Server!\r\n");
	replies.push_back(":" + _name + " 002 " + nick + " :Your host is " +
		_name + "\r\n");
	replies.push_back(":" + _name + " 376 " + nick + " :End of MOTD\r\n");

	for (size_t i = 0; i < replies.size(); ++i) {
		send(user.getPoll().fd, replies[i].c_str(), replies[i].size(), 0);
	}
}

void Server::sendWrongPassword(User& user) {
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
	std::string err = ":" + _name + " 464 " + nick + " :Password incorrect\r\n";
	send(user.getPoll().fd, err.c_str(), err.size(), 0);

	std::string closing = "ERROR :Closing link: " + nick + " (Password required)\r\n";
	send(user.getPoll().fd, closing.c_str(), closing.size(), 0);
}

void Server::sendCapabilities(User& user) {
	std::string capReply = ":" + _name + " CAP * LS :\r\n";
	send(user.getPoll().fd, capReply.c_str(), capReply.size(), 0);
}

void Server::sendPong(User* user, std::string ping) {
	std::string pong = "PONG " + ping.substr(5) + "\r\n";
	send(user->getPoll().fd, pong.c_str(), pong.size(), 0);
}

void Server::sendKick(const User* u, const Channel* c, const std::string target, std::string msg) {

	if (!msg.empty() && msg[msg.size() - 1] == '\r') {
		msg.erase(msg.size() - 1);
	}

	if (msg.empty()) {
		msg = ":Bye bye Malaka";
	}
	
	std::string message = ":" + u->getNickname() + "!" + u->getUsername() + "@"
			+ u->getHostname() + " KICK #" + c->getName() + " " + target + " " + msg + "\r\n";
	Server::broadcastChannel(*c, message);
}

void Server::sendPermisions(const User* user) {
	std::string msg = ":" + _name + " 381 " + user->getNickname() + " :You are now an IRC operator\r\n";
	send(user->getPoll().fd, msg.c_str(), msg.size(), 0);

	std::string notice = ":" + _name + " NOTICE * :Operator " + user->getNickname() + " has authenticated\r\n";
	broadcast_message(notice, *user);
}

void Server::sendMode(const User* user, const Channel* c, bool isPossitive, const char m) {
	std::string tag = (isPossitive ? "+" : "-");
	tag.push_back(m);
	std::string msg = ":" + user->getNickname() + "!" + user->getUsername() + "@"
		+ user->getHostname() + " MODE #" + c->getName() + " " + tag + "\r\n";
	Server::broadcastChannel(*c, msg);
}

void Server::sendInvToTarget(const User* user, const Channel* channel, const User* target) {
	std::string msg = ":"  + user->getNickname() + "!" + user->getUsername() + "@" + user->getHostname()
		+ " INVITE " + target->getNickname() + " :#" + channel->getName() + "\r\n";
	send(target->getPoll().fd, msg.c_str(), msg.size(), 0);
}

void Server::sendInvConfirm(const User* user, const Channel* channel, const std::string& server, const User* target) {
	std::string msg = ":" + server + " 341 " + user->getNickname() + " " + target->getNickname()
		+ " #" + channel->getName() + "\r\n";
	send(user->getPoll().fd, msg.c_str(), msg.size(), 0);
}

void	Server::sendStripChannelOper(const User* user, const Channel* channel, const User* target) {
	std::string msg = ":" + user->getNickname() + "!" + user->getUsername() + "@"
		+ user->getHostname() + "MODE #" +  channel->getName() + "-o " + target->getNickname()
		+ "\r\n";
	broadcastChannel(*channel, msg);
}
