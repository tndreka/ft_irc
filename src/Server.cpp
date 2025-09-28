/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 15:22:24 by tndreka           #+#    #+#             */
/*   Updated: 2025/09/28 20:24:34 by tndreka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server()
{}

Server::Server(const Server& other)
{
    (void)other;
    std::cout << "Copy server not allowed !  !  !\n";
}

Server& Server::operator=(const Server& other)
{
    (void)other;
    std::cout << "= operator_server not allowed !  !  !\n";
    return *this;
}

Server::~Server()
{}

/*
            ====== Socket_creation ======
        we use socket() function.
		socket(int Domain, int type, int protocol);
		1)First Param: 
		-int Domain -> use AF_INET -> {is an addres family,
		that is used to designate the type of address that your socket can communicate in this case IPV4(internet protocol V4)} 
		2)Second Param:
		-int type -> SOCK_STREAM -> {Provides sequenced, reliable two way connection based byte streams.
		it make sure that the data is not dublicated, it doesnt get lost and its delivered on correct order}
		3) Third param:
		-int protocol -> 0 -> {Protocol specifies a particular protocol to be used with the socke.
		Normally a single protocol exit to support the socket type within the given protocol family wich most of the times is 0}
		RETURN:
			SUCCESS: A file descriptor for the new socket created is being returned.
			ERROR: -1
*/

bool Server::createSocket()
{
    listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1)
        return false;
    else
        return true;
}

