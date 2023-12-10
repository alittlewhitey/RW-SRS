

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

#include"player.hpp"
#include"room.hpp"
#include"programVar.hpp"
#include<sstream>
#include<map>
#ifndef SRS_COMMAND
#define SRS_COMMAND

std::map<std::string,int> commands{
    {"afk",1},
    {"t",2}
};

void afk_thread(Player& sendAdmin){
try{
    if(sendAdmin.room.isAfking){
        sendAdmin.sendSystemMessage("正在进行AFK");
        return;
    }
    sendAdmin.room.isAfking = 1;
    sendAdmin.room.sendSystemBroadCast("10秒后转移管理员，输入break命令以阻止");
    while(sendAdmin.room.afkTime <= 10){
        sleep(1);
        ++sendAdmin.room.afkTime;
    }
    if(sendAdmin.room.afkTime == 10&&sendAdmin.room.isAfking){
        sendAdmin.isAdmin = 0;
        srand(time(0));
        sendAdmin.room.players[rand()%sendAdmin.room.players.size()]->isAdmin = 1;
        sendAdmin.room.sendSystemBroadCast("管理员已转移");
        sendAdmin.room.afkTime = 0;
        sendAdmin.room.isAfking = 0;
    }else if(!sendAdmin.room.isAfking){
        sendAdmin.room.afkTime = 0;
    }
}catch(std::exception e){
    std::cerr << e.what() << std::endl;
}
}
void handleClientCommand(std::string command,Player& player){
    std::istringstream iss(command);
    std::string temp;
    iss >> temp;
    std::vector<std::string> args{temp};
    while(iss){
        iss >> temp;
        args.push_back(temp);
        switch(commands[args[0]]){
        case 1:{
            std::jthread t1([&player](){afk_thread(player);});
            t1.detach();
        }break;
        case 2:{
            std::string tempStr = "";
            for(int i = 1;i != args.size();++i){
                tempStr += args[i];
                tempStr += ' ';
            }
            for(auto& p : player.room.players){
                
                if(p->team == player.team){

                    p->sendSystemMessage(tempStr);
                }
            }
        }break;
        }
        
    }

    try{

    }catch(std::exception e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
        player.sendSystemMessage("处理命令时发生异常，已停止处理");
    }
}

#endif //!SRS_COMMAND