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

 
 #ifdef KSD_GUI_SCAN_PROGRESS
 #include <math.h>
 #include <iostream>
 
 #define IDT_SCAN_PROGRESS_TIMER			( WM_TIMER + 4  )
 
 /*
 //Contains info that is passed  to the thread
  typedef struct  ScanProgressThreadedInfo{
 
 //is there a file scan currently running 
 bool isScanRunning;
 
 //is the running scan finished
 bool isScanFinished; 
 
 //has user paused scanner
 bool isPaused;
 
 //has user stopped scanner
 bool isStopped; 
 
 //handle to scan window
 //used by thread and main class
 HWND hwndScan;
 
 //number of files to scan
 int totalFileCount;
 
 //number of file scanned
 int totalFilesScanned;
 
 //number of detected files
 int detectedFileCount;
 
 //current scan speed
 int scanSpeed;
 
 //percentage complete
 float  percentComplete;
 
 //tricky part. pointer to ScanProgressWindow.
 //this is used for accessing private members
 ScanProgressWindow* ptrSPW;
 } SPTI , *PSPTI; //abbreviation for ScanProgressThreadedInfo
 
 */
 

 
//@todo
//bug fix for close window thread. 
//since data is copied by pointer to 
//thread function, which is actually a pointer
// to an object which contains a pointer. 
//too complex.
bool g_spwCloseThread = false; 
long g_filesCounted = 0;
 
 
void SetButtonBitmap(HWND  parent ,UINT IDCCONTROL , UINT bitMapResource){
	    HBITMAP hb = LoadBitmapA(GetModuleHandle(NULL) , MAKEINTRESOURCE(bitMapResource));
     SendMessageA(GetDlgItem(parent ,IDCCONTROL) , BM_SETIMAGE , (WPARAM)IMAGE_BITMAP , (LPARAM)hb );
	 
	 }
 
 //number of seconds the scanner has been running , used by timer to calculate speed
 float g_fSecondsRunning= 1.00f;//avoid divide by zero
 
