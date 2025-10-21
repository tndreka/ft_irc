#include "../include/Channel.hpp"
#include "../include/User.hpp"

Channel::Channel() :
	_name("default"),
	_members()
{};

Channel::Channel(std::string name) :
	_name(name),
	_members()
{
	std::cout << "New channel created named " << _name << std::endl;
};

Channel::Channel(const Channel& other) :
	_name(other._name),
	_members(other._members)
{};

Channel::~Channel() {};

Channel&	Channel::operator=(const Channel& other) {
	if (this != &other) {
		_name = other._name;
		_members = other._members;
	}
	return (*this);
};

const std::string	Channel::getName(void) const {
	return (_name);
};

std::map<int, User*>	Channel::getMembers(void) const {
	return (_members);
};

void	Channel::addMember(User& member) {
	_members[member.getPoll().fd] = &member;
};

void	Channel::removeMember(User& member) {
	_members.erase(member.getPoll().fd);
};

std::ostream&	operator<<(std::ostream& out, const Channel& obj) {
	out << "Channel name: '" << obj.getName() << std::endl;
	return (out);
};