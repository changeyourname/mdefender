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

 
 #ifndef KSD_GUI_SCAN_PROGRESS
 
 #define KSD_GUI_SCAN_PROGRESS
 
#include <windows.h> 
#include <string>

 
 
 
 class ScanProgressWindow;
 
 
 //Contains info that is passed  to the thread
  typedef struct  ScanProgressThreadedInfo{
 
 //is there a file scan currently running 
 bool isRunning;
 
 //is the running scan finished
 bool isFinished; //lol my father is insane.
 
 //has user paused scanner
 bool isPaused;
 
 //has user stopped scanner
 bool isStopped; 
 
 //is it still enumerating files
 bool isCountingFiles;
 
 //pointer to number of files counted so far
 long* filesCounted;
 
 //handle to scan window
 //used by thread and main class
 HWND hwnd;
 
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
 
 
 DWORD WINAPI ThreadScanProgressWindow( LPVOID lpParam );


//please note that in order to work
//common controls must be initiated
 
 class ScanProgressWindow{
 
 //so that thread can have access to some functions here

	public:
	
	//init common controls, for progress bar to work
	static bool InitCommonControls();
	
	
	ScanProgressWindow(int totalFileCount , HWND parent );
	
	~ScanProgressWindow();
	
	bool Create( );
	
	//show the window. initially the window is hidden so this funtion
	//must be called and when it is called the message loop is started if it had
	//not already being started
	bool Show( bool _show = true );
	
	//calls createwindow if it had not been called, then shows the window
   //then it starts the message loop thread and sets all PSPTI
 //structure variables to indicate that the scan is running 
	bool  Start();
	
	//terminate window
	bool Destroy();
	
	//check if the window is valid
	bool IsValidWindowHandle();
	
	//check if scan is running
	bool IsRunning();
	
	//check if scan is paused
bool IsPaused();

	//check if it has completed
		//only complete when filecount = scannedfiles 
bool IsFinished();

//get percent [ use also for getting progress bar value ]
float GetPercent()const;	

//get window handle
HWND GetWindowHandle()const;

//get scan speed
int GetScanSpeed();

//get estimated time of finish
std::string GetETA();

//get current action
std::string GetAction();

//set the enumerate file count pointer
void SetEnumCounterPointer( long* filesEnumerated );

//turn file enumeration on or off
void ToggleEnumFileMode( bool );

	///		Set  Window values ///

	
	//set curent scan action
	void SetAction( std::string );

	//set current action status. this can be the file being scanned or whatever
	void SetActionStatus( std::string );
	
	//set detected file count
	void SetDetectedFileCount( int );
	
	//increment detected file count [recommended]
    void IncrementDetectedCount();
	
	//[recommended for enumerate]
	void IncrementTotalFileCount();
	
	//chould be called after a file has been scanned
	void IncrementFileScanCount();

	void SetTotalFileCount( int );
	
	//very important must be updated regularly so that ETA can be calculated by thread
	void SetSpeed( int  );
	
	//set the title of the window
	void SetWindowTitle( std::string );
	
	//set the window title. by progress
	void SetWindowTitle(  int );
	
	//set the progress bar value
	void SetProgressBarValue( int );
	
	//step the progress bar by 1 %
	void StepProgressBar();
  //contains all scan window info
	static PSPTI  pInfo;
		//for version 1, only one scan progress window  per process 
	static int singleTone;
	
 protected: 
	//parent
	HWND parent;

	//started thread id
	DWORD threadId;
	
	//started thread handle
	HANDLE hThread;
	
	//prompt user to pause and pause if yes
	bool PromptPause(  );
	
	//prompt user to stop scan and stop if yes
	bool PromptStop();
	
	std::string toString( int );
	
	std::string toString(  float );
	
	std::string ReadCtrlText(HWND hwnd , UINT control);

	int ReadCtrlInt(HWND hwnd , UINT control);	
 
    void SetControlFocus(HWND hwnd , UINT  control  );
	
	void SetControlText(HWND parent , UINT control , std::string );
	
	 void SetButtonBitmap(HWND  parent ,UINT IDCCONTROL , UINT bitMapResource);
	 
	 
 };
 
 
 
#endif 
