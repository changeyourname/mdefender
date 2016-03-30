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

 
 #ifndef KSD_CONFIG_READER
 
 
 
 
 #define KSD_CONFIG_READER
 
 //Need SQLITE
#ifndef _SQLITE3_H_

#include <sqlite3.h>

#endif 
 
 #include <string>
 

 class ConfigReader{
 public:
 
 //Constructor. Filename , ReadOnly 
 ConfigReader( std::string  , bool  );
 
 //Close database connection and free meory
 ~ConfigReader( );
 
 //Open a connection to a database specified by filename 
 //passed to constructor
 bool Open( );
 
 //Close the database 
 bool Close();
 
 //Reload database
 bool Flush();

 //Check if a database connection is open and valid
bool IsOpen();
 
 //Alias for Flush
 bool Reload( ){ return this->Flush(); }

 //Get a key value. Returns true on success
 
bool Get( std::string _key , std::string& );

//Get a integer value from a  key
bool Get( std::string  _key , int& );

//Get a float value from a key
bool Get( std::string _key , float& );

//Set the new string value to a key
bool Set( std::string _key , std::string _value );

//Set an int value to a key
bool Set( std::string _key , int _value );

//Set a float value to a key
bool Set( std::string _key , float _value );

//Add a new key value into the database
bool AddKey( std::string _key );

//Get sql dabatase object
sqlite3* GetSQLConnection()const{ return this->sqliteConn;  }

//GetFileName of database file
std::string GetFileName( )const{ return this->fName; }

//Is the database opened n read only mode
bool IsReadOnly()const{ return this->isReadOnly; }

//Set the database filename.
//Please note that this method requires the databae to be reopened
//this can be a call to Reload , Flush or Close then Open
void SetFileName(  std::string  _filename ){ this->fName =  _filename; }


//Convert different types to string
std::string toString( int );

std::string toString( float ); 

protected:

sqlite3* sqliteConn;
//is the database supposed to be opened in read only mode
bool isReadOnly;  
 //filename
 std::string fName;
 
 };
 
 
 
#endif 
