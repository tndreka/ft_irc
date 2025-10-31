#include "../include/Utils.hpp"
#include "../include/Channel.hpp"
#include "../include/User.hpp"
#include "../include/Server.hpp"

/****************************************************
*						SERVER						*
****************************************************/

/**
 * @brief Splits a line using ',' as the delimeter.
 * 
 * @param list_str Line to be split.
 * @param mode_flag Sets the behavior based on the mode.
 * 
 * @return A vector container containing all the extracted tokens
 * from the input
 */
static std::vector<std::string> split_comma_list(const std::string& list_str, bool mode_flag) {
    std::stringstream			ss(list_str);
    std::vector<std::string>	result;
    std::string					token;

	if (mode_flag) {
		while (std::getline(ss, token, ',')) {
			if (token == "x")
				result.push_back("");
			else
				result.push_back(token);
			}
	}
	else {
	    while (std::getline(ss, token, ','))
			result.push_back(token);
	}
	return (result);
}

/**
 * @brief Extracts all the channel names as parameters of the /part command.
 * 
 * @param line The entire user input.
 * 
 * @note line = "PART <channel1_name>,<channel2_name>".
 * @note line = "PART #<active_channel> :<channel1_name>,<channel2_name>".
 * 
 * @return A vector with all the channel names.
 * 
 */
static std::vector<std::string>	parsePart(const std::string& line) {
	std::vector<std::string> result;
	std::istringstream iss(line);
	std::string command;
	std::string channel_list_str;

	if (!(iss >> command) || command != "PART")
		return (result);
	if (!(iss >> channel_list_str))
		return (result);
	if (channel_list_str[0] == '#') {
		iss >> channel_list_str;
		channel_list_str.erase(0, 1);
	}
	result = split_comma_list(channel_list_str, false);
	return (result);
}

/**
 * @brief Extract the recepient and the message included in the private msg.
 * 
 * @param line Line to be parsed.
 * 
 * @note line = 'PRIVMSG <user_nickname> :<msg>'
 * @note line = 'PRIVMSG #<channel_name> :<msg>'
 * 
 * @return pair = <recepient_name>, <message>.
 */
static std::pair<std::string, std::string>  parsePrivMsg(const std::string& line) {
	const char 							*control_chars = "\r\n";
	std::pair<std::string, std::string> result;
	std::istringstream                  iss(line);
	std::string                         command;
	std::string                         target;

	if (!(iss >> command) || (command != "PRIVMSG" && command != "NOTICE"))
		return result;
	if (!(iss >> target))
		return result;
	result.first = target;
	if (!result.first.empty() && result.first[0] == '#')
		result.first.erase(0, 1);
	size_t colon_pos = line.find(':');
	if (colon_pos != std::string::npos)
		result.second = line.substr(colon_pos + 1);
	else
		result.second = ""; 
	size_t end_of_msg = result.second.find_last_not_of(control_chars);
	if (end_of_msg != std::string::npos)
		result.second = result.second.substr(0, end_of_msg + 1);
	else if (result.second.empty() == false)
		result.second.clear();
	return (result);
}

/**
 * @brief Extracts all the channel names as parameters of the /join command.
 * 
 * @param line The entire user input.
 * 
 * @note line = "JOIN #<channel1>,#<channel2>,#<channel3> [<pass1>,,<pass3>]\r\n".
 * 
 * @return A vector with all the channel names. 
 */
static std::map<std::string, std::string> parseJoin(const std::string& line) {
	std::map<std::string, std::string> result;
	std::istringstream iss(line);
	std::string command;
	std::string key_list_str;

	// TODO authenticate channel
	if (!(iss >> command) || command != "JOIN")
		return (result);
	if (!(iss >> command))
		return (result);
	iss >> key_list_str;
	std::vector<std::string> channels = split_comma_list(command, true);
	std::vector<std::string> keys;
	if (!key_list_str.empty())
		keys = split_comma_list(key_list_str, true);
	for (size_t i = 0; i < channels.size(); ++i) {
		std::string chan_name = channels[i];
		std::string key = "";
		if (!chan_name.empty() && chan_name[0] == '#')
			chan_name = chan_name.substr(1);
		if (i < keys.size())
			key = keys[i];
		result[chan_name] = key;
	}
	return (result);
}

/**
 * @brief Handles the way a user joins a channel.
 * 
 * @param server The server's instance.
 * @param user The joining user.
 * @param user_input The user input to be parsed.
 * 
 * @note channel_map[<channel_name]>=<channel_pass>.
 * 
 */
