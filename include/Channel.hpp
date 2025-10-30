#ifndef CHANNEL_HPP
#define	CHANNEL_HPP

#define DEFAULT_SIZE 20

#include <iostream>
#include <map>
#include <sys/socket.h>
#include <set>
#include <string>

#define MAX_SIZE 200
#define DEFAULT_SIZE 20

class User;

class Channel {
	private:
		std::string				_name;
		std::map<User*, bool>	_members;
		unsigned int			_size;
		std::string				_password;
		std::string				_topic;
		bool					_topicAdminOnly;
		bool					_isInviteOnly;
		std::set<char>			_modes;

	public:
		Channel();
		Channel(std::string name, std::string pass);
		Channel(const Channel& other);
		~Channel();

		Channel&	operator=(const Channel& other);

		const std::string&				getName(void) const;
		const std::string&				getTopic() const;
		const std::string&				getPassword(void) const;
		std::string						getModes() const;
		const std::map<User*, bool>&	getMembers(void) const;
		bool							getTopicAdminOnly(void) const;
		bool							getIsInvitedOnly(void) const;
		unsigned int					getSize(void) const;

		void							setTopic(const std::string& topic);
		void							setPassword(const std::string& pass);
		void							setSize(unsigned int num);
		void							setTopicAdminOnly(bool b);
		void							setIsInvitedOnly(bool b);

		void							addMember(User* user, bool value);
		void							removeMember(User& user, const std::string& server_name);
		bool							hasMode(char m);
		void							addMode(char m);
		void							removeMode(char m);
};

std::ostream&	operator<<(std::ostream& out, const Channel& obj);

#endif
