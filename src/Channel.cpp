#include "../include/Channel.hpp"
#include "../include/User.hpp"
#include "../include/error.hpp"

Channel::Channel() :
	_name("default"),
	_members(),
	_size(DEFAULT_SIZE),
	_password(""),
	_topic(""),
	_topicAdminOnly(false)
{};

Channel::Channel(std::string name, std::string pass) :
	_name(name),
	_members(),
	_size(DEFAULT_SIZE),
	_password(pass),
	_topic(""),
	_topicAdminOnly(false)
{
	std::cout << "New channel created named " << _name << std::endl;
};

Channel::Channel(const Channel& other) :
	_name(other._name),
	_members(other._members),
	_size(other._size),
	_password(other._password),
	_topic(""),
	_topicAdminOnly(false)
{};

Channel::~Channel() {};

Channel&	Channel::operator=(const Channel& other) {
	if (this != &other) {
		_name = other._name;
		_members = other._members;
		_size = other._size;
		_password = other._password;
		_topic = other._topic;
		_topicAdminOnly = other._topicAdminOnly;
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

unsigned int  Channel::getSize(void) const {
	return (_size);
}

void	Channel::addMember(User& member, bool value) {
	_members[&member] = value;
};

void	Channel::removeMember(User& member, const std::string& server_name) {
	const std::string	prefix = ":" + server_name + " NOTICE " + member.getNickname();
	const std::string	postfix = "\r\n";
	const std::string	msg = prefix + ": You left channel " + getName() + postfix;
	
	_members.erase(&member);
	send(member.getPoll().fd, msg.c_str(), msg.size(), 0);
};

void	Channel::setTopic(const std::string& topic) {
	_topic = topic;
}

void	Channel::setPassword(const std::string& pass) {
	_password = pass;
}

void	Channel::setSize(unsigned int num) {
	_size = num;
}

bool	Channel::getTopicAdminOnly() const {
	return _topicAdminOnly;
}

void	Channel::setTopicAdminOnly(bool b) {
	_topicAdminOnly = b;
}

bool	Channel::getIsInvitedOnly() const {
	return _isInviteOnly;
}

void	Channel::setIsInvitedOnly(bool b) {
	_isInviteOnly = b;
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
		return (it->first->getNickname() == user.getNickname() && it->second);
	}
	return false;
}

std::ostream&	operator<<(std::ostream& out, const Channel& obj) {
	out << "Channel name: '" << obj.getName() << "', Password: '" << obj.getPassword() << "'";
	return (out);
};
