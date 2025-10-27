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
	void		handlePrivMsg(Server& server, User& user, const std::string& user_input);
	void		handleQuit(Server& server, User& user);
	void		printChannels(const std::vector<Channel*>& channels);
	void		printUsers(std::map<int, User*>& users);
	Channel*	getChannelFromList(const std::vector<Channel*>& channels, std::string name);
	User*		getUserFromList(const std::map<int, User*>& users, const std::string& name);
	// handleNick()
};

namespace channel {
	bool		isAlreadyExisting(const std::vector<Channel*>& channels, const std::string name);
	Channel*	create(User* user, std::pair<std::string, std::string> pair);
	void		printMembers(Channel& channel);
	void		welcomeUser(std::string server_name, Channel& channel, User& user);
	void		goodbyeUser(Channel& channel, User& user);
	void		sendMsg(Channel& channel, User& user, const std::string& msg);
};

namespace user {
	bool	isAlreadyConnected(Channel& channel, const User& user);
	void	sendMsg(User& sender, User& recepient, const std::string& msg);
};

#endif
