#define _WIN32_IE 0x0500

#include <exception>

#include <windows.h>
#include <string>
#include <vector>
#include <stdlib.h>     
#include <time.h>       

#include "../../include/config.h"

#include "../../res/resource.h"


#include "../../include/engine.hpp"
#include "../../src/engine.cpp"



#include "../../include/gui/about.hpp"
#include "../../src/gui/about.cpp"


 
#include "../../include/gui/startup_screen.hpp"
#include "../../src/gui/startup_screen.cpp"

#include "../../include/gui/scan_results.hpp"
#include ".../../src/gui/scan_results.cpp"

//gui

#include "../../include/gui/scan_progress.hpp"
#include "../../src/gui/scan_progress.cpp"



#ifndef MDF_VERSION
#define MDF_VERSION "1.0.0"
#endif






#include "include/gui/file_selector.hpp"
#include "src/gui/file_selector.cpp"



#include "include/singletone.hpp"

using namespace std;



typedef struct ProcessScanResults{
 int deletedFiles;
 int totalFiles;
 int ignoredFiles;
 int action;
 string errorMsg;
}PDS;



LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
bool OnCustomScan(HWND _parent );
bool OnFullScan( HWND _parent );
bool StartUp();
void OnExit();
PDS  ProcessResults( vector<SROB>& results , int actionToApply );
bool ScanProcesses( vector<SROB>&  scanResults , ScanProgressWindow* spw , bool useSigEngine );
bool ScanFileSystem( vector<string>& files , vector<SROB>& scanResults , ScanProgressWindow* spw , bool useSigEngine );

namespace mdf{
FileSystem* fs=0;
SystemEnvironment* sys=0;
ConfigReader* cfg=0;   
HashScannerEngine* hse=0;
RuleScannerEngine* rse=0;
ProcessManager* ps=0;       
AV_Logger* log=0;          
KSingleTone* mutex=0;

bool scanRunning = false;          
vector<SROB> scanResults;
int expireTime ;//unix timestamp of when the beta version is unsupported


string workingDir;
string currentDir;

string startUpMessage = "";


namespace config{
 string version;
 string signatureCount;
 string scannedFilesCount;
 string lastUpdate;
 string CONFIG_FILE = "config.bin";
 string HSE_DATABASE  = "vdf.bin";
 vector<string> RSE_DATABASES; 
 string LOG_FILE = "mdefender.log";	
 string SCAN_TYPES ;
 int rseTimeout = 2500;
 int hseTimeout = 1000;//1 sec timeout
 };          


};

void OnExit(){
     
     delete mdf::fs;
     delete mdf::cfg;
     delete mdf::sys;
     delete mdf::hse;
     delete mdf::rse;
     delete mdf::log;
     delete mdf::ps;
   
     }


	 
	 

PDS  ProcessResults( vector<SROB>& results , int actionToApply ){

PDS pr;
pr.deletedFiles = 0;
pr.ignoredFiles = 0;
pr.action = actionToApply;
pr.totalFiles = results.size();

string err;

for( int i = 0; i < results.size() ; i++ ){
     bool result = false;
     if( actionToApply == 0 ){ //delete all
         result = mdf::fs->DeleteFile( results[i].filePath );
         }else{
               
         }
     if( result ){
         if( actionToApply == 0 ){
             pr.deletedFiles++;
             }else{
             pr.ignoredFiles++;  
             }
         
         }else{
          if( actionToApply == 0 ){
              err += " \n Failed to delete file: " + results[i].filePath + "\t" + results[i].malwareName ;
              
              }           
               
         }
     }
     pr.errorMsg = err;
     return pr;
}


bool g_IsFileEnumerateMode = false;	 

 DWORD WINAPI MonitorFileEnumeration( LPVOID lpParam ){
	ScanProgressWindow* spw = (ScanProgressWindow*)lpParam;
	assert( spw != NULL );
	::g_IsFileEnumerateMode = true;
    
    while( 1 ){
	char* msg = new char[1024];
			int foundFiles = (int)::g_filesCounted;
			sprintf( msg , "%d Files Found..." , foundFiles );
	        spw->SetActionStatus( string(msg) );
    delete[] msg;
    		
	if( spw->IsFinished() ){
    ::g_IsFileEnumerateMode = false;
	break;
	}
	_sleep(50);
	
	}
	::g_IsFileEnumerateMode = false;
	return 0;
 
 }