LRESULT CALLBACK ScanProgressProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
{
		SendMessage(GetDlgItem(hWndDlg,IDC_SCAN_PROGRESS), PBM_SETSTEP, 1, 1);
		
        return TRUE;
}
    break;
    
	case WM_CLOSE:
	{
	int doClose = MessageBox(hWndDlg , "Are you sure you want to stop the running scan ?" , "Scan not Complete !" , 4 | 32 );
	if( doClose == 6  ){
	ScanProgressWindow::pInfo->isRunning = false;
	}
	return TRUE;
	}
	break;
	
	case WM_TIMER: 
{
    switch (wParam) 
    { 
        case IDT_SCAN_PROGRESS_TIMER: 
            //update detection count
			{
		//check if in file enumerate mode	
		if(  ScanProgressWindow::pInfo->isCountingFiles ){
			assert(  ScanProgressWindow::pInfo->filesCounted != NULL );
			ScanProgressWindow::pInfo->ptrSPW->SetAction("Enumerating Files...");
			char* msg = new char[1024];
			int foundFiles = (int)::g_filesCounted;
			sprintf( msg , "%d Files Found..." , foundFiles );
			ScanProgressWindow::pInfo->ptrSPW->SetWindowTitle( "Enumerating files..." );
			ScanProgressWindow::pInfo->ptrSPW->SetActionStatus( msg );
			delete[] msg;
			break;
		}	
		
		int pc =0;
		if( ScanProgressWindow::pInfo->totalFileCount ) //avoid divide by zero exception
		pc = floor( (100 * ScanProgressWindow::pInfo->totalFilesScanned)/
						ScanProgressWindow::pInfo->totalFileCount );
        ScanProgressWindow::pInfo->ptrSPW->SetWindowTitle( (int)pc );
        if( pc > int(ScanProgressWindow::pInfo->percentComplete) ){
		
		do{
		ScanProgressWindow::pInfo->ptrSPW->StepProgressBar();
		 ScanProgressWindow::pInfo->percentComplete++;
		}while(  pc > int(ScanProgressWindow::pInfo->percentComplete) );
		
		}
			
			
			char* detCount = new char[ 1 + (sizeof(int)* 4) ];
			sprintf( detCount , "%d" , ScanProgressWindow::pInfo->detectedFileCount );
			SendMessage(GetDlgItem( hWndDlg , IDC_DETECTIONS_COUNT ) , WM_SETTEXT , 0 , (LPARAM)detCount );
			//calculate speed
			ScanProgressWindow::pInfo->scanSpeed = (int)(  ScanProgressWindow::pInfo->totalFilesScanned - g_fSecondsRunning );
			g_fSecondsRunning = ScanProgressWindow::pInfo->totalFilesScanned;
			sprintf( detCount , "%d" , 2*ScanProgressWindow::pInfo->scanSpeed );//since this is half a second
			SendMessage(GetDlgItem( hWndDlg , IDC_SPEED_MBPS ) , WM_SETTEXT , 0 , (LPARAM)detCount );
			//set ETA to time running
			if(  ScanProgressWindow::pInfo->scanSpeed ){
			sprintf( detCount , "%d Files" , (int)( (ScanProgressWindow::pInfo->totalFileCount  - ScanProgressWindow::pInfo->totalFilesScanned )  ) );
			SendMessage(GetDlgItem( hWndDlg , IDC_ETA ) , WM_SETTEXT , 0 , (LPARAM)detCount );
			}
			
			delete[] detCount;
			}		
             return 1; 
 
	}
}	
	break;
	case WM_COMMAND:
		switch(wParam)
		{
		case ID_SCAN_CANCEL:
{
		int doClose = MessageBox(hWndDlg , "Are you sure you want to stop the running scan ?" , "Scan not Complete !" , 4 | 32 );
	if( doClose == 6  ){
			ScanProgressWindow::pInfo->isRunning = false;
            }
}			
			return TRUE;
		case ID_SCAN_PAUSE:
            if(! ScanProgressWindow::pInfo->isPaused ){
            ScanProgressWindow::pInfo->isPaused = true;
			ScanProgressWindow::pInfo->ptrSPW->SetAction("Scanner Paused");
			SendMessage(GetDlgItem( hWndDlg , ID_SCAN_PAUSE ) , WM_SETTEXT , 0 , (LPARAM)"Start" );

			}
			else{
			ScanProgressWindow::pInfo->isPaused = false;
			ScanProgressWindow::pInfo->ptrSPW->SetAction("Scan running...");
			SendMessage(GetDlgItem( hWndDlg , ID_SCAN_PAUSE ) , WM_SETTEXT , 0 , (LPARAM)"Pause" );
          	}
			return TRUE;	
		}
		break;
	
    case WM_CTLCOLORSTATIC:
        // Set the colour of the text for our URL
        if ((HWND)lParam == GetDlgItem(hWndDlg, IDC_DETECTIONS_COUNT)) 
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


 
 
 DWORD WINAPI ThreadScanProgressWindow( LPVOID lpParam ){
 PSPTI  pInfo = (PSPTI)lpParam;
 pInfo->ptrSPW->Create();
 pInfo->ptrSPW->Show();
 
 if(   pInfo->hwnd == NULL ){
 MessageBox(NULL , "failed to create window" , "thread" , 64);
 
 }
 MSG msg;
pInfo->isRunning  = true;
while ( ( ! ::g_spwCloseThread) && pInfo->isRunning && GetMessage(&msg , pInfo->hwnd  , 0 , 0) > 0){
UpdateWindow(  pInfo->hwnd );
TranslateMessage(&msg);
DispatchMessage(&msg);
}
pInfo->isRunning = false;
pInfo->isFinished = true;
::g_spwCloseThread = false;
DestroyWindow(pInfo->hwnd);
 
 }

	
	//init common controls, for progress bar to work
	 bool ScanProgressWindow::InitCommonControls(){
	INITCOMMONCONTROLSEX InitCtrlEx;

	InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrlEx.dwICC  = ICC_PROGRESS_CLASS;
	if( ! InitCommonControlsEx(&InitCtrlEx) ){
	MessageBox(NULL , "Failed to initialize common controls. Program has to terminate" , "InitCommonControls Failed" , 32);
	ExitProcess(0);
	}   
    
	}


	//set the enumerate file count pointer
void ScanProgressWindow::SetEnumCounterPointer( long* filesEnumerated ){
if( filesEnumerated != NULL )
this->pInfo->filesCounted = filesEnumerated;
}

