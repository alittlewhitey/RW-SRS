

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
#include"../headers/player.hpp"

Player::Player(boost::asio::ip::tcp::socket& sock,Room& room):sock(sock),ep(sock.remote_endpoint()),room(room),heart_beat_thread(std::jthread([this](){this->heart_beat();})),team_beat_thread(std::jthread([this](){this->team_beat();})),site(room.getVoidSite()){
	this->ID = IDcounter;
	++IDcounter;
	heart_beat_thread.detach();
	team_beat_thread.detach();
}
Player::~Player(){
	if(heart_beat_thread.request_stop()){
		err << __FILE__ << '\t' << __LINE__ << '\t' << "heart beat thread had stoped" << std::endl;
	}
	if(team_beat_thread.request_stop()){
		err << __FILE__ << '\t' << __LINE__ << '\t' << "team beat thread had stoped" << std::endl;
	}
	for(int i = 0;i!=room.players.size();++i){
		if(!room.players[i]){
			room.players.erase(room.players.begin()+i);
			--i;
		}
	}
	try{
		sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		sock.cancel();
		sock.close();
	}catch(std::exception e){
		sock.close();
	}
}





void Player::sendServerInfo(){
	try{
		OPacket p;
		OPacket p2;
		p.write("com.corrodinggames.rts");
		p.write<int>(room.game_version);
		//map
		p.write<int>(0);
		p.write(room.mapName);
		p.write<int>(room.defaultCredits);
		p.write<int>(2);
		p.write<bool>(1);
		p.write<int>(1);
		p.write<char>(7);
		p.write<bool>(0);
		//admin ui
		p.write<bool>(isAdmin);
		p.write<int>(250);
		p.write<int>(250);
		p.write<int>(1);
		p.write<float>(1.0);
		//no nukes
		p.write<bool>(room.noNukes);
		p.write<bool>(0);
		p.write<bool>(0);
		//shared control
		p.write<bool>(room.sharedControl);
		p.write<bool>(0);
		p.write<bool>(0);
		//audiences
		p.write<bool>(1);
		p.write<bool>(0);



		p.type = PACKET_SERVER_INFO;
		sock << p;

	}catch(std::exception e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
		is_alive = 0;
	}
}
bool Player::getPlayerInfo(IPacket& ipac){//TODO:
	try{
		ipac.read();
		ipac.read<int>();
		ipac.read<int>();
		ipac.read<int>();
		std::string name = ipac.read();
		this->name = name;
		if(room.players.size() >= room.maxPlayer){
			sendKick("服务器已满");
			is_alive = 0;
			return 0;
		}
		if(room.is_gaming){
			site = -1;
		}else{
			site = room.getVoidSite();
		}
		if((site == -1)&&(room.is_gaming)){
			sendSystemMessage("观战模式");
		}
		return 1;
	}catch(std::exception e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
		is_alive = 0;
		return 0;
	}
}
void Player::heart_beat(){
	static std::random_device rd;
	static std::mt19937_64 gen(rd());
	static std::uniform_int_distribution<long long> distrib(1e12,3e12);
	try{
	while(room.is_run&is_alive){
		if(heart_beat_times > heart_beat_times_max){
			is_alive = 0;
		}
		heart_beat_times ++;
		send_time = clock();
		OPacket p;
		p.type = PACKET_HEART_BEAT;


		p.write<long long>(distrib(gen));
		p.write<char>(0);
		sock << p;
		sleep(5);

	}
	}catch(std::exception e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
		is_alive = 0;
	}
}
void Player::team_beat(){
	try{
	while(room.is_run&is_alive){
		timeval t1;
		gettimeofday(&t1,NULL);
		


		OPacket p;
		//Player position
		p.write<int>(site);
		p.write<bool>(room.is_gaming);
		//Largest Player
		p.write<int>(room.maxPlayer);
		OPacket p2;
		for(int i = 1;i!=room.maxPlayer+1;++i){
			const Player* a = room.find_site(i);
			if(!a){
				p2.write<bool>(0);
				continue;
			}
			p2.write<bool>(1);
			p2.write<int>(0);
			
			p2.write<unsigned char>((unsigned char)a->site);
			p2.write<int>(a->credits);
			p2.write<int>(a->team);
			p2.write<bool>(1);
			if(a->isAdmin)
				p2.write(a->name+" (Admin)");
			else 
				p2.write(a->name);
			p2.write<bool>(1);
			p2.write<int>(a->ping);
			p2.write<long long>(t1.tv_sec*1000+t1.tv_usec/1000);
			p2.write<bool>(0);
			p2.write<int>(0);
			p2.write<int>(a->site);
			p2.write<char>(0);
			p2.write<bool>(0);
			p2.write<bool>(0);
			p2.write<bool>(0);
			p2.write<bool>(0);
			p2.write<int>(-9999);
			
		}
		p.write_Gzip("teams",p2);
		//frog 
		p.write<int>(room.mist);
		p.write<int>(room.defaultCredits);
		p.write<bool>(1);
		//ai dificuty
		p.write<int>(1);
		p.write<char>(4);
		p.write<int>(room.maxUnit);
		p.write<int>(room.maxUnit);
		//init unit
		p.write<int>(room.initUnit);
		//income speed
		p.write<float>(room.income);
		//no nukes
		p.write<bool>(room.noNukes);
		p.write<bool>(0);
		p.write<bool>(0);
		//shared control
		p.write<bool>(room.sharedControl);
		//game pause
		p.write<bool>(room.gamePaused);

		p.type = PACKET_TEAM_LIST;

		std::string s = p.pData.data;
		std::cout << std::hex;
		for(int i = 0;i!=s.size();++i){
			std::cout << (unsigned int)(unsigned char)(s[i]) << ' ';
		}
		std::cout << std::endl;

		sock << p;

		sleep(5);
	}
	}catch(std::exception e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
		is_alive = 0;
	}
}
void Player::sendKick(const std::string& reason){
	try{
		OPacket p;
		p.write(reason);
		p.type = PACKET_KICK;
		sock << p;
		sleep(3);
		is_alive = 0;
	}catch(std::exception e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
		is_alive = 0;
	}
}
void Player::sendChatMessage(const std::string& msg,const std::string& sendBy,int team){

	try{
		OPacket p;
		p.type = PACKET_SEND_CHAT;
		p.write(msg);
		p.write<char>(3);
		p.write<bool>(1);
		p.write(sendBy);
		p.write<int>(team);
		p.write<int>(team);
		sock << p;
	}catch(std::exception e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
	}
}
void Player::sendSystemMessage(const std::string& msg){
	sendChatMessage(msg,"SERVER",5);
}
void Player::sendGameTickCommand(int tick, const int sendBy,std::string arr){
	try{
		OPacket p,p2;
		p.write<int>(tick);
		p.write<int>(1);
		p2.write(arr);
		p.write_Gzip("c",p2);
		p.type = PACKET_TICK;
		sock << p;
	}catch(std::exception e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
	}
}
void Player::sendTick(int tick){
	try{
		OPacket p;
		p.write<int>(tick);
		p.write<int>(0);
		p.type = PACKET_TICK;
		sock << p;
	}catch(std::exception e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
	}
}



