#include "../include/Server.hpp"

void Server::broadcast_message(const std::string &message, int sender_fd) {
  for (size_t i = 0; i < poll_fds.size(); i++) {
    if (poll_fds[i].fd != listening && poll_fds[i].fd != sender_fd)
      send(poll_fds[i].fd, message.c_str(), message.length(), 0);
  }
}

void Server::sendWelcome(int fd) {
  std::string nick = client_nicknames[fd];
  std::vector<std::string> replies;

  replies.push_back(":" + serverName + " 001 " + nick + " :Welcome to the " +
                    serverName + " IRC Server!\r\n");
  replies.push_back(":" + serverName + " 002 " + nick + " :Your host is " +
                    serverName + "\r\n");
  replies.push_back(":" + serverName + " 376 " + nick + " :End of MOTD\r\n");

  for (size_t i = 0; i < replies.size(); ++i) {
    send(fd, replies[i].c_str(), replies[i].size(), 0);
  }
}

void Server::sendWronPassword(int fd) {
  std::string nick = client_nicknames[fd].empty() ? "*" : client_nicknames[fd];
  std::string err =
      ":" + serverName + " 464 " + nick + " :Password incorrect\r\n";
  send(fd, err.c_str(), err.size(), 0);

  std::string closing =
      "ERROR :Closing link: " + nick + " (Password required)\r\n";
  send(fd, closing.c_str(), closing.size(), 0);
}

void Server::sendCapabilities(int fd) {
  std::string capReply = ":" + serverName + " CAP * LS :\r\n";
  std::cout << "===>Sending capabilities" << std::endl;

  send(fd, capReply.c_str(), capReply.size(), 0);
}
