#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <poll.h>

enum ClientState {
	WAITING_AUTH,
	REGISTERED,
	WAITING_NICKNAME,
	WAITING_USER,
	VERIFIED,
};

class User {
	private:
		std::string	_username;
		std::string	_nickname;
		std::string	_realname;
		std::string	_hostname;
		ClientState	_state;
		pollfd		_poll;
		bool		_isAdmin;
		
	public:
		User();
		User(const std::string un, const std::string nn, const std::string rn, const std::string hn);
		User(int fd, const std::string hostname);
		User(const User& other);
		~User();

		User	operator=(const User& other);

		const std::string	getUsername(void) const;
		const std::string	getNickname(void) const;
		const std::string	getRealname(void) const;
		const std::string	getHostname(void) const;
		bool				getIsAdmin(void) const;
		ClientState			getState(void) const;
		pollfd				getPoll(void) const;
};

std::ostream&	operator<<(std::ostream& out, const User& other);

#endif