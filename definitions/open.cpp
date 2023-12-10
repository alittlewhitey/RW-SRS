

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
#include"../headers/open.hpp"
void openRoom(Room& room){
    std::string token = generateStr(40);
    std::string uuid = generate_uuid();


    httplib::Client cli1("http://gs4.corrodinggames.net");
    httplib::Client cli2("http://gs1.corrodinggames.com");
    httplib::Params params;
    params.emplace("action","add");
    params.emplace("user_id","u_"+uuid);
    params.emplace("game_name","RW_SRS");
    params.emplace("game_version",std::to_string(room.game_version));
    params.emplace("game_version_string","1.15");
    params.emplace("private_token",token);
    params.emplace("private_token_2",b(b(token)));
    params.emplace("confirm",b("a"+ b(token)));
    params.emplace("password_required","false");
    params.emplace("created_by",room.created_by);
    params.emplace("private_ip","192.168.0.100");//TODO
    params.emplace("port_number",std::to_string(room.port));
    params.emplace("game_map",room.mapName);
    params.emplace("game_mode",room.game_mode);
    params.emplace("game_status",room.state);
    params.emplace("player_count",std::to_string(room.players.size()));
    params.emplace("max_player_count",std::to_string(room.maxPlayer));
    auto a1 = cli1.Post("/masterserver/1.4/interface",params);
    auto a2 = cli2.Post("/masterserver/1.4/interface",params);
    if(a1.error() == httplib::Error::Success || a2.error() == httplib::Error::Success){
        std::cout << "uplist success" << std::endl;
        std::jthread t([uuid,token,&room](){
            while(room.is_open){
                updateOpenRoom(uuid,token,room);
                sleep(15);
            }
        });
        t.detach();
    }else{
        std::cout << "uplist error" << std::endl;
		err << __FILE__ << '\t' << __LINE__ << '\t' << "uplist error:"<<  a1.error() << std::endl;
    }
    
}
void updateOpenRoom(std::string uuid, std::string token, Room& room){
    if(room.is_gaming){
        room.state = "ingame";
    }else{
        room.state = "battleroom";
    }
    httplib::Client cli1("http://gs4.corrodinggames.net");
    httplib::Client cli2("http://gs1.corrodinggames.com");
    httplib::Params params;
    params.emplace("action","update");
    params.emplace("id","u_"+uuid);
    params.emplace("private_token",token);
    params.emplace("check_port","false");
    params.emplace("user_id","u_"+uuid);
    params.emplace("game_name","SERVER");
    params.emplace("game_version",std::to_string(room.game_version));
    params.emplace("game_version_string","1.15");
    params.emplace("private_token",token);
    params.emplace("private_token_2",b(b(token)));
    params.emplace("confirm",b("a"+ b(token)));
    params.emplace("password_required","false");
    params.emplace("created_by",room.created_by);
    params.emplace("private_ip","192.168.0.100");//TODO
    params.emplace("port_number",std::to_string(room.port));
    params.emplace("game_map",room.mapName);
    params.emplace("game_mode",room.game_mode);
    params.emplace("game_status",room.state);
    params.emplace("player_count",std::to_string(room.players.size()));
    params.emplace("max_player_count",std::to_string(room.maxPlayer));
    auto a1 = cli1.Post("/masterserver/1.4/interface",params);
    auto a2 = cli2.Post("/masterserver/1.4/interface",params);
    if(a1.error() != httplib::Error::Success && a2.error() != httplib::Error::Success){
        std::cout << "uplist error" << std::endl;
		err << __FILE__ << '\t' << __LINE__ << '\t' << "uplist error:"<<  a1.error() << std::endl;
    }
}