void	server::handleJoin(Server& server, User* user, std::string user_input) {
    std::map<std::string, std::string>	channel_map = parseJoin(user_input);

	for (std::map<std::string, std::string>::const_iterator it = channel_map.begin(); it != channel_map.end(); ++it) {
		Channel* channel = server::getChannelFromList(server.getChannels(), it->first);

		if (!channel::isAlreadyExisting(server.getChannels(), (*it).first)) {
			Channel* new_channel = new Channel(it->first, it->second);
			new_channel->addMember(*user, true);
			server.addChannel(new_channel);
			channel::welcomeUser(server.getName(), *new_channel, *user);
			continue;
		}
		if (user::isAlreadyConnected(*channel, *user)) {
			std::cout << "'" << user->getNickname() << "' already connected to " << channel->getName() << std::endl;
			error::channel::USERONCHANNEL(user, server.getName(), user->getNickname(), channel->getName());
			continue;
		}
		if (channel->getMembers().size() == channel->getSize()) {
			std::cout << "Channel '" << channel->getName() << "' already full!" << std::endl;
			error::channel::CHANNELISFULL(*user, *channel);
			continue;
		}
		if (!channel->getPassword().empty() && channel->getPassword() != it->second) {
			error::common::NOCREDENTIALS(user, server.getName());
			continue;
		}
		channel->addMember(*user, false);
		channel::welcomeUser(server.getName(), *channel, *user);
		std::cout << "'" << user->getNickname() << "' just connected to " << channel->getName() << std::endl;
	}
	server::printChannels(server.getChannels());
}

/**
 * @brief Handles the way a user leaves a channel.
 * 
 * @param server The server's instance.
 * @param user The leaving user.
 * @param user_input The user input to be parsed.
 */
void	server::handlePart(Server& server, User* user, std::string user_input) {
	std::vector<std::string> channels_to_leave = parsePart(user_input);
	
	std::cout << "'" << user->getNickname() << "' leaving the channel(s): ";
	for (std::vector<std::string>::const_iterator it = channels_to_leave.begin(); it != channels_to_leave.end(); ++it)
		std::cout << "'" << *it << "' ";
	std::cout << std::endl;


	//TODO handle auth
	for (std::vector<std::string>::const_iterator it = channels_to_leave.begin(); it != channels_to_leave.end(); ++it) {
		Channel* channel = server::getChannelFromList(server.getChannels(), *it);
		if (!channel::isAlreadyExisting(server.getChannels(), *it)) {
			std::cout << "The channel doesn't exist!" << std::endl;
			error::channel::NOSUCHCHANNEL(user, server.getName(), channel->getName());
			continue;
		}
		if (!user::isAlreadyConnected(*channel, *user)) {
			std::cout << "'" << user->getNickname() << "' is not connected to any channels named '" << *it << "'!" << std::endl; 
			error::channel::NOTONCHANNEL(user, server.getName(), channel->getName());
			continue;
		}

		channel::goodbyeUser(*channel, *user);
		channel->removeMember(*user, server.getName());
		if (channel->getMembers().empty())
			server.deleteChannel(user, *it);
	}
	server::printChannels(server.getChannels());
}

/**
 * @brief Sends either a channel or a private message.
 * 
 * @param server The server's instance.
 * @param sender The user that sends the message.
 * @param user_input The user input to be parsed.  
 * 
 * @note pair = <recepient>,<message>.
 */
void	server::handlePrivMsg(Server& server, User& sender, const std::string& user_input) {
	const std::pair<std::string, std::string>	pair = parsePrivMsg(user_input);
	bool notice = !user_input.find("NOTICE") ? true : false;

	if (user_input.find('#') != std::string::npos) {
		Channel* channel = server::getChannelFromList(server.getChannels(), pair.first);
		if (channel)
			channel::sendMsg(*channel, sender, pair.second, notice);
		else
			error::channel::NOSUCHCHANNEL(&sender, server.getName(), channel->getName());
	}
	else {
		User* recepient = server::getUserFromList(server.getUsers(), pair.first);
		if (recepient)
			user::sendMsg(sender, *recepient, pair.second, notice);
	}
}

/**
 * @brief Notifies the remaining clients when another user disconnects.
 * 
 * @param server The server's instance.
 * @param user The quiting user.
 * @param user_input The user input to be parsed.
 * 
 */
void	server::handleQuit(Server& server, User& user) {
	// const std::string		prefix = ":" + user.getNickname() + "!" + user.getUsername() + "@" + user.getHostname();
	// const std::string		postfix = ":Client exiting!\r\n";
	// const std::string		msg = prefix + " QUIT " + postfix;
	std::vector<Channel*>	channels = server.getChannels();

	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
		if (user::isAlreadyConnected(**it, user)) {
			channel::goodbyeUser(**it, user);
			(*it)->removeMember(user, server.getName());
		}
	}
	// send(user.getPoll().fd, msg.c_str(), msg.size(), 0);
	server.removeUser(user.getPoll().fd);
	// send message to exit server
}