bool ScanFileSystem( vector<string>& files , vector<SROB>& scanResults , ScanProgressWindow* spw , bool useSigEngine ){
assert( spw != 0 );
    
spw->SetEnumCounterPointer( &g_filesCounted );
spw->ToggleEnumFileMode( true );
bool doRecurse = true; //default recursive scan
vector<ScanObject> fileObjs;

DWORD threadId = 0;
HANDLE hThread = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            MonitorFileEnumeration,       // thread function name
            spw ,          // argument to thread function 
            0,                      // use default creation flags 
           &threadId );   // returns the thread identifier 
::g_IsFileEnumerateMode = true;
Enum_Folder_Info fi = mdf::fs->GetDirectoryScanObjects(files , fileObjs , &::g_filesCounted , doRecurse , &::g_IsFileEnumerateMode );

//explicitly kill thread
//dangerous !!!! 
::g_IsFileEnumerateMode = false;
TerminateThread( hThread , 0 );


if( fi.fileCount == 0 ){
return false;   
}

spw->ToggleEnumFileMode( false );
{   
char* msg = new char[1024];
sprintf( msg , "Found %d files in %d sub directories. " , fi.fileCount , fi.subFolderCount );
spw->SetActionStatus( msg );
delete[] msg;
} 
    
spw->SetAction("FileSystem Scanner Running ... ");
int size = fileObjs.size();
spw->SetAction("FileSystem Scanner Running ... ");
spw->SetTotalFileCount( size );
int step = ( size / 100 );    
int detCount = 0;
int lastDetCount;

for( int x = 0;  x < size && ! spw->IsFinished() ; x++ ){
if( spw->IsPaused() ){
     _sleep(10);
     x--;
     continue;
}    
   
         
         if( x%5 ) 
        spw->SetActionStatus( fileObjs[x].GetFilename() );
        
        
       
    
    mdf::rse->Scan( fileObjs[x] );   
    
    detCount = RuleScannerEngine::vHashEngineDetected.size() + HashScannerEngine::vHashEngineDetected.size();
    if( detCount != lastDetCount){
    spw->SetDetectedFileCount( detCount );
    lastDetCount = detCount;
    }
    spw->IncrementFileScanCount();
       
}   
//set progress bar to 100

spw->SetAction("100%\tFileSystem Scanner Finished");
spw->SetActionStatus("");
//copy out results
  {
   
   for( int x = 0 ; x < HashScannerEngine::vHashEngineDetected.size() ; x++ )
   scanResults.push_back( HashScannerEngine::vHashEngineDetected[x] );
   
   for( int x = 0 ; x < RuleScannerEngine::vHashEngineDetected.size() ; x++ )
   scanResults.push_back( RuleScannerEngine::vHashEngineDetected[x] );
   
  HashScannerEngine::vHashEngineDetected.clear();
  RuleScannerEngine::vHashEngineDetected.clear();                              
                                 
   
   }
return true;
}



bool OnFullScan(HWND _parent ){
std::vector<std::string> scanFiles;
bool scanProcesses = true;

//first clear previous filles
mdf::gui::ClearBrowseFileBuffer();
mdf::gui::SetDefaultPaths( mdf::fs->GetAllDrives() );
MessageBox( _parent , "Mthwakazi Defender will now scan all of the fixed drives on your computer.\n\nNB: This may be slow and take long to enumerate the files.\n\tPlease be patient" , "Full System Scan" , 64 );
if( ! mdf::gui::MultipleFileSelect( scanFiles ) ){
    return false;
    }
    
  
//hide parent window
ShowWindow( _parent , SW_HIDE );

ScanProgressWindow* spw = new ScanProgressWindow(0); 
vector<SROB> scanResults;

spw->Start();

//to avoid being too slow, each folder is scanned individually

int driveCount = scanFiles.size();
     
ScanFileSystem( scanFiles , scanResults ,  spw , false );

if( scanProcesses )
ScanProcesses( scanResults , spw , false );
//do not use signature engine for scanning processes in custom scan mode
spw->Destroy();
if( scanResults.size() ){
    int actionToTake=0;
mdf::gui::ScanResultsWindow(NULL ,  scanResults , actionToTake );
     
}
ShowWindow( _parent , SW_SHOW );
delete spw;
spw = 0;
return true;
     }

