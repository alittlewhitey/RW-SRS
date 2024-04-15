

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



#ifndef SRS_SERVER
#define SRS_SERVER



struct Server{
    std::map<int,std::unique_ptr<Room>> rooms;
    std::ostream& out = std::cout;
    std::istream& in = std::cin;
    void add_room(int port){
        for(auto& i : rooms){
            if(i.first == port)
                return;
        }
        rooms.insert(std::pair<int,std::unique_ptr<Room>>{port,new Room(port)});
    }
    void add_room(int port,std::string name){
        for(auto& i : rooms){
            if(i.first == port){
                i.second->name = name;
                return;
            }
        }
        rooms.insert(std::pair<int,std::unique_ptr<Room>>{port,new Room(port,name)});
    }
    void remove_room(int port){
        for(auto& i : rooms){
            if(i.first == port){
                i.second = nullptr;
                rooms.erase(port);
                return;
            }
        }
    }
    void close_all_rooms(){
	for(auto& a : rooms){
		a.second->is_run = 0;
	}
	sleep(1);
	// for(auto& a : rooms){
	// 	remove_room(a.first);
	// }
    while(rooms.size() != 0){
        remove_room(rooms.begin()->second->port);
    }
}
    void shell_handle();
private:
    std::map<std::string,int> commands={
        {"add",1},
        {"remove",2},
        {"rooms",3},
        {"room",4},
        {"exit",5},
        {"name",6}
	};
};

#endif // !SRS_SERVER