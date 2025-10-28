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
		std::map<int, User*>	_members;
		std::map<int, User*>	_operators; // delete pointer in quit???
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

		const std::string			getName(void) const;
		const std::map<int, User*>&	getMembers(void) const;
		const std::map<int, User*>&	getOperators(void) const; 
		std::string					getPassword(void) const;
		bool						getTopicAdminOnly(void) const;
		bool						getIsInvitedOnly(void) const;
		std::string					getTopic() const;
		std::string					getModes() const;
		unsigned int				getSize(void) const;
	
		void						setTopic(const std::string& topic);
		void						setPassword(const std::string& pass);
		void						setSize(unsigned int num);
		void						setTopicAdminOnly(bool b);
		void						setIsInvitedOnly(bool b);

		void						addMember(User* user);
		void						addOperator(User* user);
		void						removeMember(User& user, const std::string& server_name);
		// void						removeOperator(User& user, const std::string& server_name);
		bool						hasMode(char m);
		void						addMode(char m);
		void						removeMode(char m);
};

std::ostream&	operator<<(std::ostream& out, const Channel& obj);

#endif
