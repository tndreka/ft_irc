/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_helpers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 16:28:32 by tndreka           #+#    #+#             */
/*   Updated: 2025/10/01 16:41:45 by tndreka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::remove_from_vector(size_t index)
{
    if (index >= poll_fds.size())
        return ;
    if (index < poll_fds.size() - 1)
        poll_fds[index] = poll_fds[poll_fds.size() - 1];
    poll_fds.pop_back();
}

bool Server::set_Port(const std::string& port)
{
    int portNum;

    portNum = atoi(port.c_str());
    if(portNum < 1024 || portNum > 65535)
    {
        std::cerr<< "Port number must be between 1024 - 65535\n";
        return false;
    }
    else
    {
        this->port = portNum;
        return true;
    }
}

