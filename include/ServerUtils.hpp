#ifndef SERVERUTILS_HPP
#define SERVERUTILS_HPP

#include <iostream>
#include <vector>
#include <string>

class User;
class Channel;
class Server;

namespace channels {
	void	handleJoin(std::vector<Channel>& channels, User* user, std::string line);
	bool	isAlreadyExisting(std::vector<Channel>& channels, const std::string name);
	void	create(const std::string admin_name, const std::string channel_name);
	void	join(Channel channel, User* user);
};

namespace users {
	Channel*	isAlreadyConnected(std::vector<Channel>& channels, User* user);
};

#endif