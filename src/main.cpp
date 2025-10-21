/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 17:53:43 by tndreka           #+#    #+#             */
/*   Updated: 2025/10/01 16:56:28 by tndreka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

int main(int ac, char *av[]) {
	Server server;

	if (ac != 3) {
		std::cerr << "Wrong Arguments! Usage:" << av[0] << " <port> <password>\n";
		return 1;
	}
	if (!server.set_Port(av[1]))
		return 1;
	if (!server.set_Pass(av[2]))
		return 1;
	if (server.init_Server() == 1)
		return 1;
	std::cout << "Connected to the server: '" << server.getName() << "'" << std::endl;
	server.run_Server();
}
