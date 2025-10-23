#ifndef SERVERUTILS_HPP
#define SERVERUTILS_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <sys/socket.h>

class User;
class Channel;
class Server;

namespace server {
	void		handleJoin(Server& server, User* user, std::string user_input);
	void		handlePart(Server& server, User* user, std::string user_input);
	void		handlePrivMsg(Server& server, const std::string user_input);
	void		printChannels(std::vector<Channel*>& channels);
	Channel*	getChannelFromList(const std::vector<Channel*>& channels, std::string name);
	bool		isAlreadyExisting(const std::vector<Channel*>& channels, const std::string name);
	// handleNick()
};

namespace channel {
	Channel*	create(User* user, std::pair<std::string, std::string> pair);
	void		printMembers(Channel& channel);
	void		welcomeUser(std::string server_name, Channel& channel, User& user);
	void		goodbyeUser(Channel& channel, User& user);
};

namespace user {
	bool	isAlreadyConnected(Channel& channel, User& user);
	void	leaveChannel(Channel* channel, User& user);
};

#endif