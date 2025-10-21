/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 15:22:31 by tndreka           #+#    #+#             */
/*   Updated: 2025/10/02 17:34:19 by tndreka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <arpa/inet.h> // for inet_addr
#include <cstdlib>
#include <fcntl.h> // fcntl()
#include <iostream>
#include <map>
#include <netdb.h>      // defines NI_MAXHOST
#include <netinet/in.h> // for socketaddr
#include <poll.h>       // for poll()
#include <sstream>      //for istringstream
#include <string.h>
#include <string>
#include <sys/socket.h> // for socket liberary
#include <unistd.h>
#include <vector>

#ifndef MAX_BUFF
#define MAX_BUFF 4444
#define IRC_OPER_NAME "root"
#define IRC_OPER_PASS "secretpass"
#endif

#include "User.hpp"
#include "../include/error.hpp"

class Server {
    private:
        std::string serverName;
        std::string password;
        std::map<int, User*> _activeUsers;

        int port;
        int listening;
        sockaddr_in hint;
        sockaddr_in client;
        socklen_t clientSize;
        char host[NI_MAXHOST];    // 1025 max num of hosts
        char service[NI_MAXSERV]; // 32 max num of serv
        std::vector<pollfd> poll_fds;

        pollfd listening_fd;
        int poll_count;
        int new_connection;

        bool incoming_data;
        bool client_hungup;
        bool is_listening;
        bool err;

        char buff[MAX_BUFF];
        int bytes_recived;

        // Server
        bool createSocket();
        bool bindSocket();
        bool listenSocket();
        void accept_connection();
        bool init_poll();
        void event_check(size_t index);
        void event_state();
        void handle_new_host();
        void handle_messages(size_t index);
        void handle_disconn_err_hungup(size_t index);

		// Helpers
        void remove_from_vector(size_t index);
		bool isNickInUse (std::string& attemptedNick);
		bool isValidNick(std::string& attemptedNick);
		void removeUser(int fd);
		bool isUserAlreadySigned(User& user);
		void closeConnection(int fd);

		// Parsing
		int authenticateParser(User& user);
		std::string authenticateNickname(User& user, std::string line);
		void parse(User& user, std::string buff);
 
        // Messages
        void broadcast_message(const std::string &message, User& user);
        void sendWelcome(User& user);
        void sendWrongPassword(User& user);
        void sendCapabilities(User& user);
		void sendPong(User *user, std::string ping);

		// Commands
		void cmdNick(User *user, std::string line);
		void cmdWhois(User *user, std::string line);
		void cmdOper(User *user, std::string line);


    public:
        Server();
        Server(const Server &other);
        Server &operator=(const Server &other);
        ~Server();
        bool set_Port(const std::string &port);
        bool set_Pass(const std::string &pass);
        int get_Port() const;
        int init_Server();
        void run_Server();
        std::string getServerName() const;
};
