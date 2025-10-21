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
	void								handlePart(std::vector<Channel*>& channels, User* user, std::string line);
	void								printChannels(std::vector<Channel*>& channels);
	Channel*							getChannelFromList(std::vector<Channel*>& channels, std::string name);
	void								deleteChannel(std::vector<Channel*>& channels);
	std::map<std::string, std::string>	parseJoin(const std::string& line);
	std::map<std::string, std::string>	parsePart(std::string line);
	// handleNick()
};

namespace channel {
	bool		isAlreadyExisting(std::vector<Channel*>& channels, const std::string name);
	Channel*	create(User* user, std::pair<std::string, std::string> pair);
	void		join(Channel channel, User* user);
	void		printMembers(Channel& channel);
	void		welcomeMessage(std::string server_name, Channel& channel, User& user);
};

namespace user {
	Channel*	getConnectedChannel(std::vector<Channel>& channels, User* user);
	void		leaveChannel(Channel* channel, User& user);
	void		joinChannel(Channel* channel, User* user);
};

#endif