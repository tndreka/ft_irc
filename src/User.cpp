#include "../include/User.hpp"
#include <unistd.h>

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

User::User(int connection, const std::string hostname) {
	_poll.fd = connection;
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

User::~User() {
	close(_poll.fd);
};

User&	User::operator=(const User& other) {
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
};

void	User::setUsername(std::string str) {
	_username = str;
};

void	User::setNickname(std::string str) {
	_nickname = str;
};

void	User::setRealname(std::string str) {
	_realname = str;
};

void	User::setHostname(std::string str) {
	_hostname = str;
};

void	User::setState(ClientState state) {
	_state = state;
}


std::ostream&	operator<<(std::ostream& out, const User& obj) {
	out << "Username: " << obj.getUsername() << "\nNickname: " << obj.getNickname() << "\nRealname: " << obj.getRealname()
		<< "\nHostname: " << obj.getHostname() << "\nFD: " << obj.getPoll().fd << "\nState: " << obj.getState();
	return (out);
};