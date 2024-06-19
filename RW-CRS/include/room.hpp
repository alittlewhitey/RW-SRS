

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

#ifndef SRS_ROOM
#define SRS_ROOM

#include"../including.hpp"
struct Room{
	enum State{battleroom,ingame};

	std::vector<std::jthread> roomThread;


    std::ostream& out = std::cout;
    std::istream& in = std::cin;

	//Room context
	std::string mapName = "Crossing Large (10p)";
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
	//std::vector<std::shared_ptr<Player>> players = {};
	std::vector<Player_ptr> players = {};
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
	~Room();
	void sendChatBroadCast(const std::string& msg,const std::string& sendBy,int team);
	void sendSystemBroadCast(const std::string& msg);
	

	void acceptClient();

	Player_ptr find(unsigned int ID);
	Player_ptr find_site(unsigned int site);
	bool find_if(unsigned int ID){
		if(find(ID)){
			return 1;
		}
		return 0;
	}
	inline int getVoidSite();
	void shell_handle();
private:
	std::map<std::string,int> commands={
		{"exit",1},
		{"say",2},
		{"kick",3},
		{"uplist",4},
		{"players",5},
		{"su",6},
		{"unsu",7}
	};
};
#endif // !SRS_ROOM