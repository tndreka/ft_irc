#include "../include/Utils.hpp"
#include "../include/Channel.hpp"
#include "../include/User.hpp"

/****************************************************
*						SERVER						*
****************************************************/

static std::vector<std::string> split_comma_list(const std::string& list_str) {
    std::stringstream			ss(list_str);
    std::vector<std::string>	result;
    std::string					token;

    while (std::getline(ss, token, ',')) {
		if (token == "x")
			result.push_back("");
		else
        	result.push_back(token);
	}
    return (result);
}

/**
 * @brief Extracts all the channel names from the user input
 * 
 * @return A vector with all the channel names. 
 */
std::map<std::string, std::string> server::parseJoin(const std::string& line) {
	std::map<std::string, std::string> channel_keys;
	std::istringstream iss(line);
	std::string command;
	std::string channel_list_str;
	std::string key_list_str;

	if (!(iss >> command) || command != "JOIN")
		return (channel_keys);
	if (!(iss >> channel_list_str))
		return (channel_keys);
	iss >> key_list_str; 
	std::vector<std::string> channels = split_comma_list(channel_list_str);
	std::vector<std::string> keys;
	if (!key_list_str.empty())
		keys = split_comma_list(key_list_str);
	for (size_t i = 0; i < channels.size(); ++i) {
		std::string chan_name = channels[i];
		std::string key = "";
		if (!chan_name.empty() && chan_name[0] == '#')
			chan_name = chan_name.substr(1);
		if (i < keys.size())
			key = keys[i];
		channel_keys[chan_name] = key;
	}
	return (channel_keys);
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
 * @note line = "JOIN #<channel1>,#<channel2> [<key1>,<key2>]\r\n".
 * @note channel_map[<channel_name]>=<channel_pass>. 
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

std::vector<std::string>	server::parsePart(const std::string& line) {
	std::istringstream iss(line);
	std::string command;
	std::string list_str_token;

	if (!(iss >> command) || command != "PART")
        return std::vector<std::string>();
	if (!(iss >> list_str_token))
		return std::vector<std::string>();
	if (iss.peek() != std::istringstream::traits_type::eof()) {
		std::string remainder;
		std::getline(iss >> std::ws, remainder);
		size_t colon_pos = remainder.find(':');
		if (colon_pos != std::string::npos)
			list_str_token = remainder.substr(colon_pos + 1);
	}
	std::vector<std::string> raw_channels = split_comma_list(list_str_token);
	std::vector<std::string> result;
	for (std::vector<std::string>::const_iterator it = raw_channels.begin(); it != raw_channels.end(); ++it) {
		std::string cleaned_chan = *it;
		if (!cleaned_chan.empty() && cleaned_chan[0] == '#')
			cleaned_chan = cleaned_chan.substr(1);
		if (!cleaned_chan.empty())
			result.push_back(cleaned_chan);
	}
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
 * 
 * @note buff = "PART <channel1_name>,<channel2_name>".
 * @note buff = "PART #<active_channel> :<channel1_name>,<channel2_name>".
 */
void	server::handlePart(std::string server_name, std::vector<Channel*>& channels, User* user, std::string user_input) {
	std::vector<std::string> channels_to_delete = server::parsePart(user_input);
	
	(void) server_name;
	std::cout << "Channels to delete: ";
	for (std::vector<std::string>::const_iterator it = channels_to_delete.begin(); it != channels_to_delete.end(); ++it) {
		std::cout << "'" << *it << "' ";
	}
	std::cout << std::endl;
	for (std::vector<std::string>::const_iterator it = channels_to_delete.begin(); it != channels_to_delete.end(); ++it) {
		Channel* channel = server::getChannelFromList(channels, *it);
		if (channel::isAlreadyExisting(channels, *it)) {
			if (user::isAlreadyConnected(*channel, *user)) {
				channel::goodbyeUser(*channel, *user);
				channel->removeMember(*user);
				if (channel->getMembers().empty())
					server::deleteChannel(channels);
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
 * @note Prints <channel_name>, <channel_password>, <channel_members>.
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

Channel*	server::getChannelFromList(std::vector<Channel*>& channels, std::string name) {
	typedef std::vector<Channel*>::const_iterator ChannelIterator;

	for (ChannelIterator it = channels.begin(); it != channels.end(); ++it) {
		if (*it != NULL && (*it)->getName() == name)
			return (*it);
	}
	return (NULL);
}

void	server::deleteChannel(std::vector<Channel*>& channels) {
	(void) channels;
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
		std::string msg = ":" + user->getUsername() + " JOIN :#" + channel->getName() + "\r\n";
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
	std::cout << "Members:";
	for (std::map<int, User*>::const_iterator it = map.begin(); it != map.end(); ++it) {
		if (it != map.begin())
			std::cout << ", ";
		std::cout << "'" << (*it).second->getUsername() << "'";
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
				msg += "[" + it->second->getUsername() + "] ";
			}
			msg += postfix;
			send(it->first, msg.c_str(), msg.size(), 0);
		}
		else {
			msg = "[" + user.getUsername() + "@" + user.getHostname() + "] joined the channel!";
			send(it->second->getPoll().fd, (prefix + msg + postfix).c_str(), (prefix + msg + postfix).size(), 0);
		}
	}
}

// /**
//  * @brief Sends a joining message to all members of the channel, except for the new user.
//  * 
//  * @param server_name The server's name, necessary to send the message back to the client.
//  * @param channel The joined channel.
//  * @param user The user just joined, excluded from receiving the joining message.
//  */
// void	channel::joinMessage(std::string server_name, Channel& channel, User& user) {
// 	const std::string		prefix = ":" + server_name + " PRIVMSG #" + channel.getName() + " :";
// 	const std::string		msg = "[" + user.getUsername() + "@" + user.getHostname() + "] joined the channel!";
// 	const std::string		postfix = "\r\n";
// 	std::map<int, User*>	members = channel.getMembers();
	
// 	for (std::map<int, User*>::iterator it = members.begin(); it != members.end(); ++it) {
// 		if (it->second->getPoll().fd == user.getPoll().fd)
// 			continue;
// 		send(it->second->getPoll().fd, (prefix + msg + postfix).c_str(), (prefix + msg + postfix).size(), 0);
// 	}
// }

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
			msg = prefix + "[" + user.getNickname() + "] left the chat!" + postfix;
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