/**
 * @brief Prints all the channels of the server.
 * 
 * @param channels The list of channels to be printed.
 * 
 * @note Output: <channel_name>, <channel_password>, {<channel_members>}.
 */
void    server::printChannels(const std::vector<Channel*>& channels) {
    if (channels.empty())
		return ((void)(std::cout << "No active channels!" << std::endl));
	std::cout << "Channel list: ";
	std::cout << "\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
    for (std::vector<Channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
		std::cout << **it << std::endl;
		channel::printMembers(**it);
		channel::printOperators(**it);
		if (it + 1 != channels.end())
			std::cout << std::endl;
    }
    std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n" << std::endl;
}

/**
 * @note QUIT :leaving (disconnect)
 * @note Client 5(127.0.0.1) disconnected (/disconnect)
 */
void	server::printUsers(const std::map<int, User*>& users) {
	if (users.empty())
		return ((void)(std::cout << "No active users!" << std::endl));
	std::cout << "\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
	std::cout << "User list: ";
    for (std::map<int, User*>::const_iterator it = users.begin(); it != users.end(); ++it) {
		if (it != users.begin())
			std::cout << ", ";
		std::cout << "{" << it->second->getPoll().fd << ", '" << it->second->getNickname() << "'}";
    }
    std::cout << "\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n" << std::endl;
}

/**
 * @brief Searches in a list for a channel and returns it.
 * 
 * @param channels The list of channel to search in.
 * @param name The channel's name to search for in the list.
 * 
 * @return The channel's address if found, NULL otherwise.
 */
Channel*	server::getChannelFromList(const std::vector<Channel*>& channels, std::string name) {
	typedef std::vector<Channel*>::const_iterator ChannelIterator;

	for (ChannelIterator it = channels.begin(); it != channels.end(); ++it) {
		if (*it != NULL && (*it)->getName() == name)
			return (*it);
	}
	return (NULL);
}

/**
 * @brief Searches for a user in a list and returns it.
 * 
 * @param users The user map to search in.
 * @param name The user's name to search for in the list.
 * 
 * @return The user's address if found, NULL otherwise.
 */
User*	server::getUserFromList(const std::map<int, User*>& users, const std::string& name) {
	for (std::map<int, User*>::const_iterator it = users.begin(); it != users.end(); ++it) {
		if (it->second != NULL && it->second->getNickname() == name)
			return (it->second);
	}
	return (NULL);
}

/****************************************************
*						CHANNEL						*
****************************************************/

/**
 * @brief Searches for a specific channel in the list.
 * 
 * @param channels The list of channels.
 * @param name The channel to search for in the list.
 * 
 * @return True if found, false otherwise.
 */
bool	channel::isAlreadyExisting(const std::vector<Channel*>& channels, const std::string name) {
    std::vector<Channel*>::const_iterator it;

	for (it = channels.begin(); it != channels.end(); ++it)
		if ((*it)->getName() == name)
			return (true);
	return (false);
}

/**
 * @brief Print a channel's members' nickname.
 * 
 * @param channel The channel's for its users to be printed.
 */
void	channel::printMembers(const Channel& channel) {
	const std::map<User*, bool> map = channel.getMembers();
	
	if (map.empty())
		return ((void)(std::cout << "No members in channel '" << channel.getName() << "'!" << std::endl));
	std::cout << "'" << channel.getName() << "' members: ";
	for (std::map<User*, bool>::const_iterator it = map.begin(); it != map.end(); ++it) {
		if (it != map.begin())
			std::cout << ", ";
		std::cout << "'" << it->first->getNickname() << "' (" << it->first->getPoll().fd << ")";
	}
	std::cout << std::endl;
}

/**
 * @brief Print a channel's members' nickname.
 * 
 * @param channel The channel's for its users to be printed.
 */
void	channel::printOperators(const Channel& channel) {
	const std::map<User*, bool> members = channel.getMembers();
	
	if (members.empty())
		return ((void)(std::cout << "No operators in the channel!" << std::endl));
	std::cout << "'" << channel.getName() << "' operators: ";
	for (std::map<User*, bool>::const_iterator it = members.begin(); it != members.end(); ++it) {
		if (!it->second)
			continue;
		if (it != members.begin())
			std::cout << ", ";
		std::cout << "'" << it->first->getNickname();
	}
	std::cout << std::endl;
}

/**
 * @brief Sends a Welcome message to the user that entered a new channel for the first time.
 * 
 * @param server_name The server's name, necessary to send the message back to the client.
 * @param channel The newly created channel.
 * @param user The user entered the new channel.
 */
