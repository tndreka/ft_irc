/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 15:22:31 by tndreka           #+#    #+#             */
/*   Updated: 2025/10/22 12:52:49 by tndreka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <arpa/inet.h>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <csignal>
#include <cstring>
#include <cerrno>

#ifndef MAX_BUFF
#define MAX_BUFF 4444
#define IRC_OPER_NAME "root"
#define IRC_OPER_PASS "secretpass"
#endif

extern volatile sig_atomic_t signal_flag;

#include "User.hpp"
#include "Channel.hpp"
#include "Utils.hpp"
#include "error.hpp"

class Server {
    private:
        std::string				_name;
        std::string				_password;
        std::map<int, User*>	_users;
        std::vector<Channel*>	_channels;
        std::vector<pollfd>		_pollFds;

        int						port;
        int						listening;
        sockaddr_in				hint;
        sockaddr_in				client;
        socklen_t				clientSize;

        pollfd					listening_fd;

        bool					incoming_data;
        bool					client_hungup;
        bool					is_listening;
        bool					err;

        // Server
        bool					createSocket();
        bool					bindSocket();
        bool					listenSocket();
        void					accept_connection();
        bool					init_poll();
        void					event_check(size_t index);
        void					event_state();
        void					handle_new_host();
        void					handle_messages(size_t index);
        void					handle_disconn_err_hungup(size_t index);

		// Helpers
        void					remove_from_vector(size_t index);
		bool					isNickInUse (std::string& attemptedNick);
		bool					isValidNick(std::string& attemptedNick);
		bool					isUserAlreadySigned(User& user);
		void					shutdownCleanly();

		// Parsing
		int						authenticateParser(User& user, const std::string& buff);
		std::string				authenticateNickname(User& user, std::string line);
		void					parse(User& user, const std::string& line);
 
        // Messages
        void					broadcast_message(const std::string &message, const User& user);
        void					sendWelcome(User& user);
        void					sendWrongPassword(User& user);
        void					sendCapabilities(User& user);
		void					sendPong(User *user, std::string ping);
		void					sendQuitMsg(User *user);
		void					broadcastChannel(const Channel& channel, const std::string& msg);
		void					sendKick(const User* u, const Channel* c, const std::string target, std::string msg);
		void					sendPermisions(const User* user);
		void					sendInvToTarget(const User* user, const Channel* channel, const User* target);
		void					sendInvConfirm(const User* user, const Channel* channel, const std::string& server, const User* target);
		void					sendStripChannelOper(const User* user, const Channel* channel, const User* target);

		// Commands
		void					cmdNick(User *user, std::string line);
		void					cmdWhois(User *user, std::string line);
		void					cmdOper(User *user, std::string line);
		void					channelKick(const User* u, const std::string& line);
		void					channelTopic(const User* u, const std::string& line);
		void					channelMode(const User *user, const std::string& line);
		void					cmdInvite(const User* user, const std::string& line);

    public:
        Server();
        Server(const Server &other);
        ~Server();
 
        Server  &operator=(const Server &other);

		bool							init_Server();
		void							run_Server();
		void							addChannel(Channel* new_channel);
		void							deleteChannel(User* user, const std::string& name);
		void							removeUser(int fd);
		void							clearChannels(void);
		void							clearUsers(void);
		void        					sendMode(const User* user, const Channel* c, bool isPossitive, const char m);

		// Setters
        bool							set_Port(const std::string &port);
        bool							set_Pass(const std::string &pass);

		//Getters
        const std::string&   			getName() const;
        const std::string&				getPass(void) const;
        const std::map<int, User*>&		getUsers(void) const;
		const std::vector<Channel*>&	getChannels(void) const;
};

std::ostream&   operator<<(std::ostream& out, const Server& obj);
