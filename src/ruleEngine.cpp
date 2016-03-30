/*
Name: KSD_Hash Signature Engine

Author: Trevor Sibanda

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


*/

#ifdef KSD_RULE_ENGINE

extern "C"{
#ifndef YR_FILEMAP_H
#define YR_FILEMAP_H

#ifdef WIN32
#include <windows.h>
#define FILE_DESCRIPTOR         HANDLE
#else
#define FILE_DESCRIPTOR         int
#endif

#include <stdlib.h>
#include <stdint.h>


typedef struct _MAPPED_FILE
{
  FILE_DESCRIPTOR     file;
  size_t              size;
  uint8_t*            data;
  #ifdef WIN32
  HANDLE              mapping;
  #endif

} MAPPED_FILE;


int yr_filemap_map(
    const char* file_path,
    MAPPED_FILE* pmapped_file);

void yr_filemap_unmap(
    MAPPED_FILE* pmapped_file);

#endif

};

//init g_vHashEngineDetected
std::vector<SROB> RuleScannerEngine::vHashEngineDetected;

ScanObject* RuleScannerEngine::refCurrObj;

int RuleScannerEngine::singleTone = 0;

//@hack
//use this to stop scanning a file once a match has been found
//NB: Currently not thread safe
bool RuleScannerEngine::isInScanLoop = false;


