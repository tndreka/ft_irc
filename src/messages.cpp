#include "../include/Server.hpp"

void Server::broadcast_message(const std::string &message, User& user) {
  for (size_t i = 0; i < poll_fds.size(); i++) {
    if (poll_fds[i].fd != listening && poll_fds[i].fd != user.getPoll().fd)
      send(poll_fds[i].fd, message.c_str(), message.length(), 0);
  }
}

void Server::sendWelcome(User& user) {
  std::string nick = user.getNickname();
  std::vector<std::string> replies;

  replies.push_back(":" + _name + " 001 " + nick + " :Welcome to the " +
                    _name + " IRC Server!\r\n");
  replies.push_back(":" + _name + " 002 " + nick + " :Your host is " +
                    _name + "\r\n");
  replies.push_back(":" + _name + " 376 " + nick + " :End of MOTD\r\n");

  for (size_t i = 0; i < replies.size(); ++i) {
    send(user.getPoll().fd, replies[i].c_str(), replies[i].size(), 0);
  }
}

void Server::sendWronPassword(User& user) {
  std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
  std::string err =
      ":" + _name + " 464 " + nick + " :Password incorrect\r\n";
  send(user.getPoll().fd, err.c_str(), err.size(), 0);

  std::string closing =
      "ERROR :Closing link: " + nick + " (Password required)\r\n";
  send(user.getPoll().fd, closing.c_str(), closing.size(), 0);
}

void Server::sendCapabilities(User& user) {
  std::string capReply = ":" + _name + " CAP * LS :\r\n";
  // std::cout << "===>Sending capabilities" << std::endl;

  // std::cout << "fd: " << user.getPoll().fd << std::endl;
  send(user.getPoll().fd, capReply.c_str(), capReply.size(), 0);
  // std::cout << capReply << std::endl;
}

void Server::sendPong(User *user, std::string ping) {

	std::string pong = "PONG " + ping.substr(5) + "\r\n";
	// std::cout << pong << std::endl;
	// std::cout << "===>Send PONG" << std::endl;
	send(user->getPoll().fd, pong.c_str(), pong.size(), 0);
}
