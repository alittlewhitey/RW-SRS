

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