bool OnCustomScan( HWND _parent ){
std::vector<std::string> scanFiles;
bool scanProcesses = false;

//first clear previous filles
mdf::gui::ClearBrowseFileBuffer();
mdf::gui::MultipleFileSelect( scanFiles ) ;
bool fastMode = false;    

if( MessageBox(_parent , "Fast Scan mode offers faster scanning. It can reduce scan time by several times and is effective.\n\nDo you want to test use scan ? ","Fast Scan Mode" , 4 | 32 ) == 6 ){
 fastMode = true;
 
}
//set scan mode
mdf::rse->SetFastScan( fastMode );

if( MessageBox(_parent , "Do you want to scan running processes as well ? ","Scan Processes" , 4 | 32 ) == 6 ){
 scanProcesses = true;        
}else{
if ( scanFiles.size() == 0 ){
     return false;
     }      
}      

//hide parent window
ShowWindow( _parent , SW_HIDE );

ScanProgressWindow* spw = new ScanProgressWindow(0); 
vector<SROB> scanResults;



spw->Start();

_sleep(50 );//wait for thread to start
if( scanFiles.size() )
ScanFileSystem( scanFiles , scanResults ,  spw , ! fastMode );


if( scanProcesses )
ScanProcesses( scanResults , spw , false );
//do not use signature engine for scanning processes in custom scan mode
spw->Destroy();
if( scanResults.size() ){
    int actionToTake=0;
mdf::gui::ScanResultsWindow(NULL ,  scanResults , actionToTake );
PDS pResults = ProcessResults( scanResults , actionToTake );         
char* report = new char[1024];
sprintf( report , "\t\tScan Results\t\t\n\nFiles Scanned:\t%d\nTotal Detected:\t%d files\n\nDeleted Files:\t%d\nIgnored Files\t%d\n\nErrors Encountered:\n%s"
                , scanFiles.size() , pResults.totalFiles , pResults.deletedFiles , pResults.ignoredFiles , pResults.errorMsg.c_str() );
MessageBox( NULL , report , "Mthwakazi Defender | Scan Report" , 64 );                 
delete[] report;
}else{
      
 MessageBox( NULL , "The scan has completed succesfully.\n\nNo malicious files were found on your computer.\n" , "No Malware Found | M-Defender" , 64 );     
 
}    
ShowWindow( _parent , SW_SHOW );
delete spw;
spw = 0;
return true;
}

bool ScanProcesses( vector<SROB>&  scanResults , ScanProgressWindow* spw , bool useSigEngine ){
assert( spw != NULL );

spw->SetAction("Scanning  Processes...");
    
//now print out all processes
vector<Process> ps = mdf::ps->ListAllProcesses();

int psCounter = 0;
int size = ps.size();
for( int x = 0;  ! spw->IsFinished() && x < size ; x++ ){

if( spw->IsPaused() ){
 _sleep( 10 );   
 x--;
 continue;   
}

{ 
 spw->SetWindowTitle( (x*100)/size ); 
 spw->SetAction("Scanning  Processes...");
 char* pid = new char[128];
 itoa( ps[x].pid , pid , 10 );
 spw->SetActionStatus("PID: "+ std::string(pid) + "\t " + ps[x].exeName );
 delete pid; 
} 
 //now print out loaded modules
for(int y = 0;  ! spw->IsFinished() &&  y < ps[x].loadedModules.size() ; y++ ){
         ScanObject obj( ps[x].loadedModules[y] );
         mdf::fs->GetScanObject( ps[x].loadedModules[y] , &obj);
         mdf::fs->GetFileAttributes( &obj );
         
         if( useSigEngine ){
         mdf::fs->GenerateHashSignature( &obj);
         mdf::hse->Scan( obj );
         }
         
         mdf::rse->Scan( obj );  
         
         }   
     
} 

//copy out results
  {
   
   for( int x = 0 ; x < HashScannerEngine::vHashEngineDetected.size() ; x++ )
   scanResults.push_back( HashScannerEngine::vHashEngineDetected[x] );
   
   for( int x = 0 ; x < RuleScannerEngine::vHashEngineDetected.size() ; x++ )
   scanResults.push_back( RuleScannerEngine::vHashEngineDetected[x] );
   
  HashScannerEngine::vHashEngineDetected.clear();
  RuleScannerEngine::vHashEngineDetected.clear();                              
                                 
   
   }
   return true;

}  