void	channel::welcomeUser(const std::string& server_name, const Channel& channel, const User& user) {
	const std::string			prefix = ":" + user.getNickname() + "!" + user.getUsername() + "@" + user.getHostname();
	const std::string			postfix = "\r\n";
	const std::map<User*, bool>	members = channel.getMembers();
	
	for (std::map<User*, bool>::const_iterator it = members.begin(); it != members.end(); ++it) {
		std::string msg = prefix + " JOIN :#" + channel.getName() + postfix;
		send(it->first->getPoll().fd, msg.c_str(), msg.size(), 0);
	}
	channel::sendTopic(server_name, channel, user);
	channel::sendNameList(server_name, channel, user);
}

/**
 * @brief Sends a leaving message to all members of the channel, except for the new user.
 * 
 * @param channel The leaving channel.
 * @param user The user to leave the channel.
 */
void	channel::goodbyeUser(const Channel& channel, const User& user) {
	const std::map<User*, bool>	members = channel.getMembers();
	const std::string			prefix = ":" + user.getNickname() + "!" + user.getUsername() + "@" + user.getHostname();
	const std::string			info = " PART #" + channel.getName() + " :Goodbye everybody!";
	const std::string			postfix = "\r\n";
	const std::string 			msg = prefix + info + postfix;
	
	for (std::map<User*, bool>::const_iterator it = members.begin(); it != members.end(); ++it)
		send(it->first->getPoll().fd, msg.c_str(), msg.size(), 0);
}

/**
 * @brief Send a message to all members in the channel.
 * 
 * @param channel The channel to receive the messages.
 * @param sender The user that sends the message.
 * @param msg The message to be sent.werty You're not channel operator
 * 
 */
void	channel::sendMsg(Channel& channel, User& sender, const std::string& msg, bool notice) {
	const std::string			type = notice ? " NOTICE #" : " PRIVMSG #";
	const std::string			prefix = ":" + sender.getNickname() + "!" + sender.getUsername() + "@" + sender.getHostname() + type + channel.getName() + " :";
	const std::string			postfix = "\r\n";
	const std::string			full_msg = prefix + msg + postfix;
	const std::map<User*, bool>	members = channel.getMembers();

	for (std::map<User*, bool>::const_iterator it = members.begin(); it != members.end(); ++it) {
		if (it->first->getPoll().fd != sender.getPoll().fd)
			send(it->first->getPoll().fd, full_msg.c_str(), full_msg.size(), 0);
	}
}

void	channel::sendTopic(const std::string& server_name,  const Channel& channel, const User& user) {
	const std::string	prefix = ":" + server_name;
	const std::string	info = user.getNickname() + " #" + channel.getName() + " :";
	const std::string	postfix = "\r\n";
	
	if (!channel.getTopic().empty()) {
		std::string msg = prefix + PROTOCOL::RPL_TOPIC + info + channel.getTopic() + postfix;
		send(user.getPoll().fd, msg.c_str(), msg.size(), 0);
	}
}

void	channel::sendNameList(const std::string& server_name, const Channel& channel, const User& user) {
	const std::map<User*, bool> members = channel.getMembers();
	const std::string	prefix = ":" + server_name;
	const std::string	info = user.getNickname() + " = #" + channel.getName() + " :";
	const std::string	postfix = "\r\n";
	
	std::string	msg = prefix + PROTOCOL::RPL_NAMREPLY + info;
	for (std::map<User*, bool>::const_iterator it = members.begin(); it != members.end(); ++it)
		msg += it->first->getNickname() + " ";
	msg += postfix;
	send(user.getPoll().fd, msg.c_str(), msg.size(), 0);
	msg = prefix + PROTOCOL::RPL_ENDOFNAMES + user.getNickname() + " " + channel.getName() + " :End of /NAMES list" + postfix;
	send(user.getPoll().fd, msg.c_str(), msg.size(), 0);
}

/****************************************************
*						USER						*
****************************************************/

// void	channel::destroy(Channel* channel) {};

/**
 * @brief Checks if the user is already connected to a specific channel.
 * 
 * @param channel The channel to searched in.
 * @param user The user in question as a member of the channel.
 * 
 * @return True/False.
 */
bool	user::isAlreadyConnected(Channel& channel, const User& user) {
	std::map<User*, bool>	members = channel.getMembers();

	for (std::map<User*, bool>::const_iterator m_it = members.begin(); m_it != members.end(); ++m_it) {
		if (user.getPoll().fd == m_it->first->getPoll().fd)
			return (true);
	}
	return (false);
}

void	user::sendMsg(User& sender, User& recepient, const std::string& msg, bool notice) {
	const std::string	type = notice ? " NOTICE " : " PRIVMSG ";
	const std::string	prefix = ":" + sender.getNickname() + "!" + sender.getUsername() + "@" + sender.getHostname() + type + recepient.getNickname() + " :";
	const std::string	postfix = "\r\n";
	const std::string	full_msg = prefix + msg + postfix;

	send(recepient.getPoll().fd, full_msg.c_str(), full_msg.size(), 0);
}
