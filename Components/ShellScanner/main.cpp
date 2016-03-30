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



#include "../../include/tclap/CmdLine.h"
 
 

#ifndef MDF_VERSION
#define MDF_VERSION "1.0.0"
#endif






#include "include/gui/file_selector.hpp"
#include "src/gui/file_selector.cpp"



#include "include/singletone.hpp"

using namespace std;
using namespace TCLAP;


typedef struct ProcessScanResults{
 int deletedFiles;
 int totalFiles;
 int ignoredFiles;
 int action;
 string errorMsg;
}PDS;



LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
bool ShellScan( std::vector<std::string>& files , bool fastMode , bool scanProcesses);
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



bool ScanFileSystem( vector<string>& files , vector<SROB>& scanResults , ScanProgressWindow* spw , bool useSigEngine ){
assert( spw != 0 );
    
spw->SetEnumCounterPointer( &::g_filesCounted );
spw->ToggleEnumFileMode( true );
bool doRecurse = true; //default recursive scan
vector<ScanObject> fileObjs;
//start enumerate thread
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


bool ShellScan(  std::vector<std::string>& scanFiles , bool fastMode = true  , bool scanProcesses = false){


//set scan mode
mdf::rse->SetFastScan( fastMode );


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
if(  scanResults.size() ){
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


int main(int argc, char** argv)
{


    bool doStartupScreen = true;
    std::vector< std::string > fileList;
    bool doRecurse = true;
    bool doScanProcesses = true;
    bool doFastScan = true;
    std::vector<ScanObject> files;
    vector<SROB> scanResults;
    
	try {  

	// Define the command line object.
	CmdLine cmd("Mthwakazi Defender Shell Scanner", ' ', "1.0.0");


	SwitchArg sStartup("s","no-splash","Dont show splash screen", true);
	cmd.add( sStartup );
	
	SwitchArg sRecurse("r","recursive","Recursively scan subfolders", true);
	cmd.add( sRecurse );

    SwitchArg sProc("p","scan-processes","Scan all running processes", false);
	cmd.add( sProc );
	
	SwitchArg sFast("f","fast","Scan in fast mode", true);
	cmd.add( sFast );
	
	
	
	UnlabeledMultiArg<string> mFiles("fileName","file names",false,
					                "fileNameString");
	cmd.add( mFiles );
	
	// Parse the args.
	cmd.parse( argc, argv );

	doFastScan = sFast.getValue();
	doStartupScreen = sStartup.getValue();
	doRecurse = sRecurse.getValue();
	doScanProcesses = sProc.getValue();
	fileList = mFiles.getValue();
    
 if( ( ! fileList.size() ) && ( ! doScanProcesses ) ){
        MessageBox(NULL , "Nothing to do\nError you must specify file(s) to scan or use -p to scan processes\n" , "Mthwakazi Defender | Shell Scanner !" , 16 );
        return 2;
    }
    	
if(doStartupScreen){
 StartUPWindow stw;
 StartUp();    
 do{
    _sleep(1);                         
}while(stw.isActive() ); 

}else{
printf("Starting Engine...\t ");
StartUp();  
printf(" Done ! \n");
}

ShellScan( fileList , doFastScan , doScanProcesses );
	
} catch (...){
   MessageBox(NULL , "Exception caught while processing command line arguments" , "Mthwakazi Defender | TCLAP Exception " , 16 );
   FatalExit( 7  );
   }
   
return 0;   
}
