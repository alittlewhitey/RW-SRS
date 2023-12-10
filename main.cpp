

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



#include"including.hpp"

#include<boost/asio.hpp>
#include<vector>
#include<thread>
#include<string>
#include<iostream>

using namespace std;
using namespace boost::asio;
using boost::asio::ip::tcp;

void close_all_rooms(){
	for(auto& a : server.rooms){
		a.second->is_run = 0;
	}
	sleep(1);
	for(auto& a : server.rooms){
		server.remove_room(a.first);
	}
}

int main(){
	cout << "\nThis program is RW-SRS\nCopyright (C) 2023  LittleWhite\n\nThis program is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program.  If not, see <https://www.gnu.org/licenses/>.\n" << endl;
	try{
		string command;
		vector<string> args;
		string temp,temp2;
		istringstream iss;
		while(1){
			while(args.size() >= 1){
				args.erase(args.begin());
			}
			cin >> command;
			getline(cin,temp);
			iss = istringstream(temp);
			while(iss){
				temp2.clear();
				iss >> temp2;
				if(temp2 != "")
					args.push_back(temp2);
			}
			if(command == "add"){
				try{
				for(auto a : args){
					server.add_room(stoi(a));
				}
				}catch(exception e){
					cout << "not an integer" << endl;
				}
			}
			else if(command == "remove"){
				try{
				for(auto a : args){
					server.remove_room(stoi(a));
				}
				}catch(exception e){
					cout << "not an integer" << endl;
				}
			}
			else if(command == "rooms"){
				for(auto& a : server.rooms){
					cout << "port: " << a.first << "\tname: " << a.second->name << "\tnumber: " << a.second->players.size() << endl;
				}
			}
			else if(command == "room"){
				bool done = 0;
				if(args.size() > 1){
					cout << "too much args" << endl;
					goto IF_COMMAND_IS_ROOM;
				}
				try{
				for(auto& a : server.rooms){
					if(a.second->port == stoi(args[0])){
						done = 1;
						a.second->shell_handle();
					}
				}
				}catch(exception e){
					cout << "not a integer" << endl;
				}
				if(!done){
					cout << "no room find" << endl;
				}
		IF_COMMAND_IS_ROOM:
				;//这个分号有意义
			}
			else if(command == "exit"){
				close_all_rooms();
				sleep(3);
				break;
			}
			else{
				std::cout << "unknown command" << endl;
			}
		}
	}catch(exception e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
		close_all_rooms();
		sleep(3);
		return -1;
	}
    return 0;
}