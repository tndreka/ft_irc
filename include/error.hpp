#pragma once

#include "../include/Server.hpp"

namespace Error {

	//===============================REGISTRATION ERRORS====================================================
	inline void NICKNAMEINUSE(const User& user, std::string _name, std::string& atemptedNick) {
		
		std::cout << atemptedNick << std::endl;
		std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
		std::string msg = ":" + _name + " 433 " + nick + " " + atemptedNick
			+ " :Nickname is already in use\r\n";

		std::cout << msg << std::endl;;
    	send(user.getPoll().fd, msg.c_str(), msg.length(), 0);
	}

	inline void ERRONEUSNICKNAME(std::string _name, int fd, std::string nickname) {
		std::string msg = ":" + _name + " 432 * " + nickname + " :Erroneous nickname\r\n";
    	send(fd, msg.c_str(), msg.length(), 0);
	}

	inline void USERALREADYEXISTS(const User* user) {
		std::string msg = ":MalakaIRC 462 " + (user->getNickname().empty() ? "*" : user->getNickname())
			+ " :Unauthorized command (already registered)\r\n";
		send(user->getPoll().fd, msg.c_str(), msg.size(), 0);
	}



	//===============================CHANNEL ERRORS==========================================================
	inline void NOSUCHCHANNEL(const User *user, const std::string &server, const std::string &channel) {
		std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
		std::string msg = ":" + server + " 403 " + nick + " " + channel + " :No such channel\r\n";
		send(user->getPoll().fd, msg.c_str(), msg.length(), 0);
	}

	inline void NOTONCHANNEL(const User *user, const std::string &server, const std::string &channel) {
		std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
		std::string msg = ":" + server + " 442 " + nick + " " + channel + " :You're not on that channel\r\n";
		send(user->getPoll().fd, msg.c_str(), msg.length(), 0);
	}

	inline void USERNOTINCHANNEL(const User *user, const std::string &server, const std::string &target, const std::string &channel) {
		std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
		std::string msg = ":" + server + " 441 " + nick + " " + target + " " + channel +
						" :They aren't on that channel\r\n";
		send(user->getPoll().fd, msg.c_str(), msg.length(), 0);
	}

	inline void USERONCHANNEL(const User *user, const std::string &server, const std::string &target, const std::string &channel) {
		std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
		std::string msg = ":" + server + " 443 " + nick + " " + target + " " + channel +
						" :is already on channel\r\n";
		send(user->getPoll().fd, msg.c_str(), msg.length(), 0);
	}

	inline void CHANNELISFULL(const User *user, const std::string &server, const std::string &channel) {
		std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
		std::string msg = ":" + server + " 471 " + nick + " " + channel + " :Cannot join channel (+l)\r\n";
		send(user->getPoll().fd, msg.c_str(), msg.length(), 0);
	}

	inline void INVITEONLYCHAN(const User *user, const std::string &server, const std::string &channel) {
		std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
		std::string msg = ":" + server + " 473 " + nick + " " + channel + " :Cannot join channel (+i)\r\n";
		send(user->getPoll().fd, msg.c_str(), msg.length(), 0);
	}

	inline void BANNEDFROMCHAN(const User *user, const std::string &server, const std::string &channel) {
		std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
		std::string msg = ":" + server + " 474 " + nick + " " + channel + " :Cannot join channel (+b)\r\n";
		send(user->getPoll().fd, msg.c_str(), msg.length(), 0);
	}

	inline void BADCHANNELKEY(const User *user, const std::string &server, const std::string &channel) {
		std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
		std::string msg = ":" + server + " 475 " + nick + " " + channel + " :Cannot join channel (+k)\r\n";
		send(user->getPoll().fd, msg.c_str(), msg.length(), 0);
	}

	inline void WRONGMODE(const User* user, const std::string &server, char flag) {
		std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
		std::string f;
		f.append(0, flag);
		std::string msg = ":" + server + " 472 " + nick + " :Unknown MODE flag: " + f +"\r\n";
		send(user->getPoll().fd, msg.c_str(), msg.length(), 0);
	}

	inline void	CHANNELISFULL(const User& user, const Channel& channel) {
		const std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
		const std::string msg = nick + " " + channel.getName() + " :Cannot join channel (+l)" + "\r\n";
		send(user.getPoll().fd, msg.c_str(), msg.size(), 0);
	}


	//================================COMMON ERRORS=======================================

	inline void NOSUCHNICK(const User *user, const std::string &server, const std::string &target) {
		std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
		std::string msg = ":" + server + " 401 " + nick + " " + target + " :No such nick/channel\r\n";
		send(user->getPoll().fd, msg.c_str(), msg.length(), 0);
	}

	inline void CANNOTSENDTOCHAN(const User *user, const std::string &server, const std::string &channel) {
		std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
		std::string msg = ":" + server + " 404 " + nick + " " + channel + " :Cannot send to channel\r\n";
		send(user->getPoll().fd, msg.c_str(), msg.length(), 0);
	}

	inline void NEEDMOREPARAMS(const User *user, const std::string &server, const std::string &command) {
		std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
		std::string msg = ":" + server + " 461 " + nick + " " + command + " :Not enough parameters\r\n";
		send(user->getPoll().fd, msg.c_str(), msg.length(), 0);
	}

	inline void CHANOPRIVSNEEDED(const User *user, const std::string &server, const std::string &channel) {
		std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
		std::string msg = ":" + server + " 482 " + nick + " " + channel + " :You're not channel operator\r\n";
		send(user->getPoll().fd, msg.c_str(), msg.length(), 0);
	}

	inline void NOCREDENTIALS(const User *user, const std::string &server) {
		std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
		std::string err = ":" + server + " 464 " + nick + " :Incorrect credentials\r\n";
        send(user->getPoll().fd, err.c_str(), err.size(), 0);
	}
}
