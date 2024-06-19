

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
#include"../include/room.hpp"

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
		a = nullptr;
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
Player_ptr Room::find(unsigned int ID){
	for(auto& p : players){
		if(p->ID == ID){
			return p;
		}
	}
	return 0;
}
Player_ptr Room::find_site(unsigned int site){
	for(auto& p : players){
		if(p->site == site){
			return p;
		}
	}
	return 0;
}
int Room::getVoidSite(){
	int result = -1;
	for(int i = maxPlayer-1;i != -1;--i){
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

	
	players.push_back(make_player(sock,*this));
	auto& p = players[players.size()-1];
	if(p!=nullptr)
		p->run();
		
	if(!is_run){
		return;
	}
	p.remove_all();
	//socket不能立即被析构
	usleep(200000);
}catch(boost::system::system_error e){
	err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << '\t' << e.code() << std::endl;
}catch(std::exception e){
	err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
}
}

void Room::shell_handle(){
	out << "=====================================" << std::endl;
	out << "Room Control\t" << port <<std::endl;
	std::string command;
	std::vector<std::string> args;
	std::string temp,temp2;
	std::istringstream iss;

	while(1){
		//in >> command;
		while(args.size() >= 1){
			args.erase(args.begin());
		}
		in >> command;
		std::getline(in,temp);
		iss = std::istringstream(temp);
		while(iss){
			temp2.clear();
			iss >> temp2;
			if(temp2 != "")
				args.push_back(temp2);
		}
		switch(commands[command]){
		case 1:{
			return;
		}break;
		case 2:{
			if(args.size() != 1){
				out << "Error Args, Usage: say <Text> " << std::endl;
				break;
			}
			std::string text = "";
			for(auto a : args){
				text += a + " ";
			}
			sendSystemBroadCast(text);
		}break;
		case 3:{
			if(args.size() != 2){
				out << "Error Args, Usage: kick <ID> <Reason> " << std::endl;
				break;
			}
			int site = std::stoi(args[0]);
			std::string reason = args[1];
			if(Player_ptr p = find_site(site)){
				p->sendKick(reason);
			}else{
				out << "no player in this site" << std::endl;
				for(auto& a : players){
					out << "ID: " << a->ID << "\tname: " << a->name << "\tsite: "<< a->site << "\tIP: " << a->ep.address() << std::endl;
				}
			}
		}break;
		case 4:{
			if(args.size() != 1){
				out << "Error Args, Usage: uplist <add/remove> " << std::endl;
				break;
			}
			std::string state = args[0];
			if(state == "add"){
				is_open = 1;
				openRoom(*this);
			}else if(state == "remove"){
				is_open = 0;
			}else{
				out << "unknown command" << std::endl;
			}
		}break;
		case 5:{
			if(args.size() != 0){
				out << "Error Args, Usage: players " << std::endl;
				break;
			}
			if(players.size() == 0){
				out << "no player" << std::endl;
			}
			for(auto& a : players){
				out << "ID: " << a->ID << "\tname: " << a->name << "\tsite: "<< a->site << "\tIP: " << a->ep.address() << std::endl;
			}
		}break;
		case 6:{
			if(args.size() != 1){
				out << "Error Args, Usage: su <ID> " << std::endl;
				break;
			}
			int site = std::stoi(args[0]);
			if(Player_ptr p = find_site(site)){
				p->isAdmin = 1;
			}else{
				out << "no player in this site" << std::endl;
			}
		}break;
		case 7:{
			if(args.size() != 1){
				out << "Error Args, Usage: unsu <ID> " << std::endl;
				break;
			}
			int site = std::stoi(args[0]);
			if(Player_ptr p = find_site(site)){
				p->isAdmin = 0;
			}else{
				out << "no player in this site" << std::endl;
			}
		}break;
		default:{
			out << "unknown command" << std::endl;
		}break;
		}
	}
}