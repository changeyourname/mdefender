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

#ifndef KSD_AV_HASH_ENGINE

#define KSD_AV_HASH_ENGINE

class ScanObject;

#include "scan_object.hpp"

#include "scan_result_object.h"


#include <windows.h>

 //Need SQLITE
#ifndef _SQLITE3_H_

#include <sqlite3.h>

#endif 

 
#include <string>
#include <vector>

//default scanner callback 
static int HashEngineCallback(void *NotUsed, int argc, char **argv, char **azColName);


class HashScannerEngine{
public:

	HashScannerEngine(){
	HashScannerEngine("" , 500);
	}
	
	//constructor by the filename and the timeout
	HashScannerEngine( std::string dbFile , int _timeout  );
	
	//dtor
	~HashScannerEngine();
	
	//set the database file
	void SetDbFile( std::string );
	
	//db file
	std::string GetDbFile()const;
	
	int GetTimeOut()const;
	
	void SetTimeOut(int );
	
	bool StartEngine();
	
	bool DestroyEngine();
	
	//was the engine started successfully
	bool IsEngineReady();
	
	//send a file to the scanner by ScanObect 
	bool Scan(  ScanObject& );
	
	//return a vector of detected scanobjects
	static std::vector<SROB> GetDetected();
	
	static int CountDetected();
	
	static std::string GetLastError();
	
	static void AddDetection( ScanObject );
	
	static std::vector<SROB> vHashEngineDetected; //scanresultobject
	
	static int singleTone;
	
	//current file being scanned
	static ScanObject* refCurrObj;
	
private:
	std::string filename;
	int timeout;
	sqlite3* dbConn;
	char* szErrMsg;
	int result;
	std::string sql;
	
};
	
#endif

