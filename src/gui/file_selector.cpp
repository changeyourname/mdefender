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

 
 #ifdef KSD_SYS_GUI_FILE_SELECTOR
 

 
 namespace mdf{
 
 namespace gui{

bool g_isMultiSelectAdd = false;
std::vector<std::string> g_vAddedFiles; 

#define WM_MYMESSAGE 678

void ClearBrowseFileBuffer(){

mdf::gui::g_vAddedFiles.clear();

}

void SetDefaultPaths( std::vector<std::string> _paths ){
mdf::gui::g_vAddedFiles.clear();
mdf::gui::g_vAddedFiles = _paths;
}

static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg, LPARAM lParam, LPARAM lpData)
{

    if(uMsg == BFFM_INITIALIZED)
    {
        std::string tmp = (const char *) lpData;
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }

    return 0;
}


std::string BrowseFile( std::string start_path ){           
  OPENFILENAME ofn;
    char szFileName[MAX_PATH]="";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lpstrTitle = "Select File To Scan";
    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
    ofn.hwndOwner = GetActiveWindow();
    ofn.nFileOffset=1;
    ofn.Flags=OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
    ofn.lpstrFile = (LPSTR)szFileName;  
    ofn.nMaxFile = MAX_PATH;
    //@todo Add support for multiple file selection
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY  ;
    ofn.lpstrDefExt = "";

    if(GetOpenFileName(&ofn))
    {
    return std::string( ofn.lpstrFile );
    }
 return "";           
}            

std::string BrowseFolder(std::string saved_path)
{
    TCHAR path[MAX_PATH];

    const char * path_param = saved_path.c_str();

    BROWSEINFO bi = { 0 };
    bi.lpszTitle  = ("Select Folder to Scan...");
    bi.ulFlags    = BIF_RETURNONLYFSDIRS ;
    bi.lpfn       = BrowseCallbackProc;
    bi.lParam     = (LPARAM) path_param;

    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );

    if ( pidl != 0 )
    {
        //get the name of the folder and put it in path
        SHGetPathFromIDList ( pidl, path );

        //free memory used
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( pidl );
            imalloc->Release ( );
        }

        return path;
    }

    return "";
}




 