bool StartUp(  ){
//check if only one instance is running
mdf::mutex = new KSingleTone();

//register exit function
atexit( OnExit );

//no debugger allowed for final version
#ifndef DEBUG_MDF
if(  IsDebuggerPresent() ){
MessageBox(NULL , "Sorry\r\nMthwakazi Defender cannot be run with a debugger attached to it. Consider recompiling M-Defender to enable this feature" , "No Debugger Allowed" , 16);
FatalExit(0);
}
#endif


mdf::sys = new SystemEnvironment;
assert( mdf::sys != 0 );
mdf::cfg = new ConfigReader( mdf::config::CONFIG_FILE , false );
assert( mdf::cfg != 0 );
mdf::currentDir = mdf::sys->GetExecutableDirectory();
assert( mdf::currentDir != "");
mdf::workingDir = mdf::sys->GetWorkingDirectory();
assert( mdf::workingDir != "");
mdf::cfg->SetFileName( mdf::workingDir + "\\data\\" + mdf::config::CONFIG_FILE );
//open config
if( ! mdf::cfg->Open()  ){
MessageBox( NULL ,  "Failed to load the configuration file.\r\nPlease make sure the installation is complete.\r\nIf problem persists, reinstall M-Defender and/or conatct the M-Team" , "Load Config File Failed" , 16);
FatalExit(0);
} 
//config is open
//check if valid config file
string name;
 mdf::cfg->Get("program" , name );
assert(bool ( name == std::string("MDEFENDER") ) );
 
assert(  mdf::cfg->Get( "sig_file" , mdf::config::HSE_DATABASE ) );
assert( mdf::cfg->Get( "last_update" , mdf::config::lastUpdate ) );
assert( mdf::cfg->Get( "scanned_files" , mdf::config::scannedFilesCount ) );
assert( mdf::cfg->Get( "signature_count" , mdf::config::signatureCount ) );
assert( mdf::cfg->Get( "version" , mdf::config::version ) );
mdf::cfg->Get( "startup_msg" , mdf::startUpMessage  );
assert ( mdf::cfg->Get("log_file" , mdf::config::LOG_FILE ) );
//loging is not really necessary tho
mdf::config::HSE_DATABASE = mdf::currentDir + "\\data\\" + mdf::config::HSE_DATABASE;
if( mdf::config::version !=  std::string(MDF_VERSION)  ){
MessageBox(NULL , "Warning, \r\n This program is outdated and is using a newer  virus definition file. \r\n We recommend you update the entire software to avoid bugs " , "Outdated Version" , 64);
}
//start filesystem
mdf::fs = new FileSystem;

assert( mdf::cfg->Get("scan_ext" , mdf::config::SCAN_TYPES)  );
//extract file extensions to scan
{
 std::string delimiter = ",";
	string exts = mdf::config::SCAN_TYPES;
    size_t pos = 0;
    std::string token;
    while ((pos = exts.find(delimiter)) != std::string::npos) {
    token = exts.substr(0, pos);
    mdf::fs->AddExtension(token);
    exts.erase(0, pos + delimiter.length());
    }

}

//start logger
mdf::log = new AV_Logger(  mdf::config::LOG_FILE ); 
//start processmanager
mdf::ps = new ProcessManager;
//start scanner engines

if (    ( ! mdf::fs->GetFileSize(mdf::config::HSE_DATABASE )  )  )  
 {
 MessageBox(NULL , "Error\r\nHash signature virus defintion file does not exist or is corrupt. Pleae reinstall M-Defender ", "Corrupt Installation" , 16);
 FatalExit(0);
 }
mdf::hse = new HashScannerEngine( mdf::config::HSE_DATABASE , mdf::config::hseTimeout );


{
 std::string files;
    assert(  mdf::cfg->Get("rule_files" , files) );
    //now 
    std::string delimiter = ";";

    size_t pos = 0;
    std::string token;
    while ((pos = files.find(delimiter)) != std::string::npos) {
    token = files.substr(0, pos);
    mdf::config::RSE_DATABASES.push_back( mdf::sys->GetExecutableDirectory() + "\\data\\" + token);
    files.erase(0, pos + delimiter.length());
    }              
              
} 
mdf::rse = new RuleScannerEngine( mdf::config::RSE_DATABASES , mdf::config::rseTimeout);

//start engine
if( ! mdf::rse->StartEngine() ){
MessageBox(NULL , "Failed to load the Rule Scanner Engine Database File \r\nPLease make sure that it is not corrupt.\r\nProgram will now terminate" , "RSE Error" , 16);
FatalExit(0);
}

if( ! mdf::hse->StartEngine() ){
MessageBox(NULL , "Failed to load the Hash Scanner Engine Database File \r\nPLease make sure that it is not corrupt.\r\nProgram will now terminate" , "HSE Error" , 16);
FatalExit(0);
}

//done :)
return true;



}




