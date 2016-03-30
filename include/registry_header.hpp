/*
Name: KSD_AV_REGISTRY_READER

Description: Reads and writes to registry

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


Date: 21 March 2014

Author: Trevor Sibanda

Email: trevorsibb@gmail.com

*/

#ifndef KSD_AV_REGISTRY_READER

#include <string>
#include <vector>

#define KSD_AV_REGISTRY_READER


//Include scan object 
//Class will store each scan object
#ifndef KSD_AV_SCAN_OBJ

#include "scan_object.hpp"

#endif 


 class RegistryReader{
	public:
	
	//Read a key from registry and return value
	std::string GetKey(std::string , std::string );
	
	//Write key to registry and return operation status
	bool WriteKey(std::string _hk , std::string _key, std::string _value);
	
	//Enumerate all startup applications for system
	std::vector<ScanObject> GetStartupPrograms(bool _machine );
	
 
 };
 
 
 #endif
 