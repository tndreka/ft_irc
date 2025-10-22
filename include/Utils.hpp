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
	void								handleJoin(std::string server_name, std::vector<Channel*>& channels, User* user, std::string line);
	void								handlePart(std::string server_name, std::vector<Channel*>& channels, User* user, std::string line);
	void								printChannels(std::vector<Channel*>& channels);
	void								deleteChannel(std::vector<Channel*>& channels);
	Channel*							getChannelFromList(std::vector<Channel*>& channels, std::string name);
	std::map<std::string, std::string>	parseJoin(const std::string& line);
	std::vector<std::string>			parsePart(const std::string& line);
	// handleNick()
};

namespace channel {
	bool		isAlreadyExisting(std::vector<Channel*>& channels, const std::string name);
	Channel*	create(User* user, std::pair<std::string, std::string> pair);
	void		printMembers(Channel& channel);
	void		welcomeUser(std::string server_name, Channel& channel, User& user);
	// void		joinMessage(std::string server_name, Channel& channel, User& user);
	void		goodbyeUser(Channel& channel, User& user);
};

namespace user {
	bool	isAlreadyConnected(Channel& channel, User& user);
	void	leaveChannel(Channel* channel, User& user);
};

#endif