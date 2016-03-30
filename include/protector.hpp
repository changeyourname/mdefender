/*
Name:  Mthwakazi Defender Protector

Description: Protects Mthwakazi defender from debuggers and checks if 
			 the program has been altered at startup.

	Copyright (C) 2014  Trevor Sibanda, Clifford J Mthemba, ZimCODERZ

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

Author: Trevor Sibanda
	
Date: 	27 April 2014
*/

#include <windows.h>
#include <string>

#ifndef KSD_MDEF_PROTECTOR


class MdefProtector{
public:
	~MdefProtector();
	
	MdefProtector(std::string programDir  , std::string programHash );
	
	bool Start();
	
	bool Kill();

private:
	std::string filename;
	std::string filePath;
	std::string programHash;
	std::string checkHash;


};

#endif

		 