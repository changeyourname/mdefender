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

 
 #ifdef KSD_CONFIG_READER
 
 
 //Constructor. Filename , ReadOnly 
 ConfigReader::ConfigReader( std::string  _filename , bool  _readOnly ):
			fName(_filename) , isReadOnly(_readOnly)  {
			//not much to do
		}
 
 //Close database connection and free meory
 ConfigReader::~ConfigReader( ){
		if( this->IsOpen() )
		this->Close();
		
		this->fName.clear();
		this->sqliteConn = 0;
		}
 
 //Open a connection to a database specified by filename 
 //passed to constructor
 bool ConfigReader::Open( ){
		//code to open database
	int	 rc = sqlite3_open(this->fName.c_str() , &this->sqliteConn );
    if( rc ){
      sqlite3_close(this->sqliteConn);
      return false; //failed to open database
    }
		return true;
		}
 
 //Close the database 
 bool ConfigReader::Close(){
		sqlite3_close( this->sqliteConn  );
		return true;
		}
 
 //Reload database
 bool ConfigReader::Flush(){
		this->Close();
		return this->Open();
		}

 //Check if a database connection is open and valid
bool ConfigReader::IsOpen(){
	if( !  this->sqliteConn )return false;
	return true;
		}
 


 //Get a key value. Returns true on success
 
bool ConfigReader::Get( std::string _key , std::string&  ref){
	if(  _key == ""  ){
	return false;
	}
	std::string sqlStatement = "SELECT  value FROM config  WHERE   key = \"" + _key + "\" " ;
	 sqlite3_stmt *ppStmt;
     
    int rc = sqlite3_prepare( this->sqliteConn , sqlStatement.c_str() , sqlStatement.length() , &ppStmt , NULL );  
   
    if( rc!= SQLITE_OK ){
      return false;
    }
	//Obtaining output is not really necessary of yet
	char* zErrMsg = 0;
	rc = sqlite3_exec(this->sqliteConn , sqlStatement.c_str()  , NULL , 0, &zErrMsg);
	if( rc!= SQLITE_OK ){
//not really interested in the nitty details of the cause of each failure
//not YET 
	delete[] zErrMsg;    
	return false;
    }
	int s = sqlite3_step (ppStmt);
            if (s == SQLITE_ROW) {
                int bytes;
                const unsigned char * text;
                bytes = sqlite3_column_bytes(ppStmt, 0);
                text  = sqlite3_column_text (ppStmt, 0);
               ref = std::string( (char*) text );
            }
            else if (s == SQLITE_DONE) {
                return false; //empty set. kinda wierd
            }
	delete[] zErrMsg; 		
	//now free statement
	sqlite3_finalize( ppStmt );
	return true;
		}
		

//Get a integer value from a  key
bool ConfigReader::Get( std::string  _key , int&  ref){
		if(  _key == ""  ){
	return false;
	}
	std::string sqlStatement = "SELECT  value FROM config  WHERE   key = \"" + _key + "\" " ;
	 sqlite3_stmt *ppStmt;
     
    int rc = sqlite3_prepare( this->sqliteConn , sqlStatement.c_str() , sqlStatement.length() , &ppStmt , NULL );  
   
    if( rc!= SQLITE_OK ){
      return false;
    }
	//Obtaining output is not really necessary of yet
	char* zErrMsg = 0;
	rc = sqlite3_exec(this->sqliteConn , sqlStatement.c_str()  , NULL , 0, &zErrMsg);
	if( rc!= SQLITE_OK ){
//not really interested in the nitty details of the cause of each failure
//not YET     
	return false;
    }
	int s = sqlite3_step (ppStmt);
            if (s == SQLITE_ROW) {
           
				ref =  sqlite3_column_int( ppStmt , 0 );
                
            }
            else if (s == SQLITE_DONE) {
                return false; //empty set. kinda wierd
            }
	//now free statement
	sqlite3_finalize( ppStmt );
	return true;
		}

