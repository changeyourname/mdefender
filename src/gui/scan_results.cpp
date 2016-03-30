/*
Name: KSD_GUI_SCAN_RESULTS_WINDOW

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

 
 #ifdef KSD_GUI_SCAN_RESULTS
  
 
 //we must have the detected scanObjects reference
  #ifndef KSD_AV_SCAN_OBJ
 
//now we really need it
#include "include/scan_object.hpp" 
   
  #endif
  
 //Actions to be carried out on detected files
 #define KSD_DELETE_DETECTED 					0
 #define KSD_QUARANTINE_DETECTED 			1	//not yet implemented
 #define KSD_REPAIR_DETECTED					2 //NOT YET IMPLEMENTED
 #define KSD_IGNORE_DETECTED					3
 
namespace mdf{

namespace gui{ 
 
BOOL CALLBACK ScanResultsProc(HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam);


char* g_ComboActions[] = { "Delete All" , "Do Nothing"  };
#define COMBO_ACTIONS_SIZE            2  
 
bool ScanResultsWindow(HWND _parent ,  std::vector<SROB>& refSROB ,  int& actionToTake ){
mdf::gui::scanResults = refSROB;

if( _parent != NULL )
ShowWindow(_parent , SW_HIDE );

HWND hwnd = CreateDialog(GetModuleHandle(NULL) , MAKEINTRESOURCE(IDD_SCAN_RESULTS) , NULL , (DLGPROC)ScanResultsProc );
if( ! hwnd){
MessageBox(NULL , "Failed to create results window !!!\nProgram will now exit" , "Fatal Exit" , 16 );
FatalExit(0);
}

ShowWindow( hwnd , SW_SHOW );
MSG umsg;
while(  GetMessage(&umsg , NULL  , 0 , 0) > 0){
UpdateWindow(  hwnd );
TranslateMessage(&umsg);
DispatchMessage(&umsg);
}

DestroyWindow( hwnd);

actionToTake = mdf::gui::actionToApply;

return bool( mdf::gui::actionToApply );
}
  

 BOOL CALLBACK ScanResultsProc(HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam) {

    switch(msg) {
    case WM_INITDIALOG: 
{
    HBITMAP header = LoadBitmap( GetModuleHandle(NULL) , MAKEINTRESOURCE(IDB_BANNER) );
    
    SendMessage( GetDlgItem( hwnd , IDC_SR_PIC ) , STM_SETIMAGE , IMAGE_BITMAP , (LPARAM)header );
             
                    
  
    for(int x = 0; x < COMBO_ACTIONS_SIZE ; x++ ){
    SendMessage( (HWND)GetDlgItem( hwnd , IDC_COMBO_ACTION )  ,
                       (UINT) CB_ADDSTRING, 
                       (WPARAM) wParam,  
                       (LPARAM)mdf::gui::g_ComboActions[x] );
    }
   //set default action
    SendMessage( GetDlgItem( hwnd , IDC_COMBO_ACTION), CB_SETCURSEL, 0, 0);
    char* msg = new char[4096];
    sprintf(msg , "%d Detections" , mdf::gui::scanResults.size() );
    SendMessage( GetDlgItem( hwnd , IDC_SCAN_RESULTS_LIST_SELECTED_NAME ) , WM_SETTEXT , wParam , (LPARAM)(msg) );
    //start to display results
    {
                HWND hwndList = GetDlgItem(hwnd, IDC_SCAN_RESULTS_LIST_DETECTIONS);  
            int size = ( mdf::gui::scanResults.size()  );
            for (int i = 0; i < size; i++) 
            { 
                
                std::string sel = std::string( "[" + mdf::gui::scanResults[i].malwareName + "]     " + mdf::gui::scanResults[i].filePath)
                ;     
                int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0, 
                    (LPARAM)( sel.c_str() )  );
                // Set the array index of the player as item data.
                // This enables us to retrieve the item from the array
                // even after the items are sorted by the list box.
                SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM) i); 
            } 
    
    }
    
}
    return TRUE;
    break;
    case WM_CLOSE:
        {
         if( MessageBox( hwnd , "Are you sure you want to exit ?\r\nALL DETECTIONS WILL BE IGNORED AND MAY INFECT YOUR COMPUTER ! ", "WARNING !!!" ,  4 | 16 ) == 6 ){          
         mdf::gui::actionToApply = -1;
         PostQuitMessage(0);
         }
         
        }
        return true;
        break;
    case WM_COMMAND:
        
        switch (LOWORD(wParam))
        {
        
        case IDC_SCAN_RESULTS_APPLY_ACTION:
           {
           char* msg = new char[1024];
           int i = (UINT)SendMessage( GetDlgItem( hwnd , IDC_COMBO_ACTION)  , CB_GETCURSEL , 0 , 0 );
           if( std::string(mdf::gui::g_ComboActions[i]) == "Delete All" ){
               mdf::gui::actionToApply = 0; //delete all
           }else if(std::string(mdf::gui::g_ComboActions[i]) == "Do Nothing" ){
           mdf::gui::actionToApply = 1; //do nothing   
           }else{
           MessageBox( hwnd , "Sorry the selected feature is not yet active as of this version. \nPlease select a different action" , "Feature Unavailable" , 64 );
           return true;      
           }      
           if( i > COMBO_ACTIONS_SIZE && i < 0 )return true;//invalid selection  
           sprintf( msg , "Are you sure you wish to %s all the listed files ?" , mdf::gui::g_ComboActions[i] );
                                           
           if( MessageBox( hwnd , msg , "Confirm Action" , 4 | 128 ) == 6 ){
           //get current combo selection
               
           PostQuitMessage(0);    
           
           }
           delete[] msg;
           return true;    
           } 
        break;
        case IDC_SCAN_RESULTS_CANCEL_APPLY:            
        if( MessageBox( hwnd , "Are you sure you want to exit ?\r\nALL DETECTIONS WILL BE IGNORED AND MAY INFECT YOUR COMPUTER ! ", "WARNING !!!" ,  4 | 16 ) == 6 ){          
         mdf::gui::actionToApply = -1; //nothing or cancelled
         PostQuitMessage(0);
         }
        
        break;
             
        case IDC_SCAN_RESULTS_LIST_DETECTIONS:
            {
                switch (HIWORD(wParam)) 
                { 
                case LBN_SELCHANGE:
                    {
                        HWND hwndList = GetDlgItem(hwnd, IDC_SCAN_RESULTS_LIST_DETECTIONS); 

                        // Get selected index.
                        int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0); 

                        // Get item data.
                        int i = (int)SendMessage(hwndList, LB_GETITEMDATA, lbItem, 0);

                        //print out info about currently selected item
                        {
                        SROB tmp = mdf::gui::scanResults[i];        
                        char messg[4096];
                        sprintf( messg , "%s\r\n The file has malicious traits and should be treated" , tmp.filePath.c_str() );
                         
                        SendMessage( GetDlgItem( hwnd , IDC_SCAN_RESULTS_LIST_SELECTED_NAME ) , WM_SETTEXT , 0 , (LPARAM)(tmp.malwareName.c_str() ) );
                        SendMessage( GetDlgItem( hwnd , IDC_SCAN_RESULTS_LIST_SELECTED_DESCRIPTION ) , WM_SETTEXT , 0 , (LPARAM)(messg ) );
                                 
                        }                                 
                        return TRUE; 
                    } 
                }
            }      
        }       
        
        return TRUE;
        break;
       case WM_CTLCOLORSTATIC:
        // Set the colour of the text for our URL
        if ((HWND)lParam == GetDlgItem(hwnd, IDC_SCAN_RESULTS_LIST_SELECTED_NAME)) 
        {
                // we're about to draw the static
                // set the text colour in (HDC)lParam
                SetBkMode((HDC)wParam,TRANSPARENT);
                SetTextColor((HDC)wParam, RGB(255,0,0));
                return (BOOL)CreateSolidBrush (GetSysColor(COLOR_MENU));
        }
        
        break;    
        
    }
    return false;
}
 
 	
	
}
	
}

#endif 
