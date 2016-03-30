/*
Name: ZMCZ

Description: GUI

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


Date: 1 April 2014

Author: Trevor Sibanda

Email: trevorsibb@gmail.com

*/

 
 #ifndef KSD_SYS_GUI_FILE_SELECTOR
 
 #define KSD_SYS_GUI_FILE_SELECTOR
 
   
#include <windows.h>
#include<commctrl.h>
#include <vector>
#include <algorithm>
#include <string>
#include <shlobj.h>
#include <shellapi.h>
 
 namespace mdf{
 
 namespace gui{
 

 
 //Not thread safe
 //select multiple files and or folder and returns as vector
 //returns false if user cancelled selection
 bool  MultipleFileSelect( std::vector<std::string>& refFiles );
 
 //thread safe. browse for one folder and returns it
 //returns empty string on fail
 std::string BrowseFolder(std::string saved_path);
 
 //same as above except for file
 //no multi select (yet)
 std::string BrowseFile( std::string _saved_path);
 

 
 
};
 
 
 
 
 };

#endif


 
