#include "../include/User.hpp"

User::User() :
	_username("default"),
	_nickname("default"),
	_realname("default"),
	_hostname("default"),
	_isAdmin(false)
{};

User::User(const std::string un, const std::string nn, const std::string rn, const std::string hn) :
	_username(un),
	_nickname(nn),
	_realname(rn),
	_hostname(hn),
	_isAdmin(false)
{};

User::User(int fd, const std::string hostname) {
	_poll.fd = fd;
	_poll.events = POLLIN;
	_poll.revents = 0;
	_hostname = hostname;
	_state = WAITING_AUTH;
};

User::User(const User& other) :
	_username(other._username),
	_nickname(other._nickname),
	_realname(other._realname),
	_hostname(other._hostname),
	_isAdmin(other._isAdmin)
{};

User::~User() {};

User	User::operator=(const User& other) {
	if (this != &other) {
		_username = other._username;
		_nickname = other._nickname;
		_realname = other._realname;
		_hostname = other._hostname;
		_isAdmin = other._isAdmin;
	}
	return (*this);
};

const std::string	User::getUsername(void) const {
	return (_username);
};

const std::string	User::getNickname(void) const {
	return (_nickname);
};

const std::string	User::getRealname(void) const {
	return (_realname);
};

const std::string	User::getHostname(void) const {
	return (_hostname);
};

ClientState	User::getState(void) const {
	return (_state);
};

bool	User::getIsAdmin(void) const {
	return (_isAdmin);
};

pollfd	User::getPoll(void) const {
	return (_poll);
}

std::ostream&	operator<<(std::ostream& out, const User& obj) {
	out << "Username: " << obj.getUsername() << "\nNickname: " << obj.getNickname() << "\nRealname: " << obj.getRealname() << std::endl;
	return (out);
};