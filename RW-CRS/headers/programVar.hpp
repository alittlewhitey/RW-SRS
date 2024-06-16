

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


#ifndef SRS_PROGRAMVAR
#define SRS_PROGRAMVAR
#include<iostream>
#include<fstream>
#include<boost/asio.hpp>

bool program_is_run = 1;
std::ofstream err("errors.log");
boost::asio::io_context ioc;
constexpr int v1_3 = 136;
constexpr int v1_4 = 151;
constexpr int v1_5 = 176;

int initUnit = 1;
int initMist = 2;


#endif // !SRS_PROGRAMVAR