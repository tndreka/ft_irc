#include "../include/User.hpp"

User::User() :
	_username("default"),
	_nickname("default"),
	_realname("default"),
	_hostname("default"),
	_state(WAITING_AUTH),
	_isAdmin(false),
	_isPassVerified(false)
{
	_poll.fd = -1;
	_poll.events = POLLIN;
	_poll.revents = 0;
	_userBuffer.reserve(8192);
};

User::User(int connection, const std::string hostname) :
	_username(""),
	_nickname(""),
	_realname(""),
	_hostname(hostname),
	_state(WAITING_AUTH),
	_isAdmin(false),
	_isPassVerified(false)
{
	_poll.fd = connection;
	_poll.events = POLLIN;
	_poll.revents = 0;
	_userBuffer.reserve(8192);
};

User::User(const User& other) :
	_username(other._username),
	_nickname(other._nickname),
	_realname(other._realname),
	_hostname(other._hostname),
	_state(other._state),
	_poll(other._poll),
	_isAdmin(other._isAdmin),
	_isPassVerified(other._isPassVerified)
{};

User::~User() {
	_userBuffer.clear();
};

User&	User::operator=(const User& other) {
	if (this != &other) {
		_username = other._username;
		_nickname = other._nickname;
		_realname = other._realname;
		_hostname = other._hostname;
		_isAdmin = other._isAdmin;
		_state = other._state;
		_poll = other._poll;
		_userBuffer = other._userBuffer;
		_isPassVerified = other._isPassVerified;
	}
	return (*this);
};

bool	User::operator==(const User& other) const {
	return (_poll.fd == other._poll.fd);
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

void	User::setAdmin(bool isAdmin) {
	_isAdmin = isAdmin;
}

void	User::appendToBuffer(const std::string& data) {
	if (!_userBuffer.empty() && _userBuffer[_userBuffer.size() - 1] == '\n')
		_userBuffer.erase(_userBuffer.size() - 1);
	_userBuffer += data; 
}

std::string& User::getUserBuffer(void) {
	return _userBuffer;
}

void User::clearUserBuffer(void) {
	_userBuffer.clear();
}

bool	User::getIsPassVerified() const {
	return _isPassVerified;
}

void	User::setPassVerified(bool isverified) {
	_isPassVerified = isverified;
}
std::ostream&	operator<<(std::ostream& out, const User& obj) {
	out << "Username: " << obj.getUsername() << "\nNickname: " << obj.getNickname() << "\nRealname: " << obj.getRealname()
		<< "\nHostname: " << obj.getHostname() << "\nFD: " << obj.getPoll().fd << "\nState: " << obj.getState();
	return (out);
};
