#include "../include/ServerUtils.hpp"
#include "../include/Channel.hpp"
#include "../include/User.hpp"

/**
 * @brief Handles all the different cases for the /join command
 */
void	server::handleJoin(std::vector<Channel*>& channels, User* user, std::string line) {
    std::string	channel_name;
	bool		isExistingChannel;
	// Channel*			connected_channel = user::getConnectedChannel(channels, user);

	if (!line.empty() && line[line.length() - 1] == '\r')
		line.erase(line.length() - 1);
	std::cout << "Line: '" + line + "'" << std::endl;
    channel_name = line.substr(6);
	isExistingChannel = channel::isAlreadyExisting(channels, channel_name);
	if (isExistingChannel) {
		std::cout << "Requested channel: " << channel_name << std::endl;
		// printChannels(channels);
		// if new channel == connected channel -> nothing
		// else join()
	}
	else {
		Channel* new_channel = channel::create(user, channel_name);
		channels.push_back(new_channel);
	}
	// server::printChannels(channels);
}

void    server::printChannels(std::vector<Channel*>& channels) {
    typedef std::vector<Channel*>::iterator ChannelIterator;

    if (channels.empty())
		return ((void)(std::cout << "No active channels!" << std::endl));
	std::cout << "Channel list: ";
	std::cout << "\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
    for (ChannelIterator it = channels.begin(); it != channels.end(); ++it) {
        if (it != channels.begin())
            std::cout << ", ";
        std::cout << "'" << (*it)->getName() << "'";
    }
    std::cout << "\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n" << std::endl;
}

bool	channel::isAlreadyExisting(std::vector<Channel*>& channels, const std::string name) {
    std::vector<Channel*>::const_iterator it;

	for (it = channels.begin(); it != channels.end(); ++it)
		if ((*it)->getName() == name)
			return (true);
	return (false);
}

Channel*	channel::create(User* admin, const std::string channel_name) {
	Channel*	channel = new Channel(admin, channel_name);
	return (channel);
}

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
		if (it->getAdmin() == *user)
			return &(*it);
		for (mem_it m_it = it->getMembers().begin(); m_it != it->getMembers().end(); ++m_it)
			if (m_it->second == user)
				return &(*it);
	}
	return (NULL);
}