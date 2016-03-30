/*
Name: KSD_AV_PROCESS_MANAGER

Description: Manage and terminate processes. 
			
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
			
Date: 15 March 2014

Author: Trevor Sibanda

Email: trevorsibb@gmail.com

*/

 
 #ifndef KSD_AV_PROCESS_MANAGER
 
 #define KSD_AV_PROCESS_MANAGER
 
 #include <vector>
 #include <windows.h>
 #include <string>
 #include <tlhelp32.h>
 
struct Process{
DWORD pid;
std::string exeName;
std::string filename;
std::vector< std::string > loadedModules;
int threadCount;
}; 
 
class ProcessManager{
	public:
	
	//Constructor. 
	//Autoload specifies whether populate should be called by constructor.
	//Default is true.
	ProcessManager();
	
	//Populates an internal structure with a list of all running processes
	bool PopulateAllProcesses();
	
	//Kill a process by its process identifier
	bool KillProcess( DWORD pid);
	
	//Kill a process
	bool KillProcess( Process );
	
	//Killall processes where the process exeName is std::string
	bool KillAllByName( std::string );
	
	//Get information about a process from its pid
	Process GetProcessInfo( DWORD pid);
	
	//List all running processes from the last time PopulateAllProcesses() was called
	std::vector<Process> ListAllProcesses();
	
	
	//List all loaded process modules
	bool ListProcessModules( DWORD dwPID , Process& pRef);
	
	//Check if a module is currently loaded
	bool CheckIsLoadedModule( std::string );

protected:
	std::vector< Process > vRunningProcesses;

	
};

 

 
#endif 
