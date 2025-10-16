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
#include <sys/socket.h> // for socket liberary
#include <unistd.h>
#include <vector>

#ifndef MAX_BUFF
#define MAX_BUFF 4444
#endif

// keeps the track of every client state
enum ClientState {
  WAITTING_PASS,
  WAITING_NICKNAME,
  WAITING_USER,
  VERIFYED,
  REGISTERED
};

class Server {
private:
  std::string serverName;
  int port;
  std::string password;
  int listening;
  sockaddr_in hint;
  sockaddr_in client;
  socklen_t clientSize;
  char host[NI_MAXHOST];    // 1025 max num of hosts
  char service[NI_MAXSERV]; // 32 max num of serv
  char *client_ip;
  std::vector<pollfd> poll_fds;
  std::map<int, std::string> clients;          // fd -> client IP
  std::map<int, ClientState> client_states;    // fd -> authentication state
  std::map<int, std::string> client_nicknames; // fd-> nickname
  std::map<int, std::string> client_username;  // fd ->username
  pollfd listening_fd;
  pollfd new_client_fd; // new structor for the new accepted host
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
  void broadcast_message(const std::string &message, int sender_fd);
  int parser_irc(int client_fd);
  void remove_from_vector(size_t index);

  // Messages
  void sendWelcome(int fd);
  void sendWronPassword(int fd);
  void sendCapabilities(int fd);

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