//default scanner callback 
static int RuleEngineCallback(int message, YR_RULE* rule, void* data){

//message that finished scanning
if( message == 3 )return CALLBACK_CONTINUE;
//message that the rule does not match
if( message == 2 )return CALLBACK_CONTINUE; 
SROB tmpSrob;
tmpSrob.filePath = RuleScannerEngine::refCurrObj->GetFilename();
tmpSrob.fileSize = RuleScannerEngine::refCurrObj->GetFileSize();
tmpSrob.fileAttributes = (RuleScannerEngine::refCurrObj->GetFileAttributes()).dwAttributes; 
tmpSrob.signature =  RuleScannerEngine::refCurrObj->GetHashSignature();	
tmpSrob.malwareName = std::string( rule->identifier );
tmpSrob.actionToTake = 0;//default

RuleScannerEngine::vHashEngineDetected.push_back( tmpSrob );
//break out of scan loop
RuleScannerEngine::isInScanLoop = false;
return CALLBACK_CONTINUE;

}
	   




	//constructor by the filename and the timeout
	RuleScannerEngine::RuleScannerEngine( std::vector<std::string> _ruleFiles , int _timeout  ){
	
	
	if( this->singleTone == 1)
	{
	MessageBox(NULL , "There can only be one instance of a rule based scanner per process" , "Internal Error [Singletone]" , 16);
	FatalExit(0);
	}
	yr_initialize();
	this->ruleFiles = _ruleFiles;
	this->timeout = _timeout;
	this->singleTone++;
	this->signatureCount  = 0;
	}
	
	//dtor
	//WARNING: This function will also clear any found
	//malware matches. Watch your scope
	RuleScannerEngine::~RuleScannerEngine(){
	this->DestroyEngine();
	this->ruleFiles.clear();
	this->vHashEngineDetected.clear();	
	this->singleTone--;
	delete[] this->szErrMsg; 
	
	}
	
	//add a rule file
	//must be called before start engine
	//in future can be called later :)
	void RuleScannerEngine::AddRuleFile( std::string _file ){
		for(int x = 0; x < this->ruleFiles.size() ; x++ )
		if( _file == this->ruleFiles[x])return ;
		
		this->ruleFiles.push_back( _file );
	}
	
	//db file
	std::vector<std::string> RuleScannerEngine::GetRuleFiles()const{
		return this->ruleFiles;
	}
	
	int RuleScannerEngine::GetTimeOut()const{
		return this->timeout;
	}
	
	void RuleScannerEngine::SetTimeOut(int _timeout){
		this->timeout = _timeout;
	}
	
	bool RuleScannerEngine::StartEngine(){
	
	//error checking is done in that function, OpenRuleFile
	for( int x = 0; x < this->ruleFiles.size() ; x++ )
    this->OpenRuleFile( this->ruleFiles[x] );
	//simply load all the rule files
	return true;
	
	}
	
	bool RuleScannerEngine::DestroyEngine(){
	
		for_each( this->rules.begin() , this->rules.end() ,  yr_rules_destroy );
		yr_finalize();
		return true;	
	}
	
	//was the engine started successfully
	bool RuleScannerEngine::IsEngineReady(){
		return (bool )(  this->rules.size() );
	}
	
	//send a file to the scanner by ScanObect 
	bool RuleScannerEngine::Scan(  ScanObject& ref){
         //we only need one sample
   //first check file size
  
   //set object to temp
   this->refCurrObj = &ref;
   int size = this->rules.size();
   this->isInScanLoop = true; //scan loop started
   //only thread can break out of scan loop when it detects a file
   
  MAPPED_FILE mfile;
  int result;

  result = yr_filemap_map(ref.GetFilename().c_str(), &mfile);

   if (result != ERROR_SUCCESS)
  {
  this->isInScanLoop = false;
  //@todo
  //add more error processing, to get reason why filemap failed
  return false;
}

for( int x = 0; this->isInScanLoop && x < size ; x++ ){
   
 
  
    this->result = yr_rules_scan_mem(
        rules[x],
        mfile.data,
        mfile.size,
        RuleEngineCallback ,
        NULL,
        this->fastScan,
        this->timeout);

   
 	
		if( this->result   )
		{
        //check if not funny, unnecessary errors
        //lookup values in file yara.h
		if(! ( this->result == 30 || this->result == 3 || this->result == 5 || this->result == 6 || this->result == 7 ) ){    
		delete[] this->szErrMsg;
		this->szErrMsg = new char[2048];
		sprintf(this->szErrMsg , "R-Scanner has encountered a very serious problem and has to be terminated. Please report this error to the developers.\n\nError Code is rScan %d \n\n Program will now terminate" , this->result);
			MessageBox(NULL, this->szErrMsg , "Fatal Error"  , 16 );
			FatalExit(0);
			return false;
      }
        }
	
}
	yr_filemap_unmap(&mfile);
    this->isInScanLoop = false;
	return true;
    }
	
	void RuleScannerEngine::SetFastScan( bool _b ){
	this->fastScan = _b;
	}
	
	bool RuleScannerEngine::GetFastScan( )const{
	return this->fastScan;
	}
	
	//return a vector of detected scanobjects
	std::vector<SROB> RuleScannerEngine::GetDetected(){
		return vHashEngineDetected;
	}
	
    int RuleScannerEngine::CountDetected(){
		return (int)( vHashEngineDetected.size()  );
	}
	
	 std::string RuleScannerEngine::GetLastError(){
	//@todo Implement function
		return std::string("[Function not implemented]");
	}
	
	void RuleScannerEngine::AddDetection( ScanObject  _s ){
		MessageBox(NULL , "You cannot manipulate the Scanner Engine From Here. This is Reserved for Future Versions. Thank You" , "Operation Denied" , 16);
		return;
	}
	
	
	bool RuleScannerEngine::OpenRuleFile(std::string filename  ){
	YR_RULES* tmp;
	this->result = yr_rules_load( filename.c_str() , &tmp );
	  switch( this->result ){
	  case 0:
	  //all good
	  break;
	  case 1: //not enough memory
	  this->DestroyEngine();
	  MessageBox(NULL , "The rule engine failed to start because the system ran out of memory. \nPlease make sure you have enough memory on your system\n\nProgram will now terminate" , "Insufficient memory" , 16);
	  FatalExit(999);
	  break;
	default:
{	
char* errmsg = new char[1024];
sprintf(errmsg , "The r engine failed to start due to an unknown error.\nThis may be due to a corrupt virus definition file, missing vdf or read/write access rights.\nTry reinstalling the program.\n\nThe error code is %d \n\nProgram will now exit." , this->result );
	  MessageBox(NULL , errmsg ,  "Missing Virus Definition File" , 16 );
	  this->DestroyEngine();
	  delete[] errmsg;
	  FatalExit(0);
      return false; //failed to open database
 
}
break;

	}
    //pushback
	this->rules.push_back(  tmp );
	return true;
	}
	
	
#endif

