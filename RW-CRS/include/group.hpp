

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
#ifndef SRS_GROUP
#define SRS_GROUP

#include"../including.hpp"

struct Group{
    Room& room;
	std::vector<std::shared_ptr<Player>> players = {};


	void sendChatBroadCast(const std::string& msg,const std::string& sendBy,int team);
	void sendSystemBroadCast(const std::string& msg);
    const Player* find(unsigned int ID);
	const Player* find_site(unsigned int site);
	bool find_if(unsigned int ID){
		if(find(ID)){
			return 1;
		}
		return 0;
	}
    Group(Room& room):room(room){}
    bool addPlayer(std::shared_ptr<Player> player){

    }
    bool removePlayer(int ID){

    }
};

#endif //!SRS_GROUP