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
	std::pair<std::string, std::string> result;
	std::istringstream                  iss(line);
	std::string                         command;
	std::string                         target;

	if (!(iss >> command) || command != "PRIVMSG")
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
	const char *control_chars = "\r\n";
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
 * @note line = "JOIN #<channel1>,#<channel2> [<key1>,<key2>]\r\n".
 * 
 * @return A vector with all the channel names. 
 */
static std::map<std::string, std::string> parseJoin(const std::string& line) {
	std::map<std::string, std::string> result;
	std::istringstream iss(line);
	std::string command;
	std::string key_list_str;

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
 * @param user The active user.
 * @param user_input The user input to be parsed.
 * 
 * @note channel_map[<channel_name]>=<channel_pass>.
 * 
 */
void	server::handleJoin(Server& server, User* user, std::string user_input) {
    std::map<std::string, std::string>	channel_map = parseJoin(user_input);

	for (std::map<std::string, std::string>::const_iterator it = channel_map.begin(); it != channel_map.end(); ++it) {
		Channel* channel = server::getChannelFromList(server.getChannels(), it->first);
		if (channel::isAlreadyExisting(server.getChannels(), (*it).first)) {
			if (!user::isAlreadyConnected(*channel, *user)) {
				channel->addMember(user);
				channel::welcomeUser(server.getName(), *channel, *user);
			}
			// else send a message "already connected!" ????
		} 
		else {
			Channel* new_channel = channel::create(user, *it);
			server.addChannel(new_channel);
			channel::welcomeUser(server.getName(), *new_channel, *user);
		}
	}
}

/**
 * @brief Handles the way a user leaves a channel.
 * 
 * @param server The server's instance
 * @param user_input The user input to be parsed.
 */
void	server::handlePart(Server& server, User* user, std::string user_input) {
	std::vector<std::string> channels_to_delete = parsePart(user_input);
	
	std::cout << user->getNickname() << " deleted the channel(s): ";
	for (std::vector<std::string>::const_iterator it = channels_to_delete.begin(); it != channels_to_delete.end(); ++it)
		std::cout << "'" << *it << "' ";
	std::cout << std::endl;
	for (std::vector<std::string>::const_iterator it = channels_to_delete.begin(); it != channels_to_delete.end(); ++it) {
		// std::cout << "Channel name: '" << *it << "'" << std::endl;
		Channel* channel = server::getChannelFromList(server.getChannels(), *it);
		if (channel::isAlreadyExisting(server.getChannels(), *it)) {
			if (user::isAlreadyConnected(*channel, *user)) {
				channel::goodbyeUser(*channel, *user);
				channel->removeMember(*user);
				if (channel->getMembers().empty())
					server.deleteChannel(user, *it);
			}
			// else send a message "not connected to that channel" ????
		}
		else
			std::cout << "The channel doesn't exist!" << std::endl;
	}
}

/**
 * @brief Sends either a channel or a private message.
 * 
 * @param server The server's instance.
 * @param usert_input The user input to be parsed.  
 * 
 * @note pair = <recepient>,<message>.
 */
void	server::handlePrivMsg(Server& server, const std::string& user_input) {
	const std::pair<std::string, std::string>	pair = parsePrivMsg(user_input);
	Channel* channel = server::getChannelFromList(server.getChannels(), pair.first);

	std::cout << "Recep: '" << pair.first << "', msg: '" << pair.second << "'" << std::endl;
	//	<-------- correct parsing up to here
	if (channel) {
		channel::sendMsg(*channel, pair.second, server.getName());
	}
	else {
		User* user = server::getUserFromList(server.getUsers(), pair.first);
		if (user)
			user::sendMsg(*user, pair.second, server.getName());
		//else error
	}
}

/**
 * @brief Prints all the channels of the server.
 * 
 * @param channels The list of channels to be printed.
 * 
 * @note Output: <channel_name>, <channel_password>, {<channel_members>}.
 */
void    server::printChannels(std::vector<Channel*>& channels) {
    if (channels.empty())
		return ((void)(std::cout << "No active channels!" << std::endl));
	std::cout << "Channel list: ";
	std::cout << "\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
    for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
		std::cout << **it << std::endl;
		channel::printMembers(**it);
		if (it + 1 != channels.end())
			std::cout << std::endl;
    }
    std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n" << std::endl;
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
 * @param The user map to search in.
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
 * @brief Create a new channel and adds the user to its members.
 * 
 * @note pair = <channel_name>,<password>.
 */
Channel*	channel::create(User* user, std::pair<std::string, std::string> pair) {
	Channel*	channel = new Channel(pair.first, pair.second);
	channel->addMember(user);
	for (std::map<int, User*>::const_iterator it = channel->getMembers().begin(); it != channel->getMembers().end(); ++it) {
		std::string msg = ":" + user->getNickname() + " JOIN :#" + channel->getName() + "\r\n";
		send(it->first, msg.c_str(), msg.size(), 0);
	}
	return (channel);
}

/**
 * @brief Print a channel's members' username.
 */
void	channel::printMembers(Channel& channel) {
	std::map<int, User*> map = channel.getMembers();
	
	if (map.empty())
		return ((void)(std::cout << "No members in this channel!" << std::endl));
	std::cout << "Members: ";
	for (std::map<int, User*>::const_iterator it = map.begin(); it != map.end(); ++it) {
		if (it != map.begin())
			std::cout << ", ";
		std::cout << "'" << (*it).second->getNickname() << "'";
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
void	channel::welcomeUser(std::string server_name, Channel& channel, User& user) {
	const std::string		prefix = ":" + server_name + " PRIVMSG #" + channel.getName() + " :";
	const std::string		postfix = "\r\n";
	std::string				msg;
	std::map<int, User*>	members = channel.getMembers();

	for (std::map<int, User*>::iterator it = members.begin(); it != members.end(); ++it) {
		if (it->second->getPoll().fd == user.getPoll().fd) {
			msg = prefix + "[Users #" + channel.getName() + "]" + postfix;
			send(it->first, msg.c_str(), msg.size(), 0);
			msg = prefix;
			for (std::map<int, User*>::iterator it = members.begin(); it != members.end();++it) {
				msg += "[" + it->second->getNickname() + "] ";
			}
			msg += postfix;
			send(it->first, msg.c_str(), msg.size(), 0);
		}
		else {
			msg = "[" + user.getNickname() + "@" + user.getHostname() + "] joined the channel!";
			send(it->second->getPoll().fd, (prefix + msg + postfix).c_str(), (prefix + msg + postfix).size(), 0);
		}
	}
}

/**
 * @brief Sends a leaving message to all members of the channel, except for the new user.
 * 
 * @param server_name The server's name, necessary to send the message back to the client.
 * @param channel The joined channel.
 * @param user The user just joined, excluded from receiving the joining message.
 */
void	channel::goodbyeUser(Channel& channel, User& user) {
	std::string				prefix = ":" + user.getNickname() + " PART #" + channel.getName() + " :";
	const std::string		postfix = "\r\n";
	std::string				msg;
	std::map<int, User*>	members = channel.getMembers();

	for (std::map<int, User*>::iterator it = members.begin(); it != members.end(); ++it) {
		if (it->second->getPoll().fd != user.getPoll().fd) {
			msg = prefix + postfix;
			send(it->first, msg.c_str(), msg.size(), 0);
		}
		else {
			msg = ":" + user.getNickname() + " PART #" + channel.getName() + postfix;
			send(it->second->getPoll().fd, msg.c_str(), msg.size(), 0);
		}
	}
}

/**
 * @brief Send a message to all members in the channel.
 * 
 * @param channel The channel to receive the messages.
 * @param msg The message to be sent.
 * @param server_name The server's name, necessary to send the message.
 * 
 */
void	channel::sendMsg(Channel& channel, const std::string& msg, const std::string& server_name) {
	const std::string			prefix = ":" + server_name + " PRIVMSG #" + channel.getName() + " :";
	const std::string			postfix = "\r\n";
	const std::string			full_msg = prefix + msg + postfix;
	const std::map<int, User*>	members = channel.getMembers();

	for (std::map<int, User*>::const_iterator it = members.begin(); it != members.end(); ++it) {
		send(it->second->getPoll().fd, full_msg.c_str(), full_msg.size(), 0);
	}
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
bool	user::isAlreadyConnected(Channel& channel, User& user) {
	for (std::map<int, User*>::const_iterator m_it = channel.getMembers().begin(); m_it != channel.getMembers().end(); ++m_it) {
		if (user.getPoll().fd == m_it->second->getPoll().fd)
			return (true);
	}
	return (false);
}

void	user::sendMsg(User& user, const std::string& msg, const std::string& server_name) {
	const std::string	prefix = ":" + server_name + " PRIVMSG " + user.getNickname() + " :";
	const std::string	postfix = "\r\n";
	const std::string	full_msg = prefix + msg + postfix;

	send(user.getPoll().fd, full_msg.c_str(), full_msg.size(), 0);
}