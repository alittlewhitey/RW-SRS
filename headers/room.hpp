

    // This file is a part of RW-SRS
    // Copyright (C) 2023  LittleWhite

    // This program is free software: you can redistribute it and/or modify
    // it under the terms of the GNU General Public License as published by
    // the Free Software Foundation, either version 3 of the License, or
    // (at your option) any later version.

    // This program is distributed in the hope that it will be useful,
    // but WITHOUT ANY WARRANTY; without even the implied warranty of
    // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    // GNU General Public License for more details.

    // You should have received a copy of the GNU General Public License
    // along with this program.  If not, see <https://www.gnu.org/licenses/>.



#pragma once

#ifndef SRS_PLAYER
#include"player.hpp"
#endif
#ifndef SRS_OPEN
#include "open.hpp"
#endif
#ifndef SRS_ROOM
#define SRS_ROOM
#include<string>
#include<boost/asio.hpp>
struct Room{
	enum State{battleroom,ingame};

	std::vector<std::jthread> roomThread;

	//Room context
	std::string mapName = "Crossing Large (10p)";
	std::string unitPath = "./gameData/assets/units";
	std::string game_mode = "skirmishMap";
	std::string state = "battleroom";
	std::string created_by = "RW_SRS";

	int maxPlayer = 20;
	int game_version = 176;
	bool is_gaming = 0;
	bool isAfking = 0;
	int afkTime = 0;


	bool is_run = 1;
	bool is_open = 0;
	int port;
	std::string name = "none";

	//Room state
	std::vector<std::unique_ptr<Player>> players = {};
	boost::asio::ip::tcp::socket* waiting_socket = 0;

	float income = 1;
	int defaultCredits = 4000;
	bool noNukes = 1;
	bool sharedControl = 0;
	bool gamePaused = 0;
	int initUnit = 1;
	int maxUnit = 250;
	int mist = 2;



	


    boost::asio::ip::tcp::acceptor acptr{ioc,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),port)};
    Room(int port):port(port){
		roomThread.push_back(std::jthread([this](){this->acceptClient();}));
		roomThread[roomThread.size()-1].detach();
	}
    Room(int port,std::string name):port(port),name(name){
		roomThread.push_back(std::jthread([this](){this->acceptClient();}));
		roomThread[roomThread.size()-1].detach();
	}
	~Room(){
		is_run = 0;
		is_open = 0;
		is_gaming = 0;

		if(waiting_socket != 0){
			boost::asio::ip::tcp::socket sock(ioc);
			boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"),port);
			sock.connect(ep);
			try{
				sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
				sock.close();
				sock.cancel();
			}catch(boost::system::system_error e){
				err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
				sock.close();
			}
			
		}

		for(auto& a : players)
			a = 0;
		for(auto& a: roomThread)
			if(a.get_stop_source().stop_possible()){
				a.request_stop();
				if(!a.get_stop_source().stop_requested())
					err << __FILE__ << '\t' << __LINE__ << '\t' << "room thread had not stopped" << std::endl;
			}
	}
	void sendChatBroadCast(const std::string& msg,const std::string& sendBy,int team);
	void sendSystemBroadCast(const std::string& msg);
	

	void acceptClient(){
	try{
		boost::asio::ip::tcp::socket sock(ioc);
		waiting_socket = &sock;
		acptr.accept(sock);
		waiting_socket = 0;
		if(!is_run){
			return;
		}

		roomThread.push_back(std::jthread([this](){this->acceptClient();}));
		roomThread[roomThread.size()-1].detach();

		
		players.push_back(std::unique_ptr<Player>(new Player(sock,*this)));
		auto& p = players[players.size()-1];
		if(p!=0)
			p->run();
		p = 0;
		//socket不能立即被析构
		usleep(200000);
	}catch(boost::system::system_error e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << '\t' << e.code() << std::endl;
	}catch(std::exception e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
	}
	}

	const Player* find(unsigned int ID);
	const Player* find_site(unsigned int site);
	bool find_if(unsigned int ID){
		if(find(ID)){
			return 1;
		}
		return 0;
	}
	inline int getVoidSite();
	void shell_handle(){
		std::cout << "=====================================" << std::endl;
		std::cout << "Room Control\t" << port <<std::endl;
		std::string command;
		while(1){
			std::cin >> command;
			if(command == "exit"){
				std::cout << "=====================================" << std::endl;
				std::cout << "Program Control" << std::endl;
				break;
			}else if(command == "say"){
				std::string text;
				std::getline(std::cin,text);
				sendSystemBroadCast(text);
			}else if(command == "kick"){
				int site;
				std::string reason;
				std::cin >> site;
				std::getline(std::cin,reason);
				if(const Player* p = find_site(site)){
					const_cast<Player*>(p)->sendKick(reason);
				}else{
					std::cout << "no player in this site" << std::endl;
					for(auto& a : players){
						std::cout << "ID: " << a->ID << "\tname: " << a->name << "\tsite: "<< a->site << "\tIP: " << a->ep.address() << std::endl;
					}
				}
			}else if(command == "uplist"){
				std::string state;
				std::cin >> state;
				if(state == "add"){
					is_open = 1;
					openRoom(*this);
				}else if(state == "remove"){
					is_open = 0;
				}else{
					std::cout << "unknown command" << std::endl;
				}
			}else if(command == "players"){
				if(players.size() == 0){
					std::cout << "no player" << std::endl;
				}
				for(auto& a : players){
					std::cout << "ID: " << a->ID << "\tname: " << a->name << "\tsite: "<< a->site << "\tIP: " << a->ep.address() << std::endl;
				}
			}else{
				std::cout << "unknown command" << std::endl;
			}
		}
	}
};
#endif // !SRS_ROOM