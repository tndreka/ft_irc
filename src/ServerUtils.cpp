#include "../include/ServerUtils.hpp"
#include "../include/Channel.hpp"
#include "../include/User.hpp"

/**
 * @brief Handles all the different cases for the /join command
 */
void	server::handleJoin(std::vector<Channel>& channels, User* user, std::string line) {
    const std::string	channel_name = line.substr(6);
	bool				isExistingChannel = channel::isAlreadyExisting(channels, channel_name);
	// Channel*			connected_channel = user::getConnectedChannel(channels, user);

	if (isExistingChannel) {
		std::cout << "New channel: '" << channel_name << "'" << std::endl;
		std::cout << "Channel list: ";
		printChannels(channels);
		std::cout << std::endl;
		return;
		// if new channel == connected channel -> nothing
		// else join()
	}
	else {
		Channel* new_channel = channel::create(user, channel_name);
		channels.push_back(*new_channel);
		//add to the list with the others
	}
}

void	server::printChannels(std::vector<Channel>& channels) {
	size_t	size = channels.size();

	for (size_t i = 0; i < size; i++) {
		std::cout << "'" << channels[i].getName() << "'";
		if (i < size - 1)
			std::cout <<", ";
	}
	std::cout << std::endl;
}

bool	channel::isAlreadyExisting(std::vector<Channel>& channels, const std::string name) {
    std::vector<Channel>::const_iterator it;

	for (it = channels.begin(); it != channels.end(); ++it)
		if (it->getName() == name)
			return (true);
	return (false);
}

Channel*	channel::create(User* admin, const std::string channel_name) {
	Channel*	channel = new Channel(admin, channel_name);
	std::cout << "New channel created named '" << channel->getName() << "', with admin named '" << channel->getAdmin().getUsername() << "'" << std::endl;
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