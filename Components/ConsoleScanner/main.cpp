/****************************************************
Author: Trevor Sibanda

Name: Mthwakazi Defender Console Scanner

Description:
            
License:
        
****************************************************/

#include <ctime>                    
#include "../../include/engine.hpp"
#include "../../src/engine.cpp"
#include <string>
#include <algorithm>
#include "../../include/tclap/CmdLine.h"

#define CONSOLE_SCANNER_VERSION "1.0.0.0"


using namespace TCLAP;
using namespace std;

char* izaga[] = {
                "Indlela ibuzwa kwabaphambili" , 
                "Umuntu ngumuntu ngabantu" , 
                "Hamba juba bazakucutha phambili",
                "Inkiwane elibomvu libolile",
                "IBosso yiBosso !",
                "Isala kutshelwa sibona ngmopho !"
                
                };

void PrintHeader( ){
     printf(" Mthwakazi Defender %s Console Scanner\n" , CONSOLE_SCANNER_VERSION );
     srand( time(NULL) );
     int rndm = rand()%6;
     printf("->\n->\tby ZIMCODERZ (c) 2014 All Rights Reserved\n->\t%s\n->\n" , izaga[rndm]  );
     printf("->\tBug reports: \t Trevor Sibanda <mthwakazidefender@gmail.com>\n");
     
     }
     
int ProcessDetection(SROB* pS){
if( ! pS )return 2;
printf("\nDetection: %s \t %s", pS->malwareName.c_str() , pS->filePath.c_str() );
printf("\nAction: [0] Delete [1] Do Nothing [2] Skip [3] Exit \n");
printf("ACTION: ");
char act;
cin>>act;
if( act == '0' ){
 return 0;    
}else if(act == '1' ){
return 1;
}else if(act == '2' ){
return 2;
}else{
 return 3;     
}      
     
}      

