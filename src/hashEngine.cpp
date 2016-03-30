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

Description: 

*/

#ifdef KSD_AV_HASH_ENGINE


//init g_vHashEngineDetected
std::vector<SROB> HashScannerEngine::vHashEngineDetected;

ScanObject* HashScannerEngine::refCurrObj;

int HashScannerEngine::singleTone = 0;

//default scanner callback 
static int HashEngineCallback(void *NotUsed, int argc, char **argv, char **azColName)
{

//message that finished scanning

SROB tmpSrob;
tmpSrob.filePath = HashScannerEngine::refCurrObj->GetFilename();
tmpSrob.fileSize = HashScannerEngine::refCurrObj->GetFileSize();
tmpSrob.fileAttributes = (HashScannerEngine::refCurrObj->GetFileAttributes()).dwAttributes; 
tmpSrob.signature =  HashScannerEngine::refCurrObj->GetHashSignature();	

int i;
	for(i=0; i<argc; i++)
	{
		if( std::string(azColName[i]) == "name"){
		tmpSrob.malwareName = std::string( argv[i] );
		continue;
		}
		if( std::string(azColName[i]) == "detection"){
		tmpSrob.actionToTake = 0; //default
		continue;
		}
		if( std::string(azColName[i]) == "signature"){
		tmpSrob.signature = std::string( argv[i] );
		continue;
		}
		
	
	}


tmpSrob.actionToTake = 0;//default
HashScannerEngine::vHashEngineDetected.push_back( tmpSrob );
return 0;

}
	   




	//constructor by the filename and the timeout
	HashScannerEngine::HashScannerEngine( std::string dbFile , int _timeout  )
	{
	
	if( this->singleTone == 1)
	{
	MessageBox(NULL , "There can only be one instance of a hash based scanner per process" , "Internal Error [Singletone]" , 16);
	FatalExit(0);
	}
	this->dbConn = 0;
	this->filename = dbFile;
	this->timeout = _timeout;
	this->singleTone++;
	
	}
	
	//dtor
	//WARNING: This function will also clear any found
	//malware matches. Watch your scope
	HashScannerEngine::~HashScannerEngine(){
	this->DestroyEngine();
	this->filename.clear();
	this->vHashEngineDetected.clear();	
	delete[] this->szErrMsg; 
	
	}
	
	//set the database file
	//must be caled before start engine
	void HashScannerEngine::SetDbFile( std::string _file ){
		this->filename =  _file;
	}
	
	//db file
	std::string HashScannerEngine::GetDbFile()const{
		return this->filename;
	}
	
	int HashScannerEngine::GetTimeOut()const{
		return this->timeout;
	}
	
	void HashScannerEngine::SetTimeOut(int _timeout){
		this->timeout = _timeout;
	}
	
	bool HashScannerEngine::StartEngine(){
	this->result = sqlite3_open_v2(this->filename.c_str() , &this->dbConn , SQLITE_OPEN_READONLY , NULL );

if(this->result != SQLITE_OK ){	
char* errmsg = new char[1024];
sprintf(errmsg , "The hash engine failed to start due to an unknown error.\nThis may be due to a corrupt virus definition file, missing vdf or read/write access rights.\nTry reinstalling the program.\n\nThe error code is %d \n\nProgram will now exit." , this->result);
	  MessageBox(NULL , errmsg ,  "Startup error" , 16 );
	  this->DestroyEngine();
	  delete[] errmsg;
	  FatalExit(0);
      return false; //failed to open database
 
}

	
	return true;
	}
	
	bool HashScannerEngine::DestroyEngine(){
		if( this->dbConn )
		sqlite3_close(this->dbConn);
		this->dbConn = 0;
		return true;	
	}
	
	//was the engine started successfully
	bool HashScannerEngine::IsEngineReady(){
		return (bool )(  this->dbConn == 0  );
	}
	
	//send a file to the scanner by ScanObect 
	bool HashScannerEngine::Scan(  ScanObject& ref ){
         //we only need one sample
   //check file size
   
   
   //set object to temp
   this->refCurrObj = &ref;
   this->sql = "SELECT  signature ,name , detection FROM malware WHERE ( signature = '"+ ref.GetHashSignature() + "' ) LIMIT 1 ";
   this->result =  sqlite3_exec( this->dbConn , this->sql.c_str()  , HashEngineCallback , NULL , &this->szErrMsg);
		
		if( this->result != SQLITE_OK )
		{
		//delete[] this->szErrMsg;
		//this->szErrMsg = new char[2048];
		//sprintf(this->szErrMsg , "H-Scanner has encountered a very serious problem and has to be terminated. Please report this error to the developers.\n\nError Code is hScan %d \n\n Program will now terminate" , this->result);
		fprintf(stderr, "SQL error: %s\n", this->szErrMsg);
		sqlite3_free(this->szErrMsg);		
		MessageBox(NULL, this->szErrMsg , "Fatal Error"  , 16 );
			FatalExit(0);
			return false;
		}
    return true;
    }
	
	
	
	//return a vector of detected scanobjects
	std::vector<SROB> HashScannerEngine::GetDetected(){
		return vHashEngineDetected;
	}
	
    int HashScannerEngine::CountDetected(){
		return (int)( vHashEngineDetected.size()  );
	}
	
	 std::string HashScannerEngine::GetLastError(){
	//@todo Implement function
		return std::string("[Function not implemented]");
	}
	
	void HashScannerEngine::AddDetection( ScanObject  _s ){
		MessageBox(NULL , "You cannot manipulate the Scanner Engine From Here. This is Reserved for Future Versions. Thank You" , "Operation Denied" , 16);
		return;
	}
	
	
#endif

