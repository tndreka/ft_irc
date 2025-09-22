/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka < tndreka@student.42heilbronn.d    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 17:53:43 by tndreka           #+#    #+#             */
/*   Updated: 2025/09/22 15:42:28 by tndreka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string.h>
#include <unistd.h> 
#include <sys/socket.h> // for socket liberary
#include <netinet/in.h> // for socketaddr
#include <arpa/inet.h> // for inet_addr


/*====== TEST ========*/

int main(int ac, char *av[])
{
	/*
				=================== SOCKET =========================
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
	//create socket
	int listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == -1)
		return std::cerr << "Can't create Socket" << std::endl, -1;
	
	/*
				================== sockaddr_in ===================
		this is a structure used  to represent IPv4 internet domain socket address.
		it is designed for IPv4 only and it contains mambers for the family addres as: 
		->address_family(sin_family) = should be set as AF_INET to use the IPv4
		->port_number(sin_port) = holdsthe portnumber in the network byte order requiring 
		conversion from the host byte order using fucntion hstons().
		->and IPv4 adress = this is the 32 bit IPv4 addres which it can be assigned manually or by using
		functions like inet_addr()
		
		htons(uint16_t hostshort);
		->this function converts the unsigned short int hostshort from host byte order to network byte order.

		inet_addr(const char *p)
		
	*/
		
	//bind the socket to an IP
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.s_addr = inet_addr("127.0.0.1");
	/*
				=================== BIND ======================
		bind() function assigns the address specified by the hinf to the socked referred to by the
		file descriptor listening.
		
		bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
		Parameter:
		1) sockfd - socket file descriptor (listening) 
		2) addr - pointer to sockaddr structure (cast hint to sockaddr *)
		3) addrlen - size of the address structure (sizeof(hint))
		
		RETURN:
			Success => 0
			ERROR => -1
	*/
	if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1)
	{
		std::cerr << "Cant bind IP to PORT" << std::endl;
		close(listening);
		return -1;
	}
	//
	return 0;
}