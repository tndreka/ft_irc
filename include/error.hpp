#include <iostream>
#include <string>
#include <sys/socket.h>

namespace Errors {

	inline void NICKNAMEINUSE(std::string serverName, int fd, std::string nickname) {
		
		std::cout << nickname << std::endl;
		std::string msg = ":" + serverName + " 433 * " + nickname + " :Nickname is already in use\r\n";

		std::cout << msg << std::endl;;
    	send(fd, msg.c_str(), msg.length(), 0);
	}
}
