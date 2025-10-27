/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tndreka <tndreka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 17:53:43 by tndreka           #+#    #+#             */
/*   Updated: 2025/10/22 13:02:56 by tndreka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

volatile sig_atomic_t signal_flag = 0;

static void	handleSigint(int) {
	signal_flag = 1;
}

static void signalInit(void)
{
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handleSigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
}


int main(int ac, char *av[]) {
	Server server;
	signalInit();

	if (ac != 3) {
		std::cerr << "Wrong Arguments! Usage:" << av[0] << " <port> <password>\n";
		return 1;
	}
	if (!server.set_Port(av[1]))
		return 1;
	if (!server.set_Pass(av[2]))
		return 1;
	if (server.init_Server() == false)
		return 1;
	std::cout << "Connected to the server: '" << server.getName() << "'" << std::endl;
	server.run_Server();
}
