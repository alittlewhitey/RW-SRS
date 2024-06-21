

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

#ifndef SRS_COMMAND
#define SRS_COMMAND

#include"../including.hpp"

std::map<std::string,int> commands{
    {"afk",1},
    {"t",2},
    {"players",3},
    {"help",4},
    {"group",5}
};
std::map<std::string,int> adminCommands{
    {"kick",1001},
    {"break",1002},
    {"help",1003}
};

std::map<std::string,std::string> helpStrings = {
    {"afk","尝试更换管理员，可被管理员阻止"},
    {"t <msg>","对内消息"},
    {"players","唤起一个包含所有成员信息的弹窗"},
    {"help","你正在看的东东"},
    {"group","创建/进入小组"}
};
std::map<std::string,std::string> adminHelpStrings = {
    {"kick <ID>","踢出指定ID的用户，通过players指令获得ID"},
    {"break","阻止AFK"},
    {"help","你正在看的东东"},
    {"groups","唤起一个包含所有组信息的弹窗"}
};

void afk_thread(Player& sendAdmin){
try{
    if(sendAdmin.room.isAfking){
        sendAdmin.sendSystemMessage("正在进行AFK");
        return;
    }
    sendAdmin.room.isAfking = 1;
    sendAdmin.room.sendSystemBroadCast("120秒后转移管理员，管理员输入break命令以阻止");
    while(sendAdmin.room.afkTime < 120){
        sleep(1);
        ++sendAdmin.room.afkTime;
    }
    if(sendAdmin.room.afkTime == 120&&sendAdmin.room.isAfking){
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
void handleClientCommand(std::string commandLine,Player& player){
    std::istringstream iss(commandLine);
    std::string command,temp;
    iss >> command;
    std::vector<std::string> args{};
    while(iss){
        iss >> temp;
        args.push_back(temp);

        bool noneCmd = 0;

        switch(commands[command]){
        case 1:{
            std::jthread t1([&player](){afk_thread(player);});
            t1.detach();
        }break;
        case 2:{
            std::string tempStr = "[Team] ";
            auto size = args.size();
            for(int i = 0;i != size;++i){
                tempStr += args[i];
                tempStr += ' ';
            }
            for(auto& p : player.room.players){
                
                if(p->team == player.team){

                    p->sendChatMessage(tempStr,player.name,player.team);
                }
            }
        }break;
        case 3:{
            std::string s = "";
            for(auto& p : player.room.players){
                s += "ID: "+std::to_string(p->ID) + "   " + p->name + '\n';
            }
            player.sendPopUp(s,[](std::string){return 0;});
        }break;
        case 4:{
            player.sendSystemMessage("任何命令前均需加点 '.'");
            std::string s = "命令：\n";
            for(auto a : helpStrings){
                s += ' '+a.first+": "+a.second+'\n';
            }
            player.sendSystemMessage(s);
        }break;
        default:{
            noneCmd = 1;
        }break;
        }
        if(player.isAdmin){
            noneCmd = 0;
            switch(adminCommands[command]){
            case 1001:{
                if(args.size() != 1){
                    player.sendSystemMessage("Error Command.   错误的命令。");
                    player.sendSystemMessage("Usage: .kick <ID>");
                }
                Player_ptr p = player.room.find(std::stoi(args[0]));
                if(p)
                    p->sendKick("被管理员踢出");
                else
                    player.sendSystemMessage("无此ID");
            }break;
            case 1002:{
                if(args.size() != 0){
                    player.sendSystemMessage("Error Command.   错误的命令。");
                    player.sendSystemMessage("Usage: .break ");
                }
                if(player.room.isAfking){
                    player.room.isAfking = 0;
                    player.room.sendSystemBroadCast("AFK已取消，取消者："+player.name);
                }else{
                    player.sendSystemMessage("当前并无AFK");
                }
            }break;
            case 1003:{
                std::string s = "Admin命令：\n";
                for(auto a : adminHelpStrings){
                    s += ' '+a.first+": "+a.second+'\n';
                }
                player.sendSystemMessage(s);
            }break;
            default:{
                noneCmd = 1;
            }break;
            }
        }
        if(noneCmd){
            player.sendSystemMessage(args[0] + ": 未找到命令，请通过 .help 查看可用的命令");
        }
            
    }

    try{

    }catch(std::exception e){
		err << __FILE__ << '\t' << __LINE__ << '\t' << e.what() << std::endl;
        player.sendSystemMessage("处理命令时发生异常，已停止处理");
    }
}

#endif //!SRS_COMMAND