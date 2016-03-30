

/*
Name: KSD_GUI_SCAN_PROGRESS_WINDOW

Description: GUI Interface for scan progress, Self contained Class

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


Date: 6 April 2014

Author: Trevor Sibanda

Email: trevorsibb@gmail.com

*/

#ifndef KSD_SINGLETONE

#define KSD_SINGLETONE

#include <windows.h>

//@todo In future remove this implementation
//as it can be easily exploited by malware

 class KSingleTone{

 public:
	KSingleTone(){

//exits if other instance is running
    SECURITY_DESCRIPTOR secd;
    if( !InitializeSecurityDescriptor(&secd , SECURITY_DESCRIPTOR_REVISION ) )
    {
//if debug
		MessageBox(NULL , "Failed to create security descriptor. Singletone" , "Sec Descriptor Failed" , 32 );
        ExitProcess(0); //assume no exist
    }
    if(! SetSecurityDescriptorDacl(&secd ,1,0,0 ) )
    {
//if debug

        return ; //assume no exist
    }

    SECURITY_ATTRIBUTES secattr;
    secattr.bInheritHandle = true;
    secattr.lpSecurityDescriptor = &secd;
    secattr.nLength = sizeof(SECURITY_ATTRIBUTES);


//now create mutex
     CreateMutexA(&secattr , 1 , "MDEFENDER" );
    if(GetLastError() != 183){
                      
                      
    return;
    }else{
        MessageBox(NULL , "Only one instance of the program can run at a particular time." , "M-Defender already running" , 16 );
		ExitProcess(0);
    }
    return;
}


};

#endif

