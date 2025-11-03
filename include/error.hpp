#pragma once

#include "Server.hpp"

namespace PROTOCOL {
	// const std::string 
};

namespace error {
	namespace registration {
		void	NICKNAMEINUSE(const User& user, std::string _name, std::string& atemptedNick);
		void	ERRONEUSNICKNAME(std::string _name, int fd, std::string nickname);
		void	USERALREADYEXISTS(const User* user);
	};

	namespace channel {
		void	NOSUCHCHANNEL(const User *user, const std::string &server, const std::string &channel);
		void	NOTONCHANNEL(const User *user, const std::string &server, const std::string &channel);
		void	USERNOTINCHANNEL(const User *user, const std::string &server, const std::string &target, const std::string &channel);
		void	USERONCHANNEL(const User *user, const std::string &server, const std::string &target, const std::string &channel);
		void	CHANNELISFULL(const User *user, const std::string &server, const std::string &channel);
		void	INVITEONLYCHAN(const User *user, const std::string &server, const std::string &channel);
		void	BANNEDFROMCHAN(const User *user, const std::string &server, const std::string &channel);
		void	BADCHANNELKEY(const User *user, const std::string &server, const std::string &channel);
		void	WRONGMODE(const User* user, const std::string &server, char flag);
		void	CHANNELISFULL(const User& user, const Channel& channel);
	};

	namespace common {
		void	NOSUCHNICK(const User *user, const std::string &server, const std::string &target);
		void	CANNOTSENDTOCHAN(const User *user, const std::string &server, const std::string &channel);
		void	NEEDMOREPARAMS(const User *user, const std::string &server, const std::string &command);
		void	CHANOPRIVSNEEDED(const User *user, const std::string &server, const std::string &channel);
		void	NOCREDENTIALS(const User *user, const std::string &server);
		void	CANTKICKYOURSELF(const User* u, const std::string& server, const std::string& channel);
	};
}
