

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
#include"../headers/packet.hpp"
template<typename T>
Packet::PacketData::DataIterator::operator T(){
	//if(index + sizeof(T)>=pData.data.size()){
	if(index + sizeof(T)>pData.data.size()){

		std::cout << index + sizeof(T) << std::endl;
		std::cout << pData.data.size() << std::endl;
		throw std::runtime_error("out of packet");

	}
	T result;
	for(char i = 0;i!= sizeof(T);++i){
		*((char*)(&result)+sizeof(T)-1-i) = pData.data[index+i];
	}
	index += sizeof(T);
	return result;
}
Packet::PacketData::DataIterator Packet::PacketData::operator[](int index){
	return DataIterator(*this,index);
}
template<typename T>
void Packet::PacketData::push_back(const T t){
	for(char i = 0;i!=sizeof(t);++i){
		//TODO:
		data.push_back(*((char*)(&t)+sizeof(t)-1-i));
	}
}
template<typename T>
T IPacket::read(){
	
	T t = static_cast<T>(pData[index]);
	index += sizeof(t);
	return t;
}
std::string IPacket::read(){
	short size = read<short>();
	std::string s;
	for(int i = 0;i!=size;++i){
		s.push_back(read<char>());
	}
	return s;
}
template<typename T>
void OPacket::write(const T t){
	pData.push_back(t);
	index += sizeof(t);
	size += sizeof(t);
}

//自带写入字符串大小
void OPacket::write(const char* str){
	write(std::string(str));
}
//自带写入字符串大小
void OPacket::write(const std::string& str){
	write((short)str.size());
	for(char a : str){
		pData.push_back(a);
		index++;
		size++;
	}
}
void OPacket::write_Gzip(const std::string& key,const OPacket& data){
	this->write(key);
	this->write<int>(data.size);
	std::string enc = gzip::compress(data.pData.data.data(),data.pData.data.size(),Z_DEFAULT_COMPRESSION);
	for(int i = 0;i!= enc.size();++i){
		this->write<char>(enc[i]);
	}

}

void OPacket::write_data(const char* data,size_t size){
	for(int i =0;i!=size;++i){
		this->write<char>(data[i]);
	}
}
void OPacket::write_data(const std::string& data){
	for(auto a : data){
		this->write<char>(a);
	}
}
void operator<<(boost::asio::ip::tcp::socket& sock,const OPacket& p){

	try{	
		OPacket p2;
		p2.write<int>(p.size);
		p2.write<int>(p.type);
		for(int i = 0;i!=p.size;++i){
			p2.write<char> (p.pData.data[i]);
		}
		if(sock.is_open()){
			//std::cout << "Send  type: " << (int)p.type << "\tsize: " << p.size << std::endl;
			sock.send(boost::asio::buffer(p2.pData.data));
		}
		//std::cout << std::endl;
	}catch(boost::system::system_error e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
	}
}