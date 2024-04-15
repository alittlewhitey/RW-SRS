

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


struct Room;
class Player{
	static unsigned int IDcounter;
public:

	//用于程序的数据
	const static int heart_beat_times_max = 30;
	int heart_beat_times = 0;
	long long send_time = 0;
	long long receive_time = 0;
	bool is_alive = 1;
	//to resolve memory rewrite after destruct
	bool* heart_beat_exist = 0;
	bool* team_beat_exist = 0;

	// -1 ==> N/A   -2 ==> -   -99 ==> HOST
	int ping = -2;
	unsigned int ID = 0;
	Room& room;
	boost::asio::ip::tcp::socket& sock;
	boost::asio::ip::tcp::endpoint ep;

	std::jthread heart_beat_thread;
	std::jthread team_beat_thread;


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


	
	Player(boost::asio::ip::tcp::socket& sock,Room& room);
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
	void team_beat();
	void sendKick(const std::string& reason);
	void sendChatMessage(const std::string& msg,const std::string& sendBy,int team);
	void sendSystemMessage(const std::string& msg);
	void sendGameTickCommand(int tick, const int sendBy = -1,std::string arr = "");
	void sendTick(int tick);
	void registerConnect(IPacket& ipac);
	void handlePacket(PacketType type,const IPacket& p);

	void run();

	
};
unsigned int Player::IDcounter = 0;


#endif // !SRS_PLAYER