string izaga[] = {
"Umuntu ngumuntu ngabantu"
,"Saze sabasihle isizwe sakoMthwakazi, uMbuthwa okazi!, undlela zimhlophe!, njenginsimu yamaluba"
,"IBosso yiBosso"
,"Hamba juba bazakuchutha phambili"
,"Indlela ibuzwa kwabaphambili"
,"Isalakutshelwa sibona ngomopho"
,"Inkiwane elibomvu libolile"
,"Lead Programmer ,Malware detection engine \t Trevor Sibanda "
," GUI, Cloud Interface \t Clifford J Mthemba"
,"Umthwenthwe uhlaba usamila "
, "Umtwana ongakhaliyo ufela embelekweni" 
};


#define MAX_ZAGA_COUNT       11


int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{

    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */

   //startup window
   {
   StartUPWindow stw;
    StartUp(); //init all components
    do{
                 _sleep(1);
       }while( stw.isActive() );

    }
    //all components should be initialised by now
    
    /* The class is registered, let's create the program*/
    hwnd = CreateDialog( NULL , MAKEINTRESOURCE( IDD_MAIN_WIN ) , 0 , (DLGPROC)WindowProcedure );

    /* Make the window visible on the screen */
    ShowWindow (hwnd , SW_SHOW);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, hwnd, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

     /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}



