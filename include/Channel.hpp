#ifndef CHANNEL_HPP
#define	CHANNEL_HPP

#include <iostream>
#include <map>

class User;

class Channel {
	private:
		std::string				_name;
		User*					_admin;
		std::map<int, User*>	_members;

	public:
		Channel();
		Channel(std::string name, User admin);
		Channel(const Channel& othher);
		~Channel();

		Channel&	operator=(const Channel& other);

		std::string				getName(void) const;
		User					getAdmin(void) const;
		std::map<int, User*>	getMembers(void) const;
		void					addMember(User& user);
		void					removeMember(User& user);
};

std::ostream&	operator<<(std::ostream& out, const Channel& obj);

#endif