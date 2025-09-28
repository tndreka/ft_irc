/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 15:22:31 by tndreka           #+#    #+#             */
/*   Updated: 2025/09/28 19:38:33 by tndreka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string.h>
#include <unistd.h> 
#include <sys/socket.h> // for socket liberary
#include <netinet/in.h> // for socketaddr
#include <arpa/inet.h> // for inet_addr
#include <netdb.h> // defines NI_MAXHOST 
#include <unistd.h>
#include <fcntl.h> // fcntl()
#include <poll.h> // for poll()
#include <vector>
#include <map>
class Server
{
private:
    int listening;
    sockaddr_in hint;
    sockaddr_in client;
    socklen_t	clientSize;
    char		host[NI_MAXHOST]; // 1025 max num of hosts
	char		service[NI_MAXSERV]; // 32 max num of serv
    std::vector<pollfd> poll_fds;
    pollfd listening_fd;
    int poll_count;
    bool incoming_data = false;
	bool client_hungup = false;
	bool is_listening = false;
	bool err = false;
    char buff[4444];
    std::map<int, std::string> clients;
    
    //Helper
    bool createSocket();
    bool bindSocket();
    bool listenSocket();
public:
    Server();
    Server(const Server& other);
    Server& operator=(const Server& other);
    ~Server();
};