/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_INITDIALOG:
             {
         /* initialize random seed: */
         srand (time(NULL));
         HBITMAP header = LoadBitmap( GetModuleHandle(NULL) , MAKEINTRESOURCE(IDB_BANNER) );


         if( ! header ){
            DWORD lerror = GetLastError();
            char* lmsg = new char[1024];
            itoa(lerror , lmsg , 10);
            MessageBox(NULL , lmsg,  "Error" , 16 );
            delete[] lmsg;
         }
         //EXIT
         HICON hic = LoadIcon( GetModuleHandle(NULL) , MAKEINTRESOURCE(IDI_EXIT ) );

         SendMessage( GetDlgItem( hwnd , IDC_EXIT ) , BM_SETIMAGE , IMAGE_ICON , (LPARAM)hic);
         DeleteObject( hic );
         //ABOUT
         hic = LoadIcon( GetModuleHandle(NULL) , MAKEINTRESOURCE(IDI_ABOUT ) );
         SendMessage( GetDlgItem( hwnd , IDC_ABOUT ) , BM_SETIMAGE , IMAGE_ICON , (LPARAM)hic);
         DeleteObject( hic );
         //UPDATE
         hic = LoadIcon( GetModuleHandle(NULL) , MAKEINTRESOURCE(IDI_UPDATE ) );
         SendMessage( GetDlgItem( hwnd , IDC_UPDATE ) , BM_SETIMAGE , IMAGE_ICON , (LPARAM)hic);
         DeleteObject( hic );
         //CUSTOM SCAN
         hic = LoadIcon( GetModuleHandle(NULL) , MAKEINTRESOURCE(IDI_CUST_SCAN ) );
         SendMessage( GetDlgItem( hwnd , IDC_CUSTOM_SCAN ) , BM_SETIMAGE , IMAGE_ICON , (LPARAM)hic);
         DeleteObject( hic );
         //FULL SCAN
          hic = LoadIcon( GetModuleHandle(NULL) , MAKEINTRESOURCE(IDI_FULL_SCAN ) );
         SendMessage( GetDlgItem( hwnd , IDC_FULL_SCAN ) , BM_SETIMAGE , IMAGE_ICON , (LPARAM)hic);
           DeleteObject( hic );
         //random saga
         SendMessage( GetDlgItem( hwnd , IDC_IZAGA ) , WM_SETTEXT , 0 , (LPARAM)( izaga[rand()%MAX_ZAGA_COUNT].c_str() ) );
         //header image
         SendMessage( GetDlgItem( hwnd , IDC_MAIN_PIC ) , STM_SETIMAGE , IMAGE_BITMAP , (LPARAM)header );
         
         //set info
         string info =  "Version " + mdf::config::version + "\t" + mdf::config::signatureCount + " signatures\t Last Update: " + mdf::config::lastUpdate  ;
         SendMessage( GetDlgItem( hwnd , IDC_INFO ) , WM_SETTEXT , 0 , (LPARAM)(info.c_str() ) ); 
         
         }
          return 1;
             break;

        case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDC_ABOUT:
            {
            AboutWindow abw;
            ShowWindow( hwnd , SW_HIDE );
            do{
                        _sleep(1);
                        }while( abw.IsActive() );
            }
            ShowWindow( hwnd , SW_SHOW );
            break;
        case IDC_EXIT:
             if( MessageBox(hwnd , "Are you sure you want to exit ?" , "Confirm exit" , 4 | 32 ) == 6 ){
                 MessageBox(hwnd , "Thank you for using Mthwakazi Defender, We hope it was useful.\n\nFollow us on Facebook and Twitter, just search for Mthwakazi Defender.\n\nWe rely on your feedback so we can make M-Defender an exceptionally great tool." , "M-Defender | Beta Version" , 64 );
                 ExitProcess( 0 );
                 }
           break;
       case IDC_UPDATE:
            {
            string updateProgram = mdf::workingDir + "\\updater.exe";
            ShellExecute( GetDesktopWindow() , "open" , updateProgram.c_str() , NULL , NULL , SW_SHOW );
            
            }
            break;

        case IDC_CUSTOM_SCAN:
             {

             OnCustomScan(hwnd);
             }
             break;
        case IDC_FULL_SCAN:
             {
             OnFullScan(hwnd);

             }
             break;
             }
        return true;

        break;
        case WM_CLOSE:
             {
                 if( MessageBox(hwnd , "Are you sure you want to exit ?" , "Confirm exit" , 4 | 32 ) == 6 ){
                 MessageBox(hwnd , "Thank you for using Mthwakazi Defender, We hope it was useful.\n\nFollow us on Facebook and Twitter, just search for Mthwakazi Defender.\n\nWe rely on your feedback so we can make M-Defender an exceptionally great tool." , "M-Defender | Beta Version" , 64 );
                 ExitProcess( 0 );
                 }        
             }
             break;         
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return 0;
    }

    return 0;
}




