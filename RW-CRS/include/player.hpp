

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
#define SRS_PLAYER


#include"../including.hpp"
class Player_ptr;
class Player{
	static unsigned int IDcounter;

	std::vector<std::shared_ptr<Player>*> ptrs;

public:

	//用于程序的数据
	const static int heart_beat_times_max = 30;
	int heart_beat_times = 0;
	long long send_time = 0;
	long long receive_time = 0;
	bool is_alive = 1;
	//to resolve memory rewrite after destruct
	bool* heart_beat_exist = 0;

	// -1 ==> N/A   -2 ==> -   -99 ==> HOST
	int ping = -2;
	unsigned int ID = 0;
	Room& room;
	boost::asio::ip::tcp::socket& sock;
	boost::asio::ip::tcp::endpoint ep;

	std::jthread heart_beat_thread;


	//用于游戏的数据

	bool isAdmin = 0;
	bool isNull = 1;
	bool isRandy = 0;
	int site = -1;
	int team = 0;
	int credits = 4000;
	bool controlable = 1;
	bool sharedControl = 1;
	int startUnit = 1;
	int color = 0;
	std::string name = "unnamed";

	typedef std::function<int(std::string)> CallBackFunc;
	CallBackFunc popupProc;


private:
	Player(boost::asio::ip::tcp::socket& sock,Room& room);
public:

	friend Player_ptr make_player(boost::asio::ip::tcp::socket& sock,Room& room);
	~Player();
	
	bool operator ==(const Player& rhs){
		if(this->ID == rhs.ID){
			return 1;
		}else{
			return 0;
		}
	}
	bool operator !=(const Player& rhs){
		if(this->ID == rhs.ID){
			return 0;
		}else{
			return 1;
		}
	}
	void sendServerInfo(bool);
	bool getPlayerInfo(IPacket& ipac);
	void heart_beat();
	void sendKick(const std::string& reason);
	void sendChatMessage(const std::string& msg,const std::string& sendBy,int team);
	void sendSystemMessage(const std::string& msg);
	void sendGameTickCommand(int tick, const int sendBy = -1,std::string arr = "");
	void sendTick(int tick);
	void sendPopUp(const std::string& msg,CallBackFunc);
	void handlePopUp(const std::string& msg);
	void registerConnect(IPacket& ipac);
	void handlePacket(PacketType type,const IPacket& p);
	void start();
	void run();

	
};
unsigned int Player::IDcounter = 0;



class Player_ptr{
	Player* player = nullptr;
	std::vector<Player_ptr*>* ptrs = new std::vector<Player_ptr*>();
	void remove_ptr_from_ptrs(Player_ptr & ptr){
		auto size = ptrs->size();
		if(size <= 1&&ptr.player != nullptr){
			delete ptr.player;
			ptr.player = nullptr;
			return;
		}else if(size >1){
			for(int i = 0;i!=size;++i){
				if((*ptrs)[i] == &ptr){
					ptrs->erase(ptrs->begin() + i);
					break;
				}
			}
			ptr.player = nullptr;
			ptr.ptrs = new std::vector<Player_ptr*>();
			ptr.ptrs->push_back(&ptr);
			return;
		}
	}
	void copy_ptr(const Player_ptr& ptr){
		if(ptrs->size() <= 1){
			if(player != nullptr)
				delete player;
			delete ptrs;
			player = ptr.player;
			ptrs = ptr.ptrs;
			ptrs->push_back(this);
			return;
		}
		//else  size > 1
		if(player == nullptr){
			auto p = ptrs;
			for(auto a : *ptrs){
				a->ptrs =  new std::vector<Player_ptr*>();
				a->ptrs->push_back(a);
			}
			delete p;

			delete ptrs;//It's useful
		}else{
			auto size = ptrs->size();
			for(int i = 0;i!= size;++i){
				if((*ptrs)[i] == this){
					ptrs->erase(ptrs->begin()+i);
					break;
				}
			}
		}
		player = ptr.player;
		ptrs = ptr.ptrs;
		ptrs->push_back(this);
	}
public:
	Player_ptr(Player* player){
		this->player = player;
		ptrs->push_back(this);
	}
	Player_ptr(const Player_ptr& ptr){
		copy_ptr(ptr);
	}
	Player_ptr(Player_ptr&& ptr){
		player = ptr.player;
		ptrs = ptr.ptrs;
		ptrs->push_back(this);
		ptr.player = nullptr;
		remove_ptr_from_ptrs(ptr);
	}
	Player_ptr& operator= (const Player_ptr& ptr){
		copy_ptr(ptr);
		return *this;
	}
	bool operator==(const Player_ptr& ptr){
		if(this->player == ptr.player)
			return 1;
		return 0;
	}
	bool operator!=(const Player_ptr& ptr){
		if(this->player == ptr.player)
			return 0;
		return 1;
	}
	Player* operator->()const{
		return player;
	}
	operator bool(){
		if(player == nullptr)
			return 0;
		return 1;
	}
	void remove_all(){
		auto size = ptrs->size();
		for(int i = 0;i!=size;++i){
			if((*ptrs)[i] != this){
				remove_ptr_from_ptrs(*(*ptrs)[i]);
				(*ptrs)[i]->player = nullptr;
				(*ptrs)[i]->ptrs = new std::vector<Player_ptr*>();
				(*ptrs)[i]->ptrs->push_back((*ptrs)[i]);
			}
		}
		delete player;
		player = nullptr;
	}
	~Player_ptr(){
		remove_ptr_from_ptrs(*this);
		delete this->ptrs;
	}
};

#endif // !SRS_PLAYER