int main(int argc, char** argv)
{
	bool doDeep = false;
    bool doInteractive = true;
    std::vector< std::string > fileList;
    bool doRecurse = true;
    bool doScanProcesses = true;
    bool doFastScan = true;
    vector<ScanObject> files;
    vector<SROB> scanResults;
    
    
    PrintHeader(); 
	try {  

	// Define the command line object.
	CmdLine cmd("Mthwakazi Defender Console Scanner", ' ', "1.0.0");


	SwitchArg sInteractive("i","interactive","Interactive file scanner. Overrides delete", false);
	cmd.add( sInteractive );
	
	SwitchArg sRecurse("r","recursive","Recursively scan subfolders", false);
	cmd.add( sRecurse );

    SwitchArg sProc("p","scan-processes","Scan all running processes", false);
	cmd.add( sProc );
	
	SwitchArg sFast("f","fast","Scan in fast mode", false);
	cmd.add( sFast );
	
	SwitchArg sDeep("d","deep","Deep scan. Also uses malware hassh based signature. Slower !", false);
	cmd.add( sDeep );
	
	UnlabeledMultiArg<string> mFiles("fileName","file names",false,
					                "fileNameString");
	cmd.add( mFiles );
	
	// Parse the args.
	cmd.parse( argc, argv );

	doFastScan = sFast.getValue();
	doDeep = sDeep.getValue();
	doInteractive = sInteractive.getValue();
	doRecurse = sRecurse.getValue();
	doScanProcesses = sProc.getValue();
	fileList = mFiles.getValue();
    
    if( doDeep ){
    printf("->\tNOTICE: \t Deep Scan mode enable. Scanner will be slower. You have been warned\n");
     
     }
    if( ( ! fileList.size() ) && ( ! doScanProcesses ) ){
        printf("\tError you must specify file(s) to scan or use -p to scan processes\n");
        return 2;
        }
        
   
    FileSystem* fs = new FileSystem();
    SystemEnvironment* sysEnv = new SystemEnvironment;
    SignatureInfo *si = new SignatureInfo();
    vector<ScanObject> files;
    ProcessManager pMan;
    ConfigReader* cfgReader = 
    new ConfigReader(sysEnv->GetExecutableDirectory() + "\\data\\config.bin" , false );
    
    
    if( ! cfgReader->Open() ){
        printf("[Error] Failed to open configuration file\n->\t%s\\data\\config.bin\n" , sysEnv->GetExecutableDirectory().c_str() );
        return 1;
    } 
    
    std::string exts;
    cfgReader->Get("scan_ext" , exts);
    //now 
    std::string delimiter = ",";

    size_t pos = 0;
    std::string token;
    while ((pos = exts.find(delimiter)) != std::string::npos) {
    token = exts.substr(0, pos);
    fs->AddExtension(token);
    exts.erase(0, pos + delimiter.length());
    }
    
    //extract rule files

std::vector<std::string> ruleFiles;
{
 std::string files;
    assert(  cfgReader->Get("rule_files" , files) );
    //now 
    std::string delimiter = ";";

    size_t pos = 0;
    std::string token;
    while ((pos = files.find(delimiter)) != std::string::npos) {
    token = files.substr(0, pos);
    ruleFiles.push_back( sysEnv->GetExecutableDirectory() + "\\data\\" + token);
    files.erase(0, pos + delimiter.length());
    }              
              
}          
     
    string filesScanned;
    cfgReader->Get("scanned_files" , filesScanned ); 
    printf("->\tScanned Files:\t %s \n" , filesScanned.c_str() );
    HashScannerEngine hse(sysEnv->GetExecutableDirectory() + "\\data\\vdf.bin" , 2500 ) ;
    hse.SetDbFile(sysEnv->GetExecutableDirectory() + "\\data\\vdf.bin");
    hse.SetTimeOut(2500 );

    RuleScannerEngine rse( ruleFiles , 1000 ); 
    time_t startTime = time( NULL );
    printf("\n->\tStarting Engine..."); 
    if( doDeep ){
//disables the hse engine if not scanning in deep mode
hse.StartEngine();
    
} 
    rse.StartEngine();
    int loadTime = time(NULL) - startTime ;       
    printf("\t done [ %d s ] \n" , loadTime);
	
	if( doScanProcesses ){
	printf("->\tScanning  Processes\n");
    
//now print out all processes
vector<Process> ps = pMan.ListAllProcesses();

int psCounter = 0;
for( int x = 0; x < ps.size() ; x++ ){
 printf("\t+\tPID: %d    %s\n" , ps[x].pid, ps[x].exeName.c_str() ); 
 //now print out loaded modules
 for(int y = 0; y < ps[x].loadedModules.size() ; y++ ){
         ScanObject obj( ps[x].loadedModules[y] );
         fs->GetScanObject( ps[x].loadedModules[y] , &obj);
         fs->GetFileAttributes( &obj );
         if( doDeep ){
         fs->GenerateHashSignature( &obj);
         hse.Scan( obj );
         }
         
         rse.Scan( obj );  
         
         }   
     
} 

}
//enumerating files to scan
if( fileList.size() )
printf("->\tEnumerating files ... ");    
   {
long fileCount;    
Enum_Folder_Info fi = fs->GetDirectoryScanObjects(fileList , files , &fileCount , doRecurse);
if( files.size() ){
  
char* msg = new char[1024];
sprintf( msg , "Found %d files in %d sub directories " , fi.fileCount , fi.subFolderCount );
printf(" done\n->\t %s \n", msg );
    delete[] msg;
    } 
    


printf("\n\n->\tFileSystem Scanner Running ... \n");
int size = files.size();

int step = ( size / 100 );    
    for( int x = 0; x < size  ; x++ ){
         
     
       int percent = ( (x * 100)/(size) );
       printf("\n\t+\t%d %% \%\t %d detected \t %s \n", 
       percent , 
       ( hse.CountDetected() + rse.CountDetected() ) , 
       files[x].GetFilename().c_str()
       );
        
     
       
    
    rse.Scan( files[x] );   
    if( doDeep )
    if ( fs->GenerateHashSignature( &files[x] ) ) 
    hse.Scan(files[x] );
       
}   
printf("\n\n->\t100%\tFileSystem Scanner Finished\n");

  {
   
   for( int x = 0 ; x < HashScannerEngine::vHashEngineDetected.size() ; x++ )
   scanResults.push_back( HashScannerEngine::vHashEngineDetected[x] );
   
   for( int x = 0 ; x < RuleScannerEngine::vHashEngineDetected.size() ; x++ )
   scanResults.push_back( RuleScannerEngine::vHashEngineDetected[x] );
   
  HashScannerEngine::vHashEngineDetected.clear();
  RuleScannerEngine::vHashEngineDetected.clear();                              
                                 
   
   }
   
}
    
printf("\n\n->\tDetected %d possible infections ! \n" , ( scanResults.size() ) );

//add to database 
int newScanned = 0;
cfgReader->Get("scanned_files" , newScanned );
newScanned += scanResults.size();
cfgReader->Set("scanned_files" , newScanned  );

if( doInteractive ){
printf("\n->\tInteractive Malware Removal \n");
 
for(int x = 0; x < scanResults.size() ; x++ )
        switch( ProcessDetection(&scanResults[x] ) ){
                case 0:
                     {
                      printf("Are you sure ? [y|n] ");
                      char r;
                      cin>>r;
                      if( r == 'y' ){
                          if( fs->DeleteFile( scanResults[x].filePath ) )
                          printf("File deleted \n");
                          else
                          printf(" File not deleted. Ignored  ! \n"); 
                          }                            
                      }
                      break;
                      case 1:
                           printf("Warning ! File was ignored ! \n");
                      break;
                      case 2:
                           cout<<" File Skipped !!!\n";
                      break;
                      case 3:
                           {
                      printf("Are you sure ? [y|n] ");
                      char r;
                      cin>>r;
                      if( r == 'y' ){
                         x = 10000000 ; //just set x to very large number
                         break;
                         }                            
                      }  
                                       
                      }
                      

}else{
      
printf("\n->\tNo files can be deleted in non-interactive mode. All files ignored\n");      
}

	} catch (ArgException &e)  // catch any exceptions
	{ printf("error: %s for arg %s \n", e.error().c_str() , e.argId() ); }
}



