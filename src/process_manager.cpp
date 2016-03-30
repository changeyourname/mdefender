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

 
 #ifdef KSD_AV_PROCESS_MANAGER


	ProcessManager::ProcessManager():
	vRunningProcesses(0)

	{
this->PopulateAllProcesses();
	
	}
	
	//Populates an internal structure with a list of all running processes
	bool ProcessManager::PopulateAllProcesses(){
  HANDLE hProcessSnap;
  HANDLE hProcess;
  PROCESSENTRY32 pe32;
  DWORD dwPriorityClass;

  // Take a snapshot of all processes in the system.
  hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
  if( hProcessSnap == INVALID_HANDLE_VALUE )
  {
    return( FALSE );
  }

  // Set the size of the structure before using it.
  pe32.dwSize = sizeof( PROCESSENTRY32 );

  // Retrieve information about the first process,
  // and exit if unsuccessful
  if( !Process32First( hProcessSnap, &pe32 ) )
  {
    CloseHandle( hProcessSnap );          // clean the snapshot object
    return( FALSE );
  }

  // Now walk the snapshot of processes, and
  // display information about each process in turn
  do
  {
  Process tmp;
  tmp.exeName = std::string( pe32.szExeFile );
    hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
    if( hProcess == NULL )
	continue;
 

    tmp.pid  = pe32.th32ProcessID ;
    tmp.threadCount =   pe32.cntThreads ;
    
    // List the modules and threads associated with this process
    this->ListProcessModules( pe32.th32ProcessID , tmp);

	//now add
	this->vRunningProcesses.push_back(tmp);
	
  } while( Process32Next( hProcessSnap, &pe32 ) );

  CloseHandle( hProcessSnap );
  return( TRUE );
	
	}
	
	
//Get all loaded modules for process	
bool ProcessManager::ListProcessModules( DWORD dwPID , Process& pRef)
{
  HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
  MODULEENTRY32 me32;

  // Take a snapshot of all modules in the specified process.
  hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
  if( hModuleSnap == INVALID_HANDLE_VALUE )
  {
    return false;
  }

  // Set the size of the structure before using it.
  me32.dwSize = sizeof( MODULEENTRY32 );

  // Retrieve information about the first module,
  if( !Module32First( hModuleSnap, &me32 ) )
  {
  
    CloseHandle( hModuleSnap );           // clean the snapshot object
    return false;
  }

  // Now walk the module list of the process,
  // and display information about each module
  do
  {
   //add to vector 
    pRef.loadedModules.push_back(    me32.szExePath );

  } while( Module32Next( hModuleSnap, &me32 ) );

  CloseHandle( hModuleSnap );
  return true;
}

	
	//Kill a process by its process identifier
	bool ProcessManager::KillProcess( DWORD pid){
	HANDLE hProcess = ::OpenProcess( PROCESS_ALL_ACCESS, FALSE, pid );
    if( hProcess == NULL )
	return false; //failed to open
	
	return ::TerminateProcess(hProcess  , 9);
	//exit code  //forced
	}
	
	//Kill a process
	bool ProcessManager::KillProcess( Process  tmp){
	//shud hve made this pass by reference
	return this->KillProcess( tmp.pid );
	
	}
	
	//Killall processes where the process exeName is std::string
	bool ProcessManager::KillAllByName( std::string filename ){

    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof (pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if ( std::string(pEntry.szExeFile) ==  filename)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                                          (DWORD) pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}
	
	
	
	//Get information about a process from its pid
	Process ProcessManager::GetProcessInfo( DWORD dwPid){
	Process tmp;
	PROCESSENTRY32 pEntry;
	HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwPid );
    if( hProcess == NULL )
	return tmp;
    tmp.pid  = pEntry.th32ProcessID ;
    tmp.threadCount =   pEntry.cntThreads ;
	this->ListProcessModules(  dwPid , tmp );
	return tmp;
	}
	
	//List all running processes from the last time PopulateAllProcesses() was called
	std::vector<Process> ProcessManager::ListAllProcesses(){
	return this->vRunningProcesses;
	}
	
	//Check if a module is currently loaded
	bool ProcessManager::CheckIsLoadedModule( std::string  _filename){
	//iterate through payment processes
	int st = this->vRunningProcesses.size() ;
	for(int x = 0;	  x < st 	;	 x++){
	//now check thru loaded modules
	int mst = 0;
	for(int y = 0; y < mst; y++)
	if(  this->vRunningProcesses[x].loadedModules[y] == _filename	)
	return true;
	
	}
	return false;
	}

#endif 
