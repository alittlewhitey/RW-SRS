

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

#ifndef SRS_OPEN
#define SRS_OPEN

#include<string>
#include<boost/asio.hpp>
#include<random>
#include<iostream>
#include"programVar.hpp"
#include"md5.h"


//const std::string USER_AGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.132 Safari/537.36";

std::string private_ipv4 = "127.0.0.1";


bool prepare_local_ipv4(){
    
    try{
        boost::asio::ip::tcp::resolver res(ioc);
        boost::asio::ip::tcp::resolver::query que(boost::asio::ip::host_name(),"");
        boost::asio::ip::tcp::resolver::iterator it = res.resolve(que);
        std::cout << (private_ipv4 = (it)->endpoint().address().to_string()) << std::endl;
    }catch(std::exception e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
        return 0;
	}
    return 1;
}

std::string generateStr(int size){
    static const std::string allChar = "0123456789abcdefghijklmnopqrstuvwxyz";
    std::string s;
    srand(time(0));
    for(int i = 0;i != size;++i){
        s.push_back(allChar[rand()%36]);
    }
    return s;
}
std::string generate_uuid()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 15);

    const char* uuid_chars = "0123456789abcdef";

    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        int nibble = distrib(gen);
        char c = uuid_chars[nibble];
        if (i == 8 || i == 12 || i == 16 || i == 20) {
            ss << "-";
        }
        ss << c;
    }
    return ss.str();
}

std::string b(std::string str) {
    std::string digest;
    digest = md5(str);
    std::string s;
    //s.resize(digest.size()*2);
    for(auto a : digest){
        int b = a&0xFF;
        if(b < 16){
            s.push_back('0');
        }
        s.push_back(a);
    }
    return s;
}
struct Room;
void openRoom(Room& room);
void updateOpenRoom(std::string uuid, std::string token, Room& room);

#endif //!SRS_OPEN
