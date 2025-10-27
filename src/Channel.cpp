#include "../include/Channel.hpp"
#include "../include/User.hpp"

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

void	Channel::removeMember(User& member, const std::string server_name) {
	const std::string	prefix = ":" + server_name + " NOTICE " + member.getNickname();
	const std::string	postfix = "\r\n";
	const std::string	msg = prefix + ": You left channel " + getName() + postfix;
	
	_members.erase(member.getPoll().fd);
	send(member.getPoll().fd, msg.c_str(), msg.size(), 0);
};

std::ostream&	operator<<(std::ostream& out, const Channel& obj) {
	out << "Channel name: '" << obj.getName() << "', Password: '" << obj.getPassword() << "'";
	return (out);
};
