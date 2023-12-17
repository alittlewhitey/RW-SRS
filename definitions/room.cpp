

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
#include"../headers/room.hpp"

Room::~Room(){
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
void Room::sendChatBroadCast(const std::string& msg,const std::string& sendBy,int team){
	for(auto& a : players){
		a->sendChatMessage(msg,sendBy,team);
	}
}
void Room::sendSystemBroadCast(const std::string& msg){
	sendChatBroadCast(msg,"SERVER",5);
}
const Player* Room::find(unsigned int ID){
	for(auto& p : players){
		if(p->ID == ID){
			return p.get();
		}
	}
	return 0;
}
const Player* Room::find_site(unsigned int site){
	for(auto& p : players){
		if(p->site == site){
			return p.get();
		}
	}
	return 0;
}
int Room::getVoidSite(){
	int result = -1;
	for(int i = maxPlayer;i != 0;--i){
		for(auto& a : players){
			if(a->site != i)
				break;
		}
		result = i;
	}
	return result;
}
void Room::acceptClient(){
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

void Room::shell_handle(){
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