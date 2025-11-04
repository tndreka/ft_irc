#ifndef SERVERUTILS_HPP
#define SERVERUTILS_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <cstdio>

class User;
class Channel;
class Server;

namespace PROTOCOL {
	const std::string RPL_TOPIC = " 332 ";
	const std::string RPL_NAMREPLY = " 353 ";
	const std::string RPL_ENDOFNAMES = " 366 ";
};

namespace server {
	void		handleJoin(Server& server, User* user, std::string user_input);
	void		handlePart(Server& server, User* user, std::string user_input);
	void		handlePrivMsg(Server& server, User& user, const std::string& user_input);
	void		handleQuit(Server& server, User& user);
	// void		printChannels(const std::vector<Channel*>& channels);
	void		printUsers(const std::map<int, User*>& users);
	Channel*	getChannelFromList(const std::vector<Channel*>& channels, std::string name);
	User*		getUserFromList(const std::map<int, User*>& users, const std::string& name);
	// handleNick()
};

namespace channel {
	bool		isAlreadyExisting(const std::vector<Channel*>& channels, const std::string name);
	void		printMembers(const Channel& channel);
	void		printOperators(const Channel& channel);
	void		welcomeUser(const std::string& server_name, const Channel& channel, const User& user);
	void		goodbyeUser(const Channel& channel,const  User& user);
	void		sendMsg(Channel& channel, User& user, const std::string& msg, bool notice);
	void		sendTopic(const std::string& server_name, const Channel& channel, const User& user);
	void		sendNameList(const std::string& server_name, const Channel& channel, const User& user);
};

namespace user {
	bool	isAlreadyConnected(Channel& channel, const User& user);
	void	sendMsg(User& sender, User& recepient, const std::string& msg, bool notice);
};

#endif
