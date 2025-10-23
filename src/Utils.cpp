#include "../include/Utils.hpp"
#include "../include/Channel.hpp"
#include "../include/User.hpp"

#include <algorithm>

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
 * @brief Extracts all the channel names as parameters of the /join command.
 * 
 * @param line The entire user input.
 * 
 * @note line = "JOIN #<channel1>,#<channel2> [<key1>,<key2>]\r\n".
 * 
 * @return A vector with all the channel names. 
 */
std::map<std::string, std::string> server::parseJoin(const std::string& line) {
	std::map<std::string, std::string> result;
	std::istringstream iss(line);
	std::string command;
	std::string channel_list_str;
	std::string key_list_str;

	if (!(iss >> command) || command != "JOIN")
		return (result);
	if (!(iss >> channel_list_str))
		return (result);
	iss >> key_list_str; 
	std::vector<std::string> channels = split_comma_list(channel_list_str, true);
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
 * @param server_name The server's name, necessary for sending confirmation messages
 * back to the client.
 * @param channels The list of channels in the server.
 * @param user The active user.
 * @param user_input The user input to be parsed.
 * 
 * @note channel_map[<channel_name]>=<channel_pass>.
 * 
 */
void	server::handleJoin(std::string server_name, std::vector<Channel*>& channels, User* user, std::string user_input) {
    std::map<std::string, std::string>	channel_map = server::parseJoin(user_input);

	for (std::map<std::string, std::string>::const_iterator it = channel_map.begin(); it != channel_map.end(); ++it) {
		Channel* channel = server::getChannelFromList(channels, it->first);
		if (channel::isAlreadyExisting(channels, (*it).first)) {
			if (!user::isAlreadyConnected(*channel, *user)) {
				channel->addMember(user);
				channel::welcomeUser(server_name, *channel, *user);
				// channel::joinMessage(server_name, *channel, *user);
			}
			// else send a message "already connected!" ????
		} 
		else {
			Channel* new_channel = channel::create(user, *it);
			channels.push_back(new_channel);
			channel::welcomeUser(server_name, *new_channel, *user);
		}
	}
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
std::vector<std::string>	server::parsePart(const std::string& line) {
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
 * @brief Handles the way a user leaves a channel.
 * 
 * @param server_name The server's name, necessary for sending confirmation messages
 * back to the client.
 * @param channels The list of channels in the server.
 * @param user The active user.
 * @param user_input The user input to be parsed.
 */
void	server::handlePart(std::string server_name, std::vector<Channel*>& channels, User* user, std::string user_input) {
	std::vector<std::string> channels_to_delete = server::parsePart(user_input);
	
	std::cout << user->getNickname() << "deleted the channel(s): ";
	for (std::vector<std::string>::const_iterator it = channels_to_delete.begin(); it != channels_to_delete.end(); ++it)
		std::cout << "'" << *it << "' ";
	std::cout << std::endl;
	for (std::vector<std::string>::const_iterator it = channels_to_delete.begin(); it != channels_to_delete.end(); ++it) {
		// std::cout << "Channel name: '" << *it << "'" << std::endl;
		Channel* channel = server::getChannelFromList(channels, *it);
		if (channel::isAlreadyExisting(channels, *it)) {
			if (user::isAlreadyConnected(*channel, *user)) {
				channel::goodbyeUser(*channel, *user);
				channel->removeMember(*user);
				if (channel->getMembers().empty())
					server::deleteChannel(server_name, channels, user, *it);
			}
			// else send a message "not connected to that channel" ????
		}
		else
			std::cout << "The channel doesn't exist!" << std::endl;
	}
}

/**
 * @brief Prints all the channels of the server.
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
 * @param name The channel's name to search in the list.
 * 
 * @return The channel's address if found, NULL otherwise.
 */
Channel*	server::getChannelFromList(std::vector<Channel*>& channels, std::string name) {
	typedef std::vector<Channel*>::const_iterator ChannelIterator;

	for (ChannelIterator it = channels.begin(); it != channels.end(); ++it) {
		if (*it != NULL && (*it)->getName() == name)
			return (*it);
	}
	return (NULL);
}

void	server::deleteChannel(std::string server_name, std::vector<Channel*>& channels, User* user, const std::string& name) {
	Channel*			channel_to_delete;
	const std::string	prefix = ":" + server_name + " NOTICE " + user->getNickname() + ":";
	const std::string	postfix = "\r\n";
	std::string 		msg;

	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
		if ((*it)->getName() == name)
			channel_to_delete = *it;
	}
	std::vector<Channel*>::iterator new_end = std::remove(channels.begin(), channels.end(), channel_to_delete);
    channels.erase(new_end, channels.end());
	msg = prefix + " Channel '" + channel_to_delete->getName() + "' deleted successfully!" + postfix; 
	send(user->getPoll().fd, msg.c_str(), msg.size(), 0);
	delete channel_to_delete;

}

/****************************************************
*						CHANNEL						*
****************************************************/

bool	channel::isAlreadyExisting(std::vector<Channel*>& channels, const std::string name) {
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

void	user::leaveChannel(Channel* channel, User& user) {
	(void) channel;
	(void) user;
	// else del from members
}