#include "../include/Channel.hpp"
#include "../include/User.hpp"
#include "../include/error.hpp"
#include <vector>

Channel::Channel() :
	_name("default"),
	_members(),
	_kickedUsers(),
	_invitedUsers(),
	_size(DEFAULT_SIZE),
	_password(""),
	_topic("")
{};

Channel::Channel(std::string name, std::string pass) :
	_name(name),
	_members(),
	_kickedUsers(),
	_invitedUsers(),
	_size(DEFAULT_SIZE),
	_password(pass),
	_topic("")
{
	// std::cout << "New channel created named " << _name << std::endl;
};

Channel::Channel(const Channel& other) :
	_name(other._name),
	_members(other._members),
	_kickedUsers(),
	_invitedUsers(),
	_size(other._size),
	_password(other._password),
	_topic("")
{};

Channel::~Channel() {};

Channel&	Channel::operator=(const Channel& other) {
	if (this != &other) {
		_name = other._name;
		_members = other._members;
		_kickedUsers = other._kickedUsers;
		_invitedUsers = other._invitedUsers;
		_size = other._size;
		_password = other._password;
		_topic = other._topic;
	}
	return (*this);
};

const std::string&	Channel::getName(void) const {
	return (_name);
};

const std::map<User*, bool>&	Channel::getMembers(void) const {
	return (_members);
};


const std::string&	Channel::getPassword(void) const {
	return (_password);
}

unsigned int	Channel::getSize(void) const {
	return (_size);
}

unsigned int	Channel::getNumOfUsers(void) const {
	return _members.size();
}

void	Channel::addMember(User& member, bool value) {
	_members[&member] = value;
};

void	Channel::removeMember(User& member, const std::string& server_name, bool sendMsg) {

	if (sendMsg) {
		const std::string	prefix = ":" + server_name + " NOTICE " + member.getNickname();
		const std::string	postfix = "\r\n";
		const std::string	msg = prefix + ": You left channel " + getName() + postfix;
		send(member.getPoll().fd, msg.c_str(), msg.size(), 0);
	}
	
	_members.erase(&member);
};

void	Channel::setKickedUser(User* target) {
	_kickedUsers.push_back(target);
}

void	Channel::unsetKickedUser(User *target) {
	for (std::vector<User *>::iterator it = _kickedUsers.begin(); it != _kickedUsers.end(); ++it) {
		if (*it == target) {
			_kickedUsers.erase(it);
			break;
		}
	}
}

bool	Channel::isInKickedlist (const User& user) {
	for (std::vector<User *>::const_iterator it = _kickedUsers.begin(); it != _kickedUsers.end(); ++it) {
		if (*it == &user) {
			return true;
		}
	}
	return false;
}

void	Channel::setTopic(const std::string& topic) {
	_topic = topic;
}

void	Channel::setPassword(const std::string& pass) {
	_password = pass;
}

void	Channel::setSize(unsigned int num) {
	_size = num;
}

const std::string&	Channel::getTopic() const {
	return _topic;
}

bool Channel::hasMode(char m) {
	return _modes.count(m);
}

void	Channel::addMode(char m) {
	_modes.insert(m);
}

void	Channel::removeMode(char m) {
	_modes.erase(m);
}

std::string	Channel::getModes() const {
	std::string res("+");
	for (std::set<char>::const_iterator it = _modes.begin(); it != _modes.end(); ++it) {
		res += *it;
	}
	return res;
}

bool	Channel::isChannelAdmin(const User& user) {

	for (std::map<User*, bool>::const_iterator it = _members.begin(); it != _members.end(); ++it) {
		if (it->first->getNickname() == user.getNickname() && it->second)
			return true;
	}
	return false;
}

void	Channel::addInvitedUser( User* user) {
	_invitedUsers.push_back(user);
}


bool	Channel::isUserInvited(const User& user) {
	for (std::vector<User*>::iterator it =  _invitedUsers.begin(); it != _invitedUsers.end(); ++it) {
        if ((*it)->getNickname() == user.getNickname())
            return true;
	}
	return false;
}


std::ostream&	operator<<(std::ostream& out, const Channel& obj) {
	out << "Channel name: '" << obj.getName() << "', Password: '" << obj.getPassword() << "'";
	return (out);
};
