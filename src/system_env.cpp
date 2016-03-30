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

 
 #ifdef KSD_SYS_ENVIRONMENT_INFO


 
#ifndef MAX_PATH
	#define MAX_PATH 1024 
#endif

	
		//Constructor 
		SystemEnvironment::SystemEnvironment(){}
		
		//Destructor
		SystemEnvironment::~SystemEnvironment(){
		
		}
		
		//Get an environment variable by its string id
		//I.e to get system time the function may be called with
		//argument %time%
		bool SystemEnvironment::Get(  std::string _key , std::string& refString  ){
		char* buffer = new char[1024];
	    ::GetEnvironmentVariable(   _key.c_str()  , buffer , 1024 ); 
		refString = std::string( buffer );
		delete[] buffer;
		return true;
}
		
		//Same as above except it returns a std::string
		std::string SystemEnvironment::Get(  std::string _key  ){
		std::string buffer;
		this->Get( _key , buffer );
		return buffer;
}
		
		
		//Set an environment variable
		bool SystemEnvironment::Set( std::string _key ,  std::string _value){
		return ::SetEnvironmentVariable( _key.c_str()  , _value.c_str() );
}
		
		///		Quick Functions ///
		
		//get current user
	std::string SystemEnvironment::GetUserName(){
	DWORD dwSize=0;
    char* buffer = new char[MAX_PATH];
	::GetUserName( buffer , &dwSize );
	std::string temp( buffer );
	delete[] buffer;
	return temp;	
}
		
		//get computername
	std::string SystemEnvironment::GetComputerName(){
	char* buffer = new char[MAX_PATH];
	DWORD dwSize;
	::GetComputerName( buffer , &dwSize );
	std::string temp( buffer );
	delete[] buffer;
	return temp ;	
}
		
		//Get the system directory
	std::string SystemEnvironment::GetSystemDirectory(){
char* buffer = new char[ MAX_PATH ];
	::GetSystemDirectory( buffer , MAX_PATH );
	std::string temp( buffer );
	delete[] buffer;
	return temp + "\\";	
}
		
		//get the windows directory
		std::string SystemEnvironment::GetWindowsDirectory(){
	char* buffer = new char[MAX_PATH];
	::GetWindowsDirectory( buffer , MAX_PATH );
	std::string temp( buffer );
	delete[] buffer;
	return temp + "\\";	
}
		
		//get program files
		std::string SystemEnvironment::GetProgramFilesDirectory(){
	return this->GetSystemDrive() + "\\Program Files\\";
}
		
		//get executable directory
		//returns  directory of the running program
		std::string SystemEnvironment::GetExecutableDirectory(){
		std::string exePath = this->GetExecutablePath();
		int npos = exePath.find_last_of('\\');
		if( npos <= 0 )return exePath;
		return std::string(  exePath  , 0 , npos +1  ); //get last portion of string //
}
		
		//get executable path
		//returns path of current executable
		std::string SystemEnvironment::GetExecutablePath(){
HMODULE hModule = ::GetModuleHandle(NULL);
CHAR path[MAX_PATH];
::GetModuleFileName(hModule, path, MAX_PATH);
return std::string( path );
}
		
		//Get working directory
		std::string SystemEnvironment::GetWorkingDirectory(){
char *path=NULL;
size_t size;
path= _getcwd(path,size);
std::string tmp( path );
if( path != NULL )
delete[] path; 
return tmp + "\\";
}
		
		//get the temp directory
	std::string SystemEnvironment::GetTemporaryDirectory(){
	char* buffer = new char[MAX_PATH];
	::GetTempPath( MAX_PATH , buffer );
	std::string temp( buffer);
	delete[] buffer;
	return temp + "\\";	
}
		
		//get the system drive
		std::string SystemEnvironment::GetSystemDrive(){
	std::string path = this->GetSystemDirectory( );
	int npos = path.find_first_of('\\');
	if( npos <= 0 )return path; //failed
	return std::string( path , 0 , npos );
}
		
		//get startup folder
		std::string SystemEnvironment::GetStartUpFolder(){
	//@TODO make this work. better
	return this->Get("USERPROFILE" ) + "\\Start Menu\\Programs\\Startup\\";
}
		
#endif 