//turn file enumeration on or off
void ScanProgressWindow::ToggleEnumFileMode( bool  _state){
this->pInfo->isCountingFiles = _state;
}	
	
	ScanProgressWindow::ScanProgressWindow(int totalFileCount , HWND _parent = NULL ):
	threadId(0),
    hThread(0),
	parent(_parent)
	{
	::g_spwCloseThread = false;
	if(  this->singleTone == 0 ){
	delete this->pInfo;
	this->pInfo = (PSPTI) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                sizeof(SPTI));
				
	//only one running at the moment
	ScanProgressWindow::InitCommonControls();
	this->pInfo->totalFileCount = totalFileCount;
	this->pInfo->hwnd = NULL;
	this->pInfo->isPaused = false;
	this->pInfo->isRunning = false;
	this->pInfo->isFinished = false;
	this->pInfo->totalFilesScanned = 0;
	this->pInfo->detectedFileCount = 0;
	this->pInfo->scanSpeed = 0;
	this->pInfo->percentComplete = 0;
	this->pInfo->ptrSPW = this; 
	this->pInfo->filesCounted = NULL;
	this->singleTone++;
	}else{
	MessageBox( NULL , "You can only have one instance of a scan running at any point in time" , "Progress Window Singleton" , 16 );
	ExitProcess(0);
	}
	
	}
	
	ScanProgressWindow::~ScanProgressWindow(){
	//destroy window
	this->Destroy();
	//kill timer
	this->singleTone--;
	
	}
	
	bool ScanProgressWindow::Create( ){
	if( this->pInfo->hwnd != NULL )return true; //already created 
	this->pInfo->hwnd = CreateDialog(GetModuleHandle(NULL) , MAKEINTRESOURCE(IDD_SCAN_PROGRESS) , this->parent , (DLGPROC)ScanProgressProc );
	if(   this->pInfo->hwnd == NULL )return false;
	//start the timer
	SetTimer(this->pInfo->hwnd ,              
    IDT_SCAN_PROGRESS_TIMER,            
    500,                 // 0.5-second interval 
    (TIMERPROC) NULL);     // no timer callback 
	return true;
	}
	
	bool ScanProgressWindow::Destroy(){
    ::KillTimer( this->pInfo->hwnd , IDT_SCAN_PROGRESS_TIMER );
    this->pInfo->isRunning = false;
	::g_spwCloseThread = true;
	//dangerous !!!! 
	//TerminateThread( this->hThread , 0 );
	do{ ::DestroyWindow( this->pInfo->hwnd);
	  this->pInfo->isRunning = false;
	}while( this->pInfo->isRunning );
	this->pInfo->hwnd  = NULL;
	return true;
	}
     
	
	//show the window. initially the window is hidden so this funtion
	//must be called and when it is called the message loop is started if it had
	//not already being started
	bool  ScanProgressWindow::Show( bool _show ){
	if( _show)
	::ShowWindow( this->pInfo->hwnd ,  SW_SHOW);
	else
	::ShowWindow( this->pInfo->hwnd ,  SW_HIDE);
	
	}
	
	//calls createwindow if it had not been called, then shows the window
   //then it starts the message loop thread and sets all PSPTI
 //structure variables to indicate that the scan is running 
	bool  ScanProgressWindow::Start(){
	//code to start thread
	::g_fSecondsRunning = false;
	assert( this->pInfo != NULL );
      
	
        this->hThread = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            ThreadScanProgressWindow,       // thread function name
            this->pInfo ,          // argument to thread function 
            0,                      // use default creation flags 
            &this->threadId );   // returns the thread identifier 


        // Check the return value for success.
        // If CreateThread fails, terminate execution. 
        // This will automatically clean up threads and memory. 

        if ( this->hThread == NULL) 
        {
           MessageBox(NULL , "Failed to create thread. Will now exit" , "Start()" , 32);
           ExitProcess(3);
        }
	

	
	
	} // End of main thread creation loop.	
	
	
	//check if the window is valid
	bool ScanProgressWindow::IsValidWindowHandle(){
	if( this->pInfo->hwnd == NULL) return false;
	return true;
	}
	
	//check if scan is running
	bool ScanProgressWindow::IsRunning(){
	if(  
	( this->pInfo->isRunning) 
	) return true;
	return false;
	}
	
	//check if scan is paused
bool ScanProgressWindow::IsPaused(){
if(  this->pInfo->isPaused )return true;
return false;
}

	//check if it has completed
		//only complete when filecount = scannedfiles 
bool ScanProgressWindow::IsFinished(){

if (  this->pInfo->isCountingFiles ){
return this->pInfo->isFinished;
}

if (   ( this->pInfo->isFinished  ) ||
	   ( this->pInfo->totalFileCount == this->pInfo->totalFilesScanned )
	   )return true;
return false;
}

//get percent [ use also for getting progress bar value ]
float ScanProgressWindow::GetPercent()const{
return this->pInfo->percentComplete;
}

//get window handle
HWND ScanProgressWindow::GetWindowHandle()const{
return this->pInfo->hwnd;
}

//get scan speed
int ScanProgressWindow::GetScanSpeed(){
return (int)this->pInfo->scanSpeed;
}

//get estimated time of finish
std::string ScanProgressWindow::GetETA(){
//code goes here
return std::string("Calculating...");
}

