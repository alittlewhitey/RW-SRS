

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
#include"../include/server.hpp"

void Server::shell_handle(){
	out << "=====================================" << std::endl;
	out << "Server Control"<<std::endl;
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
			if(args.size() < 1){
				out << "Error Args, Usage: add <Ports...> " << std::endl;
				break;
			}
            try{
            for(auto a : args){
                add_room(stoi(a));
            }
            }catch(std::exception e){
                out << "not an integer" << std::endl;
            }
        }break;
        case 2:{
            if(args.size() < 1){
				out << "Error Args, Usage: remove <Ports...> " << std::endl;
				break;
			}
            try{
            for(auto a : args){
                remove_room(stoi(a));
            }
            }catch(std::exception e){
                out << "not an integer" << std::endl;
            }
        }break;
        case 3:{
            if(args.size() >= 1){
				out << "Error Args, Usage: rooms " << std::endl;
				break;
			}
            for(auto& a : rooms){
                out << "port: " << a.first << "\tname: " << a.second->name << "\tnumber: " << a.second->players.size() << std::endl;
            }
        }break;
        case 4:{
            if(args.size() != 1){
				out << "Error Args, Usage: room <Port> " << std::endl;
				break;
			}
			bool done = 0;
            try{
            for(auto& a : rooms){
                if(a.second->port == stoi(args[0])){
                    done = 1;
                    a.second->shell_handle();
                    out << "=====================================" << std::endl;
                    out << "Server Control"<<std::endl;
                }
            }
            }catch(std::exception e){
                out << "not an integer" << std::endl;
                done = 1;
            }
            if(!done){
                out << "no room found" << std::endl;
            }

        }break;
        case 5:{

            close_all_rooms();
            sleep(3);
            return;
        }break;
        case 6:{
            if(args.size() != 2){
				out << "Error Args, Usage: name <Port> <New Name> " << std::endl;
				break;
			}
            try{
                rooms[stoi(args[0])]->name = args[1];
            }catch(std::exception e){
                out << "not an integer" << std::endl;
            }
        }break;
        default:{
			out << "unknown command" << std::endl;
        }break;
        }
    }
}