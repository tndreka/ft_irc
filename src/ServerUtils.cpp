#include "../include/ServerUtils.hpp"
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
        return channel_keys;
    if (!(iss >> channel_list_str))
        return channel_keys;
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
 * @note line = "JOIN #<channel1>,#<channel2> [<key1>,<key2>]\r\n".
 * 
 * @note channel_map[<channel_name]>=<channel_pass>. 
 * 
 */
void	server::handleJoin(std::string server_name, std::vector<Channel*>& channels, User* user, std::string line) {
	typedef std::map<std::string, std::string>::const_iterator	iter;
    std::map<std::string, std::string>	channel_map;
	// Channel*			connected_channel = user::getConnectedChannel(channels, user);

	channel_map = server::parseJoin(line);
	for (iter it = channel_map.begin(); it != channel_map.end(); ++it) {
		// std::cout << "First: '" << it->first << "'" << std::endl;
		// std::cout << "Second: '" << it->second << "'" << std::endl;
		if (channel::isAlreadyExisting(channels, (*it).first)) {
			std::cout << "Requested channel: " << (*it).first << std::endl;
			// printChannels(channels);
			// if new channel == connected channel -> nothing
			// else join()
		}
		else {
			Channel* new_channel = channel::create(user, *it);
			channels.push_back(new_channel);
			channel::welcomeMessage(server_name, *new_channel, *user);
		}
	}
}

// std::vector<std::string>	server::parsePart(std::string line) {
// 	(void) line;
// 	return (NULL);
// }

/**
 * @brief Handles the way a user leaves a channel.
 * 
 * @note buff = "PART <channel_name>".
 * @note buff = "PART #<active_channel> :<channel_name>".
 */
void	server::handlePart(std::vector<Channel*>& channels, User* user, std::string line) {
	std::string	channel_name;
	size_t		colon_pos;
	
	if (!line.empty() && line[line.length() - 1] == '\r')
		line.erase(line.length() - 1);
	std::cout << "Line: '" + line + "'" << std::endl;
	colon_pos = line.find(':');
	if (colon_pos != std::string::npos)
		channel_name = line.substr(colon_pos + 1);
	else {
		size_t start_pos = line.find(' ');
		channel_name = line.find(start_pos + 1);
	}
	if (channel::isAlreadyExisting(channels, channel_name)) {
		Channel* channel = server::getChannelFromList(channels, channel_name);
		user::leaveChannel(channel, *user);
		// if (channel->getAdmin() == NULL)
		// 	channel::destroy(channel);			// TODO
	}
	else
		std::cout << "The channel doesn't exist!" << std::endl;
}

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

void	channel::welcomeMessage(std::string server_name, Channel& channel, User& user) {
	const std::string		prefix = ":" + server_name + " PRIVMSG #" + channel.getName() + " :";
	const std::string		postfix = "\r\n";
	std::string				msg;
	std::map<int, User*>	members = channel.getMembers();

	for (std::map<int, User*>::iterator it = members.begin(); it != members.end(); ++it) {
		msg = prefix;
		msg += "[" + user.getUsername() + "@" + user.getHostname() + "] has joined #" + channel.getName();
		msg += postfix;
		send(it->first, msg.c_str(), msg.size(), 0);
		msg = prefix;
		msg += "[Users #" + channel.getName() + "]";
		msg += postfix;
		send(it->first, msg.c_str(), msg.size(), 0);
		msg = prefix;
		for (std::map<int, User*>::iterator it = members.begin(); it != members.end();++it) {
			msg += "[" + it->second->getUsername() + "] ";
		}
		msg += postfix;
		send(it->first, msg.c_str(), msg.size(), 0);
	}
}

/****************************************************
*						USER						*
****************************************************/

// void	channel::destroy(Channel* channel) {};

/**
 * @brief Checks if the user is already connected to any of the server's channels.
 * 
 * @note It is suppposed to look through the entire list (TODO).
 * 
 * @return The channel's name, if connected, null otherwise.
 */
Channel*	user::getConnectedChannel(std::vector<Channel>& channels, User* user) {
	typedef std::vector<Channel>::iterator			ch_it;
	typedef std::map<int, User*>::const_iterator	mem_it;

	for (ch_it it = channels.begin(); it != channels.end(); ++it) {
		for (mem_it m_it = it->getMembers().begin(); m_it != it->getMembers().end(); ++m_it)
			if (m_it->second == user)
				return &(*it);
	}
	return (NULL);
}

void	user::leaveChannel(Channel* channel, User& user) {
	(void) channel;
	(void) user;
	// else del from members
}