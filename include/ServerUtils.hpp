#ifndef SERVERUTILS_HPP
#define SERVERUTILS_HPP

#include <iostream>
#include <vector>
#include <string>

class User;
class Channel;
class Server;

namespace server {
	void	handleJoin(std::vector<Channel>& channels, User* user, std::string line);
	void	printChannels(std::vector<Channel>& channels);
	// handleNick()
};

namespace channel {
	bool		isAlreadyExisting(std::vector<Channel>& channels, const std::string name);
	Channel*	create(User* admin, const std::string channel_name);
	void		join(Channel channel, User* user);
};

namespace user {
	Channel*	getConnectedChannel(std::vector<Channel>& channels, User* user);
};

#endif