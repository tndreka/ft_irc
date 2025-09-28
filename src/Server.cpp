/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 15:22:24 by tndreka           #+#    #+#             */
/*   Updated: 2025/09/28 20:16:02 by tndreka          ###   ########.fr       */
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
}

Server::~Server()
{}