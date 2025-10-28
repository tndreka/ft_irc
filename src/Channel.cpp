#include "../include/Channel.hpp"
#include "../include/User.hpp"
#include "../include/error.hpp"

Channel::Channel() :
	_name("default"),
	_members(),
	_operators(),
	_size(DEFAULT_SIZE),
	_password(""),
	_topic(""),
	_topicAdminOnly(false)
{};

Channel::Channel(std::string name, std::string pass) :
	_name(name),
	_members(),
	_operators(),
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
	_operators(other._operators),
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
		_operators = other._operators;
		_size = other._size;
		_password = other._password;
		_topic = other._topic;
		_topicAdminOnly = other._topicAdminOnly;
	}
	return (*this);
};

const std::string	Channel::getName(void) const {
	return (_name);
};

const std::map<int, User*>&	Channel::getMembers(void) const {
	return (_members);
};

const std::map<int, User*>&	Channel::getOperators(void) const {
	return (_operators);
}

std::string	Channel::getPassword(void) const {
	return (_password);
}

unsigned int  Channel::getSize(void) const {
	return (_size);
}

void	Channel::addMember(User* member) {
	_members[member->getPoll().fd] = member;
};

void	Channel::addOperator(User* oper) {
	_operators[oper->getPoll().fd] = oper;
}

void	Channel::removeMember(User& member, const std::string& server_name) {
	const std::string	prefix = ":" + server_name + " NOTICE " + member.getNickname();
	const std::string	postfix = "\r\n";
	const std::string	msg = prefix + ": You left channel " + getName() + postfix;
	
	_members.erase(member.getPoll().fd);
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

std::ostream&	operator<<(std::ostream& out, const Channel& obj) {
	out << "Channel name: '" << obj.getName() << "', Password: '" << obj.getPassword() << "'";
	return (out);
};
