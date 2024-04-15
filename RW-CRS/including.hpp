

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

#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<map>
#include<string>
#include<random>
#include<memory>
#include<thread>
#include<functional>
#include<variant>
#include<unistd.h>
#include<boost/asio.hpp>


#include"dependency/cpp_httpapi/httplib.h"
#include"dependency/gzip/compress.hpp"

#include"headers/programVar.hpp"
#include"headers/md5.h"
#include"headers/open.hpp"
#include"headers/packet.hpp"
#include"headers/room.hpp"
#include"headers/server.hpp"
#include"headers/player.hpp"
#include"headers/command.hpp"


#include"definitions/md5.cpp"
#include"definitions/packet.cpp"
#include"definitions/player.cpp"
#include"definitions/room.cpp"
#include"definitions/server.cpp"
#include"definitions/open.cpp"