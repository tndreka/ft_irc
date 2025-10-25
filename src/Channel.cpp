#include "../include/Channel.hpp"
#include "../include/User.hpp"
#include <set>

Channel::Channel() :
	_name("default"),
	_members(),
	_password("")
{};

Channel::Channel(std::string name, std::string pass) :
	_name(name),
	_members(),
	_password(pass)
{
	std::cout << "New channel created named " << _name << std::endl;
};

Channel::Channel(const Channel& other) :
	_name(other._name),
	_members(other._members),
	_password(other._password)
{};

Channel::~Channel() {};

Channel&	Channel::operator=(const Channel& other) {
	if (this != &other) {
		_name = other._name;
		_members = other._members;
		_password = other._password;
	}
	return (*this);
};

const std::string	Channel::getName(void) const {
	return (_name);
};

const std::map<int, User*>&	Channel::getMembers(void) const {
	return (_members);
};

std::string	Channel::getPassword(void) const {
	return (_password);
}

void	Channel::addMember(User* member) {
	_members[member->getPoll().fd] = member;
};

void	Channel::removeMember(User& member) {
	_members.erase(member.getPoll().fd);
};

void	Channel::setTopic(const std::string& topic) {
	_topic = topic;
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

std::string Channel::getModes() const {
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
