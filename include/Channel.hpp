#ifndef CHANNEL_HPP
#define	CHANNEL_HPP

#include <iostream>
#include <map>
#include <sys/socket.h>
#include <set>
#include <string>
#include <vector>

#define DEFAULT_SIZE 20
#define MAX_SIZE 200

class User;

class Channel {
	private:
		std::string				_name;
		std::map<User*, bool>	_members;
		std::vector<User*>		_kickedUsers;
		std::vector<User*>		_invitedUsers;
		unsigned int			_size;
		std::string				_password;
		std::string				_topic;
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
		unsigned int					getNumOfUsers(void) const;

		void							setTopic(const std::string& topic);
		void							setPassword(const std::string& pass);
		void							setSize(unsigned int num);
		void							setTopicAdminOnly(bool b);
		void							setIsInvitedOnly(bool b);

		void							addMember(User& user, bool value);
		void							removeMember(User& user, const std::string& server_name, bool sendMsg);
		void							setKickedUser(User* target);
		bool							isInKickedlist (const User& user);
		void							unsetKickedUser(User* target);
		bool							hasMode(char m);
		void							addMode(char m);
		void							removeMode(char m);
		bool							isChannelAdmin(const User& user);
		bool							isUserInvited(const User& user);
		void							addInvitedUser(User* user);
};

std::ostream&	operator<<(std::ostream& out, const Channel& obj);

#endif
