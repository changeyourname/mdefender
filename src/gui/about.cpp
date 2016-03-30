/*
Name: KSD_GUI_ABOUT

Description: about window

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

#ifdef KSD_GUI_ABOUT



bool g_isAboutWindowActive;

BOOL CALLBACK AboutWindowProc(HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam) {
switch( msg ){
        case WM_CLOSE:
		PostQuitMessage(0);
		return TRUE;
		break;
		case WM_INITDIALOG:
		{
		 HBITMAP img = LoadBitmap( GetModuleHandle(NULL) , MAKEINTRESOURCE(IDB_ABOUT) );
         SendMessage( GetDlgItem( hwnd , IDC_ABOUT_PIC ) , STM_SETIMAGE , IMAGE_BITMAP , (LPARAM)img );
         
	
		}
		return TRUE;
		break;
		case WM_COMMAND:
        switch (LOWORD(wParam))
        {
		case IDC_LICENSE:
		MessageBox(hwnd , MDF_LICENSE , "Mthwakazi Defender | License " , 64 );
		break;
		case IDC_WEB:
		break;
		case IDC_DONATE:
		MessageBox(hwnd , MDF_DONATE , "Mthwakazi Defender | Donations " , 64 );
		
		
		}
		break;
		return true;	
                          
}
return false;
}

 
 DWORD WINAPI ThreadAboutWindow( LPVOID lpParam ){
 MSG msg;

 HWND hwnd = CreateDialog(GetModuleHandle(NULL) , MAKEINTRESOURCE(IDD_ABOUT) , NULL , (DLGPROC)AboutWindowProc );
 
 if( ! hwnd ){
 MessageBox(NULL , "Failed to create startup screen. Very odd" , "Startup screen" , 16);
 ExitProcess(0);
 }
 
ShowWindow( hwnd , SW_SHOW ); 
::g_isAboutWindowActive = true;	
while( ::g_isAboutWindowActive && GetMessage(&msg , hwnd  , 0 , 0) > 0){
UpdateWindow( hwnd );
TranslateMessage(&msg);
DispatchMessage(&msg);
}
::g_isAboutWindowActive = false;
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
AboutWindow::AboutWindow( ){
DWORD id = 0;
::g_isAboutWindowActive = true;
 if(  ! CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            ThreadAboutWindow,       // thread function name
            NULL ,          // argument to thread function 
            0,                      // use default creation flags 
            &id )   // returns the thread identifier 
	){
	MessageBox(NULL , "Failed to create about window thread. Very odd" , "Thread startup" , 16);
	}


}
	
AboutWindow::~AboutWindow(){
::g_isAboutWindowActive = false;

}
	
bool AboutWindow::IsActive()const{
return ::g_isAboutWindowActive;
}



#endif