INT_PTR CALLBACK AddFilesDialogProc(HWND hDlg, UINT message, 
        WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
       
        {
          
            // Add items to list. 
            HWND hwndList = GetDlgItem(hDlg, IDC_LIST_FILES);  
            int size = ( mdf::gui::g_vAddedFiles.size()  );
            for (int i = 0; i < size; i++) 
            { 
                
                std::string sel = mdf::gui::g_vAddedFiles[i];     
                int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0, 
                    (LPARAM)( sel.c_str() )  );
                // Set the array index of the player as item data.
                // This enables us to retrieve the item from the array
                // even after the items are sorted by the list box.
                SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM) i); 
            } 
            // Set input focus to the list box.
            SetFocus(hwndList); 
            return TRUE;               
        } 
        
       break;        
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
     
        case IDC_CANCEL:
            mdf::gui::g_isMultiSelectAdd = false;
			EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
            break;
		case IDC_CONTINUE:
			{
			if(  ! mdf::gui::g_vAddedFiles.size() ){
			MessageBox(hDlg , "You have not added any files.\r\nAdd files or  folders.\nUse the Cancel button otherwise." , "Nothing selected" , 64);
			break;
			}
			mdf::gui::g_isMultiSelectAdd = true;
			EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
            }
			break;
        case IDC_REMOVE_SEL:
             {
                HWND hwndList = GetDlgItem(hDlg, IDC_LIST_FILES); 

                // Get selected index.
                int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0); 
                if( lbItem < 0 )return TRUE;
                mdf::gui::g_vAddedFiles.erase( mdf::gui::g_vAddedFiles.begin() + lbItem );
                //redraw
                SendMessage(hwndList, LB_RESETCONTENT , 0, 
                    0);  
            int size = mdf::gui::g_vAddedFiles.size();
       
            for (int i = 0; i < size; i++) 
            { 
                std::string sel = mdf::gui::g_vAddedFiles[i].c_str();
                int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0, 
                    (LPARAM)(sel.c_str() )  ); 
                // Set the array index of the player as item data.
                // This enables us to retrieve the item from the array
                // even after the items are sorted by the list box.
                SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM) i); 
            } 
            // Set input focus to the list box.
            SetFocus(hwndList); 
            return TRUE;             
                            
             }
             break;
        case IDC_ADD_FOLDER:
             {
                          
               std::string tmp; 
               if( mdf::gui::g_vAddedFiles.size() )
               tmp = BrowseFolder( mdf::gui::g_vAddedFiles.back() );              
               else
               tmp = BrowseFolder("");
               if( tmp != "" ){
                   
                   mdf::gui::g_vAddedFiles.push_back( tmp );
                   std::sort( mdf::gui::g_vAddedFiles.begin(), mdf::gui::g_vAddedFiles.end() );
mdf::gui::g_vAddedFiles.erase( unique( mdf::gui::g_vAddedFiles.begin(), mdf::gui::g_vAddedFiles.end() ), mdf::gui::g_vAddedFiles.end() );
                   //redraw
                 HWND hwndList = GetDlgItem(hDlg, IDC_LIST_FILES);   
                   int size = mdf::gui::g_vAddedFiles.size();
                 SendMessage(hwndList, LB_RESETCONTENT , 0, 
                    0);          
            for (int i = 0; i < size; i++) 
            { 
                std::string sel = mdf::gui::g_vAddedFiles[i].c_str();
                int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0, 
                    (LPARAM)(sel.c_str() )  ); 
                // Set the array index of the player as item data.
                // This enables us to retrieve the item from the array
                // even after the items are sorted by the list box.
                SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM) i); 
            } 
            // Set input focus to the list box.
            SetFocus(hwndList); 
                   }            
             return TRUE;
             }
             break;
			 
        case IDC_ADD_FILE:
             {
                   std::string tmp; 
               if( mdf::gui::g_vAddedFiles.size() )
               tmp = BrowseFile(mdf::gui::g_vAddedFiles.back() );              
               else
               tmp = BrowseFile("");
               if( tmp != "" ){
                   
                   mdf::gui::g_vAddedFiles.push_back( tmp );
                   std::sort( mdf::gui::g_vAddedFiles.begin(), mdf::gui::g_vAddedFiles.end() );
mdf::gui::g_vAddedFiles.erase( unique( mdf::gui::g_vAddedFiles.begin(), mdf::gui::g_vAddedFiles.end() ), mdf::gui::g_vAddedFiles.end() );
                   //redraw
                 HWND hwndList = GetDlgItem(hDlg, IDC_LIST_FILES);   
                   int size = mdf::gui::g_vAddedFiles.size();
                 SendMessage(hwndList, LB_RESETCONTENT , 0, 
                    0);          
            for (int i = 0; i < size; i++) 
            { 
                std::string sel = mdf::gui::g_vAddedFiles[i].c_str();
                int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0, 
                    (LPARAM)(sel.c_str() )  ); 
                // Set the array index of the player as item data.
                // This enables us to retrieve the item from the array
                // even after the items are sorted by the list box.
                SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM) i); 
            } 
            // Set input focus to the list box.
            SetFocus(hwndList); 
                   }            
             return TRUE;          
             }
             break;                                                                      
        case IDC_LIST_FILES:
            {
                switch (HIWORD(wParam)) 
                { 
                case LBN_SELCHANGE:
                    {
                        HWND hwndList = GetDlgItem(hDlg, IDC_LIST_FILES); 

                        // Get selected index.
                        int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0); 

                        // Get item data.
                        int i = (int)SendMessage(hwndList, LB_GETITEMDATA, lbItem, 0);

                     
                        return TRUE; 
                    } 
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}



bool  MultipleFileSelect( std::vector<std::string>& refFiles ){
mdf::gui::g_isMultiSelectAdd = false;  
DialogBox( NULL , MAKEINTRESOURCE(IDD_FILE_SELECTOR) , NULL , (DLGPROC)mdf::gui::AddFilesDialogProc );

refFiles.clear();
int s = mdf::gui::g_vAddedFiles.size();
for( int x = 0; x < s ; x++ ){
refFiles.push_back( mdf::gui::g_vAddedFiles[x] );
}
    
return mdf::gui::g_isMultiSelectAdd;  
}

 
 
 
 
 
 }
 
 
 
 
 };

#endif


 
