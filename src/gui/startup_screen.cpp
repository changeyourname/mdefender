/*
Name: KSD_GUI_STARTUP_WINDOW

Description: GUI Interface for startup window, 

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

Date: 21 April 2014

Author: Trevor Sibanda

Email: trevorsibb@gmail.com

*/

#ifdef KSD_GUI_STARTUP_WINDOW


#define IDT_TIMER_STARTUP WM_TIMER + 5

bool g_isStartUpWindowActive;

BOOL CALLBACK StartUpProc(HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam) {
switch( msg ){
        case WM_CLOSE:
		//no exit allowed here
		return TRUE;
		break;
		case WM_INITDIALOG:
		{
		 HBITMAP img = LoadBitmap( GetModuleHandle(NULL) , MAKEINTRESOURCE(IDB_BANNER) );
		SendMessage( GetDlgItem( hwnd , IDC_STARTUP_PIC ) , STM_SETIMAGE , IMAGE_BITMAP , (LPARAM)img );
        
		
		}
        return TRUE;
        break;
        case WM_TIMER:
             switch( LOWORD(wParam ) ){
                     case IDT_TIMER_STARTUP:
                          KillTimer( hwnd , IDT_TIMER_STARTUP );
						  ::g_isStartUpWindowActive = false;
						  DestroyWindow( hwnd );
                          break;
                          }
      return TRUE;
       break;                         
                          
}
return FALSE;
}

 
 DWORD WINAPI ThreadStartUpWindow( LPVOID lpParam ){
 int timeout = (int)lpParam;
 MSG msg;
 HWND hwnd = CreateDialog(GetModuleHandle(NULL) , MAKEINTRESOURCE(IDD_START_UP) , NULL , (DLGPROC)StartUpProc );
 
 if( ! hwnd ){
 MessageBox(NULL , "Failed to create startup screen. Very odd" , "Startup screen" , 16);
 ExitProcess(0);
 }
 
 SetTimer( hwnd ,  IDT_TIMER_STARTUP,            
    timeout,    
    (TIMERPROC) NULL);
::g_isStartUpWindowActive = true;	
ShowWindow(hwnd , SW_SHOW);
while( ::g_isStartUpWindowActive && GetMessage(&msg , hwnd  , 0 , 0) > 0){
UpdateWindow( hwnd );
TranslateMessage(&msg);
DispatchMessage(&msg);
}
::g_isStartUpWindowActive = false;
DestroyWindow(hwnd);
return 0;
}


//As soon as an object is instantiated
//a thread is setup and the window is shown
//the window is shown for a period of time defined 
//by timeout
//after which the window is terminated and may not
//be used again  
//like a startup window should be
//BTW its not blocking, so program execution continues
// immediately after
//a call to isActive will tell you if the window is still 
//being displayed
StartUPWindow::StartUPWindow(int timeout ){
DWORD id = 0;
::g_isStartUpWindowActive = true;
 if(  ! CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            ThreadStartUpWindow,       // thread function name
            (void*)timeout ,          // argument to thread function 
            0,                      // use default creation flags 
            &id )   // returns the thread identifier 
	){
	MessageBox(NULL , "Failed to create startup window thread. Very odd" , "Thread startup" , 16);
	ExitProcess(0);
	}


}
	
StartUPWindow::~StartUPWindow(){
::g_isStartUpWindowActive = false;

}
	
bool StartUPWindow::isActive()const{
return ::g_isStartUpWindowActive;
}



#endif

