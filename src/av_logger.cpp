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
#include "../src/av_logger.hpp"
#endif
 
 #ifdef KSD_AV_LOGGER
 

// #include <boost/assert.hpp>
 
 #include <ctime>
 #include <cstdlib>
 

//Constructor, pass full log file name
AV_Logger::AV_Logger(std::string& filename ):
file(0), fname(filename) ,isOpen( false),lastMsg(""),lastMsgType(0),logCounter(0){
	
	
}

//Destructor
AV_Logger::~AV_Logger(){
	this->fname.clear();
	this->lastMsg.clear();
	if( this->file ){
	fclose(this->file);
	}
	this->logCounter = 0;
	this->isOpen = false;
}

//Opens the Log File, Returns true on Success, Failure = False
bool AV_Logger::Open(){

if( this->file != NULL ){
this->isOpen = true;
return true;
}

this->file = fopen(this->fname.c_str()  ,  "a" );
if( this->file != NULL ){
this->isOpen = true;
}else{
this->isOpen = false;
}
return this->isOpen;

}

//Check if Log File is Open
bool AV_Logger::CheckIsOpen(){
	if( this->file == NULL){
	this->isOpen = false;
	}else{
	this->isOpen = true;
	}
	return this->isOpen;
	
}



//Write String to Log , Type of log must be specified. Default is INFO
bool AV_Logger::Write( std::string&  _msg, unsigned int _type){
std::string msg;
std::string type;

this->lastMsgType = _type;

switch(_type){
case AV_LOGGER_INFO:
type = "INFO";
break;
case AV_LOGGER_ERROR:
type = "ERROR";
break;
case AV_LOGGER_WARN:
type = "WARNING";
break;
case AV_LOGGER_FATAL:
type = "FATAL";
break;
default:
type = "NOTICE";
break;
}


 time_t rawtime;
 struct tm * timeinfo;
 char buffer [80];

 time (&rawtime);
 timeinfo = localtime (&rawtime);

 strftime (buffer,80,"%c",timeinfo);
std::string c_time = std::string( buffer );

  msg = c_time+ "\t[" + type + "] "+ _msg + "\r\n";//use \r\n for windows compatibility
  if( this->file ){
	int len = msg.length()+1;
	fprintf( this->file , msg.c_str() );
	this->lastMsg = msg;
	this->logCounter++;
	return 1;
  }	
  return 0;
}



//Write String to Log with custom type
bool AV_Logger::Write(std::string& msg, std::string& type){
//@todo INCLUDE CUSTOM SUPPORT
return this->Write( msg , 9 );//just write notice for now
}

//Get Last Log Message Written
std::string AV_Logger::GetLastWritten()const{
return this->lastMsg;
}

//Get Last Log Message Type
int AV_Logger::GetLastWrittenType()const{
return this->lastMsgType;
}

//Log error message
void AV_Logger::Error( std::string&  m){
this->Write(m , AV_LOGGER_ERROR);
}

//Log INFO message
void AV_Logger::Info(std::string& m){
this->Write( m , AV_LOGGER_INFO);
}

//Log Warning message
void AV_Logger::Warning( std::string& m){
this->Write(m , AV_LOGGER_WARN );
}

//Log Fatal message
void AV_Logger::Fatal( std::string& m ){
this->Write(m , AV_LOGGER_FATAL);
}

//Get The Size of the Log File
long AV_Logger::GetLogSize()const{
//@todo Make it work
return 0;
}

//Get number of logs written this session
long AV_Logger::GetLogCount()const{
return this->logCounter;
}

//Close the log file.
bool AV_Logger::Close(){
if(this->file)
fclose(this->file);
return true;
}


 
#endif 


