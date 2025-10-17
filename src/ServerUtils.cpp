#include "../include/ServerUtils.hpp"
#include "../include/Channel.hpp"
#include "../include/User.hpp"

/**
 * @brief Handles all the different cases for the /join command
 */
void	channels::handleJoin(std::vector<Channel>& channels, User* user, std::string line) {
    const std::string	channel_name = line.substr(6);
	const std::string	connected_channel = users::isAlreadyConnected(channels, user);
	bool				isExistingChannel = channels::isAlreadyExisting(channels, channel_name);

	if (isExistingChannel)
		return;
		// join() an existing one
	else
		channels::create(user->getUsername(), channel_name);
}

bool	channels::isAlreadyExisting(std::vector<Channel>& channels, const std::string name) {
    std::vector<Channel>::const_iterator it;

	for (it = channels.begin(); it != channels.end(); ++it)
		if (it->getName() == name)
			return (true);
	return (false);
}

void	channels::create(const std::string admin_name, const std::string channel_name) {

}

/**
 * @brief Checks if the user is already connected to any of the server's channels.
 * 
 * @note It is suppposed to look through the entire list (TODO).
 * 
 * @return The channel's name, if connected, null otherwise.
 */
const Channel*	users::isAlreadyConnected(std::vector<Channel>& channels, User* user) {
	typedef std::vector<Channel>::const_iterator	ch_it;
	typedef std::map<int, User*>::const_iterator	mem_it;

	for (ch_it it = channels.begin(); it != channels.end(); ++it) {
		if (it->getAdmin() == *user)
			return (*it);
		for (mem_it m_it = it->getMembers().begin(); m_it != it->getMembers().end(); ++m_it)
			if (m_it->second == user)
				return (it);
	}
	return (NULL);
}