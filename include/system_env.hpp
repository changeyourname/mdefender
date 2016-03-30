/*
Name: KSD_AV_LOGGER

Description: Get and Set system environment variables

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

Date: 13 March 2014

Author: Trevor Sibanda

Email: trevorsibb@gmail.com

*/

 
 #ifndef KSD_SYS_ENVIRONMENT_INFO
 
 #define KSD_SYS_ENVIRONMENT_INFO
 
#include <windows.h> 
#include <string>
#include <direct.h> 
 

 class SystemEnvironment{
 public:
		//Constructor 
		SystemEnvironment();
		
		//Destructor
		~SystemEnvironment();
		
		//Get an environment variable by its string id
		//I.e to get system time the function may be called with
		//argument %time%
		bool Get(  std::string _key , std::string&  );
		
		//Same as above except it returns a std::string
		std::string Get(  std::string _key  );
		
		
		//Set an environment variable
		bool Set( std::string _key ,  std::string _value);
		
		///		Quick Functions ///
		
		//get current user
		std::string GetUserName();
		
		//get computername
		std::string GetComputerName();
		
		//Get the system directory
		std::string GetSystemDirectory();
		
		//get the windows directory
		std::string GetWindowsDirectory();
		
		//get program files
		std::string GetProgramFilesDirectory();
		
		//get executable directory
		//returns  directory of the running program
		std::string GetExecutableDirectory();
		
		//get executable path
		//returns path of current executable
		std::string GetExecutablePath();
		
		//Get working directory
		std::string GetWorkingDirectory();
		
		//get the temp directory
		std::string GetTemporaryDirectory();
		
		//get the system drive
		std::string GetSystemDrive();
		
		//get startup folder
		std::string GetStartUpFolder();
		
 };
 
 
 
#endif 
