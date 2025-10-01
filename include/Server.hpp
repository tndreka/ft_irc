/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 15:22:31 by tndreka           #+#    #+#             */
/*   Updated: 2025/10/01 16:50:04 by tndreka          ###   ########.fr       */
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

#ifndef MAX_BUFF
#define MAX_BUFF 4444
#endif

class Server
{
private:
    int             port;
    std::string     password;
    int             listening;
    sockaddr_in     hint;
    sockaddr_in     client;
    socklen_t       clientSize;
    char		    host[NI_MAXHOST]; // 1025 max num of hosts
	char		    service[NI_MAXSERV]; // 32 max num of serv
    char            *client_ip;
    std::vector<pollfd> poll_fds;
    std::map<int, std::string> clients;
    pollfd          listening_fd;
    pollfd          new_client_fd; //new structor for the new accepted host
    int             poll_count;
    int             new_connection;
    bool            incoming_data;
	bool            client_hungup;
	bool            is_listening;
	bool            err;
    char            buff[MAX_BUFF];
    int	            bytes_recived;
    
    //Server
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
    //helper
    void remove_from_vector(size_t index);
    
    public:
    Server();
    Server(const Server& other);
    Server& operator=(const Server& other);
    ~Server();
    bool set_Port(const std::string& port);
    bool set_Pass(const std::string& pass);
    int  get_Port()const;
    int  init_Server();
    void run_Server();
};