void Player::registerConnect(IPacket& ipac){
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<int> distrib(0,89999);
	ipac.read<int>();
	int version = ipac.read<int>();
	ipac.read<int>();

	OPacket p;
	p.write("cn.rwsrs.server");
	p.write<int>(1);
	p.write<int>(version);
	p.write<int>(version);
	p.write("com.corrodinggames.rts");
	p.write("bad8b8ab-335e-475f-9953-7ac311be7f33");
	p.write<int>(10000+distrib(gen));
	p.type = PACKET_REGISTER_CONNECTION;
	sock << p;
}
void Player::handlePacket(PacketType type,const IPacket& p){
	switch(type){
	case PACKET_HEART_BEAT_RESPONSE:{
		heart_beat_times = 0;
		this->receive_time = clock();
		this->ping = static_cast<int>((receive_time-send_time)>=0?(receive_time-send_time):(send_time-receive_time));
		
	}break;
	case PACKET_PREREGISTER_CONNECTION:{
		registerConnect(const_cast<IPacket &>(p));
		sendServerInfo();
	}break;
	case PACKET_PLAYER_INFO:{
		if(getPlayerInfo(const_cast<IPacket &>(p))){
			//sendServerInfo();
		}
	}break;
	case PACKET_ADD_CHAT:{
		std::string s = const_cast<IPacket &>(p).read();
		room.sendChatBroadCast(s,this->name,this->team);
	}break;
	case PACKET_DISCONNECT:{
		is_alive = 0;
	}break;
	}
}

void Player::run(){
	try{

	while(room.is_run&is_alive){
		unsigned char buffer[1024];
		for(auto a : buffer){
			a = '\0';
		}
		try{
			if(sock.is_open())
			sock.receive(boost::asio::buffer(buffer,1024));
		}catch(boost::system::system_error e){
			err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
			is_alive = 0;
			break;
		}
		int a = 0;
		// for(int i = 0;i!=4;++i){//TODO: error
		// 	*((char*)(&a)+3-i) = buffer[i];
		// }
		for(int i = 0;i!=4;++i){
			a |= static_cast<unsigned int>(buffer[3-i]) << 8*i;
		}
		std::string str;
		str.resize(a+4);
		for(int i = 0; i!=a+4;++i){
			str[i]=buffer[i+4];
		}
		IPacket p(str);
		p.size = a+4;
		p.type = (PacketType)p.read<int>();

	//test
		//std::cout << "Receive  type: " << (int)p.type << "\tsize: " << p.size << std::endl;

		handlePacket(p.type,p);
	}

	}catch(std::exception e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
	}
}