//get current action
std::string ScanProgressWindow::GetAction(){
HWND t = this->pInfo->hwnd;            
return this->ReadCtrlText( t , IDC_ACTION);
}

	///		Set  Window values ///

	
	//set curent scan action
	void ScanProgressWindow::SetAction( std::string _action ){
	this->SetControlText( this->pInfo->hwnd , IDC_ACTION , _action );
	
	}

	//set current action status. this can be the file being scanned or whatever
	void ScanProgressWindow::SetActionStatus( std::string  _status){
	
		this->SetControlText( this->pInfo->hwnd , IDC_ACTION_TEXT , _status );
	
	}
	
	//set detected file count
	void  ScanProgressWindow::SetDetectedFileCount( int num ){
		this->SetControlText( this->pInfo->hwnd , IDC_DETECTIONS_COUNT , this->toString( num ) );
		this->pInfo->detectedFileCount = num;
	}
	
	//increment detected file count [recommended]
    void ScanProgressWindow::IncrementDetectedCount(){
	this->pInfo->detectedFileCount++;
	this->SetDetectedFileCount(  this->pInfo->detectedFileCount );
	}

	void ScanProgressWindow::SetTotalFileCount( int  _total){
	this->pInfo->totalFileCount = _total;
	}
	
	void ScanProgressWindow::IncrementTotalFileCount(){
	this->pInfo->totalFileCount++;
	}
	
	void ScanProgressWindow::IncrementFileScanCount(){
	this->pInfo->totalFilesScanned++;
	}
	
	//very important must be updated regularly so that ETA can be calculated by thread
	void ScanProgressWindow::SetSpeed( int  _speed){
	this->pInfo->scanSpeed = _speed;
		this->SetControlText( this->pInfo->hwnd , IDC_SPEED_MBPS , this->toString( this->pInfo->scanSpeed ) );
	
	}
	
	//set the title of the window
	void ScanProgressWindow::SetWindowTitle( std::string  _title){
	SetWindowText(this->pInfo->hwnd , _title.c_str() );
    return;
	}
	
	//set the window title. by progress
	void ScanProgressWindow::SetWindowTitle(  int _percent ){
	std::string speed = this->toString( _percent );
	this->SetWindowTitle( speed + " % Done |  Mthwakazi Defender " );
	return ;
	}
	
	//set the progress bar value
	void ScanProgressWindow::SetProgressBarValue( int  _value){
	SendMessage(GetDlgItem(this->pInfo->hwnd,IDC_SCAN_PROGRESS), PBM_SETSTEP, 1 , _value);
	this->StepProgressBar();
	SendMessage(GetDlgItem(this->pInfo->hwnd,IDC_SCAN_PROGRESS), PBM_SETSTEP, 1, 1); //back to 1 percent step
	}
	
	//step the progress bar by last set step value
	void ScanProgressWindow::StepProgressBar(){
	SendMessage(GetDlgItem(this->pInfo->hwnd,IDC_SCAN_PROGRESS), PBM_STEPIT, 0, 1);
	}
 
	 std::string  ScanProgressWindow::toString(  int _val ){
 char *buffer = new char[25];
itoa(   _val , buffer , 10 ); 
 std::string tmp = std::string( buffer );
 delete[] buffer;
 return tmp;
 }


 std::string  ScanProgressWindow::toString( float _val ){
 char *buffer = new char[25];
ltoa(  (long int) _val , buffer , 10 ); 
 std::string tmp = std::string( buffer );
 delete[] buffer;
 return tmp;
 }
 
	
	std::string ScanProgressWindow::ReadCtrlText(HWND hwnd , UINT control){
	   int len = GetWindowTextLength(GetDlgItem(hwnd , control));
    char* buffer = new char[(len+2)];
    GetWindowText(GetDlgItem(hwnd , control) , buffer , len+1);
	std::string tmp(buffer);
    delete[] buffer;
	return tmp;
	}

	int ScanProgressWindow::ReadCtrlInt(HWND hwnd , UINT control){
	  BOOL h;
    return GetDlgItemInt(hwnd , control , &h , FALSE );
	}
 
    void ScanProgressWindow::SetControlFocus(HWND hwnd , UINT  control  ){
	    ::SetFocus(GetDlgItem(hwnd , control));
    return;
	}
	
	void ScanProgressWindow::SetControlText(HWND parent , UINT control , std::string text ){

//now lets append text
    SendMessage(GetDlgItem( parent , control) , WM_SETTEXT , 0 , (LPARAM)text.c_str() );

    return;
	}
	
	 void ScanProgressWindow::SetButtonBitmap(HWND  parent ,UINT IDCCONTROL , UINT bitMapResource){
	    HBITMAP hb = LoadBitmapA(GetModuleHandle(NULL) , MAKEINTRESOURCE(bitMapResource));
     SendMessageA(GetDlgItem(parent ,IDCCONTROL) , BM_SETIMAGE , (WPARAM)IMAGE_BITMAP , (LPARAM)hb );
	 
	 }
	 
	 
 
 
PSPTI ScanProgressWindow::pInfo = new SPTI;
int ScanProgressWindow::singleTone = 0; 

 
#endif 

