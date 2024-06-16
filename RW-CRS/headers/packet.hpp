

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

#ifndef SRS_PACKET
#define SRS_PACKET
// #include<codecvt>
// #include<locale>
#include<string>
#include<boost/asio.hpp>

enum PacketType{
    //Server Commands
    PACKET_REGISTER_CONNECTION = 161,
	PACKET_TEAM_LIST = 115,
	PACKET_HEART_BEAT = 108,
	PACKET_SEND_CHAT = 141,
	PACKET_SERVER_INFO = 106,
	PACKET_POPUP = 117,
	
	//Client Commands
	PACKET_PREREGISTER_CONNECTION = 160,
	PACKET_HEART_BEAT_RESPONSE = 109,
	PACKET_ADD_CHAT = 140,
	PACKET_PLAYER_INFO = 110,
	PACKET_DISCONNECT = 111,
	PACKET_KICK = 150,
	PACKET_POPUP_RESPONSE = 118,
	
	//Game Commands
	PACKET_ADD_GAMECOMMAND = 20,
	PACKET_TICK = 10,
	PACKET_SYNC_CHECKSUM = 35
};
class Player;
struct Packet{
	struct PacketData{
		std::string data;
		struct DataIterator{
			PacketData& pData;
			int index;
			DataIterator (PacketData& pData,int index):pData(pData),index(index){}
			template<typename T>
			explicit operator T();
		};
		template<typename T>
		void push_back(const T t);
		DataIterator operator[](int index);
	};
	PacketData pData;
};
struct IPacket :public Packet{
	//char* buffer;
	int index = 0;
	PacketType type;
	int size = 0;
	template<typename T>
	T read();
	std::string read();
	IPacket(const std::string& str){
		pData.data = str;
	}
};
struct OPacket :public Packet{
	int index = 0;
	PacketType type;
	int size = 0;
	template<typename T>
	void write(const T t);
	//自带写入字符串大小
	void write(const char* str);
	//自带写入字符串大小
	void write(const std::string& str);
	void write_Gzip(const std::string& key,const OPacket& data);
	void write_data(const char*,size_t);
	void write_data(const std::string&);
	friend void operator<<(const boost::asio::ip::tcp::socket& sock,const OPacket& p);
	OPacket(){};
};
#endif //!SRS_PACKET