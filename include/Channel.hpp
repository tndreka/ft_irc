#ifndef CHANNEL_HPP
#define	CHANNEL_HPP

#include <iostream>
#include <map>
#include <set>
#include <string>

class User;

class Channel {
	private:
		std::string				_name;
		std::map<int, User*>	_members;
		unsigned int			_maxMembers;	// TODO: check before joining
		std::string				_password;
		std::string				_topic;
		bool					_topicAdminOnly;
		bool					_isInviteOnly;
		std::set<char>			_modes;


	public:
		Channel();
		Channel(std::string name, std::string pass);
		Channel(const Channel& othher);
		~Channel();

		Channel&	operator=(const Channel& other);

		const std::string			getName(void) const;
		const std::map<int, User*>&	getMembers(void) const;
		std::string					getPassword(void) const;
		bool						getTopicAdminOnly(void) const;
		bool						getIsInvitedOnly(void) const;
	
		void						setTopic(const std::string& topic);
		void						setPassword(const std::string& pass);
		void						setMaxMembers(unsigned int num);
		void						setTopicAdminOnly(bool b);
		void						setIsInvitedOnly(bool b);

		void						addMember(User* user);
		void						removeMember(User& user);

		bool						hasMode(char m);
		void						addMode(char m);
		void						removeMode(char m);
		std::string					getModes() const;
};

std::ostream&	operator<<(std::ostream& out, const Channel& obj);

#endif
