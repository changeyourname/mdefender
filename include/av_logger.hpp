/*
Name: KSD_AV_LOGGER

Description: Simple logger 

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

Date: 13 March 2014

Author: Trevor Sibanda

Email: trevorsibb@gmail.com

*/

 
 #ifndef KSD_AV_LOGGER
 
 #define KSD_AV_LOGGER
 
 #include <stdio.h>
 #include <vector>
 #include <string>
 
 //Following constants are used when interfacing with AV_Logger::Write directly
 #define AV_LOGGER_INFO 	0
 #define AV_LOGGER_WARN 	1
 #define AV_LOGGER_ERROR 	2
 #define AV_LOGGER_FATAL	3
 
 
class AV_Logger{
public:
	//Constructor, pass full log file name
	AV_Logger(std::string& filename );
	
	//Destructor
	~AV_Logger();
	
	//Opens the Log File, Returns true on Success, Failure = False
	bool Open();
	
	//Check if Log File is Open
	bool CheckIsOpen();
	
	
	//Write String to Log , Type of log must be specified. Default is INFO
	bool Write( std::string&  , unsigned int);
	
	//Write String to Log with custom type
	bool Write(std::string& , std::string& );
	
	//Get Last Log Message Written
	std::string GetLastWritten()const;
	
	//Get Last Log Message Type
	int GetLastWrittenType()const;
	
	//Log error message
	void Error( std::string&  );
	
	//Log INFO message
	void Info(std::string& );
	
	//Log Warning message
	void Warning( std::string& );
	
	//Log Fatal message
	void Fatal( std::string& );
	
	//Get The Size of the Log File
	long GetLogSize()const;
	
	//Get number of logs written this session
	long GetLogCount()const;
	
	//Close the log file.
	bool Close();

private:
	
	FILE* file;
	bool isOpen;
	std::string lastMsg;
	unsigned int lastMsgType;
	unsigned int logCounter;
	std::string fname;
protected:
		

};	
 
 
#endif 
