/*
Name: KSD_AV_RULE_SCANNER 

Description: Interface to C rule based scanner.

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


Date: 11 April 2014

Author: Trevor Sibanda

Email: trevorsibb@gmail.com

*/

 #ifndef KSD_RULE_ENGINE
 
 #define KSD_RULE_ENGINE
 
 
 
class ScanObject;

#include "scan_object.hpp"

#include "scan_result_object.h"


#include <windows.h>

 //Need yara
 
#ifndef YR_YARA_H

#include "yara/yara.h"

#endif 

 
#include <string>
#include <vector>

//default scanner callback 
static int RuleEngineCallback(int message, YR_RULE* rule, void* data);


class RuleScannerEngine{
public:


	//constructor by the filename and the timeout
	RuleScannerEngine( std::vector<std::string> ruleFiles , int _timeout  );
	
	//dtor
	~RuleScannerEngine();
	
	//set the database file
	void AddRuleFile( std::string );
	
	void SetFastScan( bool );
	
	bool GetFastScan( )const;
	
	//db file
	std::vector<std::string> GetRuleFiles()const;
	
	int GetTimeOut()const;
	
	void SetTimeOut(int );
	
	bool StartEngine();
	
	bool DestroyEngine();
	
	//was the engine started successfully
	bool IsEngineReady();
	
	std::vector<YR_RULES*> GetRules(){ return this->rules; }
	
	//send a file to the scanner by ScanObect 
	bool Scan(  ScanObject& );
	
	
	//return number of loaded signatures
	//called after all databases are loaded
	int GetSignatureCount( )const;
	
	//return a vector of detected scanobjects
	static std::vector<SROB> GetDetected();
	
	static int CountDetected();
	
	static std::string GetLastError();
	
	static void AddDetection( ScanObject );
	
	static std::vector<SROB> vHashEngineDetected; //scanresultobject
	
	static int singleTone;
	
	//current file being scanned
	static ScanObject* refCurrObj;
	
	static bool isInScanLoop;
	
	
private:
	int timeout;
	int signatureCount;
	std::vector<std::string> ruleFiles;
	std::vector<YR_RULES*> rules;
	char* szErrMsg;
	int result;
	bool fastScan;
protected:
	bool OpenRuleFile(std::string  );
	
};
	
 
#endif 
  