//Get a float value from a key
bool ConfigReader::Get( std::string _key , float&  ref){
	//no need for float value yet.. really
	int i = (int)ref;
	return this->Get( _key , i );
		}

//Set the new string value to a key
bool ConfigReader::Set( std::string _key , std::string _value ){
		if(  _key == ""  ){
	return false;
	}
	std::string sqlStatement = "UPDATE config SET value = \"" +  _value  + "\" WHERE   key = \"" + _key +  "\" " ;
	 sqlite3_stmt *ppStmt;
     
    int rc = sqlite3_prepare( this->sqliteConn , sqlStatement.c_str() , sqlStatement.length() , &ppStmt , NULL );  
   
    if( rc!= SQLITE_OK ){
      return false;
    }
	//Obtaining output is not really necessary of yet
	char* zErrMsg = 0;
	rc = sqlite3_exec(this->sqliteConn , sqlStatement.c_str()  , NULL , 0, &zErrMsg);
	if( rc!= SQLITE_OK ){
      return false;
    }
	return true;
		}

//Set an int value to a key
bool ConfigReader::Set( std::string _key , int _value ){
if(  _key == ""  ){
	return false;
	}
	std::string sqlStatement = "UPDATE config SET value = \"" + this->toString( _value ) + "\" WHERE   key = \"" + _key + "\" " ;
	 sqlite3_stmt *ppStmt;
     
    int rc = sqlite3_prepare( this->sqliteConn , sqlStatement.c_str() , sqlStatement.length() , &ppStmt , NULL );  
   
    if( rc!= SQLITE_OK ){
      return false;
    }
	//Obtaining output is not really necessary of yet
	char* zErrMsg = 0;
	rc = sqlite3_exec(this->sqliteConn , sqlStatement.c_str()  , NULL , 0, &zErrMsg);
	if( rc!= SQLITE_OK ){
      return false;
    }
	return true;
		}

//Set a float value to a key
bool ConfigReader::Set( std::string _key , float _value ){
	if(  _key == ""  ){
	return false;
	}
	std::string sqlStatement = "UPDATE config SET value = \"" + this->toString( _value ) + "\" WHERE   key = \"" + _key + "\" " ; 
	 sqlite3_stmt *ppStmt;
     
    int rc = sqlite3_prepare( this->sqliteConn , sqlStatement.c_str() , sqlStatement.length() , &ppStmt , NULL );  
   
    if( rc!= SQLITE_OK ){
      return false;
    }
	//Obtaining output is not really necessary of yet
	char* zErrMsg = 0;
	rc = sqlite3_exec(this->sqliteConn , sqlStatement.c_str()  , NULL , 0, &zErrMsg);
	if( rc!= SQLITE_OK ){
      return false;
    }
	return true;
	}

//Add a new key value into the database
bool ConfigReader::AddKey( std::string _key ){
	if(  _key == ""  ){
	return false;
	}
	std::string sqlStatement = "INSERT INTO config ( key , value ) VALUES (  \"" + _key + "\" , \" 0 \" ) " ;
	 sqlite3_stmt *ppStmt;
     
    int rc = sqlite3_prepare( this->sqliteConn , sqlStatement.c_str() , sqlStatement.length() , &ppStmt , NULL );  
   
    if( rc!= SQLITE_OK ){
      return false;
    }
	//Obtaining output is not really necessary of yet
	char* zErrMsg = 0;
	rc = sqlite3_exec(this->sqliteConn , sqlStatement.c_str()  , NULL , 0, &zErrMsg);
	if( rc!= SQLITE_OK ){
      return false;
    }
	return true;
		}

 
 std::string  ConfigReader::toString(  int _val ){
 char *buffer = new char[25];
itoa(   _val , buffer , 10 ); 
 std::string tmp = std::string( buffer );
 delete[] buffer;
 return tmp;
 }


 std::string  ConfigReader::toString( float _val ){
 char *buffer = new char[25];
ltoa(  (long int) _val , buffer , 10 ); 
 std::string tmp = std::string( buffer );
 delete[] buffer;
 return tmp;
 }
 
 
#endif 
