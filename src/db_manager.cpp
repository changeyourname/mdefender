/*
Name: KSD_AV_DATABASE_MANAGER 

Description: Opens and manages a connections to the sql databases.
			
			

Date: 13 March 2014

Author: Trevor Sibanda

Email: trevorsibb@gmail.com

*/

 
 #ifdef KSD_AV_DATABASE_MANAGER
 

	
	//Constructor
	DatabaseManager::DatabaseManager( DBConnectionConfig ):
		dbConn(0) , tempStmt(0) , dbTimeout(500){
	
	}
	
	DatabaseManager::~DatabaseManager(){
	//close database if open
	if(  this->dbConn )
	this->Close();
	
	
	
	}
	
	//Open database. Returns true on success
	bool DatabaseManager::Open(){
	//must check quite a number of things before connecting
	
	if(
			( this->dbConfig.dbFile == "") ||
			( this->dbConfig.readOnly && (this->dbConfig.createFileIfNonExistant ) ) //readonly, so cant create new file
	){
	return false;
	}
	
	int flags =  SQLITE_OPEN_READWRITE ; //default behaviour
	
	if(  this->dbConfig.readOnly  ){
	flags =  SQLITE_OPEN_READONLY;
	}	
	if(  this->dbConfig.createFileIfNonExistant ){
	flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	}
	
	int res =  sqlite3_open_v2(
   this->dbConfig.dbFile.c_str()  ,   /* Database filename (UTF-8) */
  this->dbConn  ,         /* OUT: SQLite db handle */
  flags,              /* Flags */
  NULL );
  
  if(  ( ! res  ) ||  ( this->dbConn == NULL ) ){
  //failed to open file or failed to allocate memory
  return false;
  }
  
  return true;

	
	}
	
	//Close database
	bool DatabaseManager::Close(){
	bool res = sqlite3_close(  this->dbConn );
	return res;
	}
	
	//Returns the database config
	DBConnectionConfig DatabaseManager::GetDBConfig()const{
	return this->dbConfig;
	}
	
	//Returns the database connection
	sqlite3*  DatabaseManager::GetSQLConnection()const{
	return this->dbConn;
	}
	
	
	///		Sql Statement Operations		///
	
	
	//Check if a sql statement is valid
	bool DatabaseManager::CheckStatement( std::string& _sql ){

	}
	
	//Prepare a sqlite3_stmt from a string and return true if valid
	bool  DatabaseManager::PrepareSql( std::string _sql , sqlite3_stmt* _st ){
	int res =  sqlite3_prepare(
  this->dbConn,            /* Database handle */
  _sql.c_str()  ,       /* SQL statement, UTF-8 encoded */
  _sql.length()  ,              /* Maximum length of zSql in bytes. */
  _st ,  /* OUT: Statement handle */
  NULL    /* OUT: Pointer to unused portion of zSql */
);
if ( res == SQLITE_OK  )return true;
return false;

}
	
	//Step a sql statement and return the result code
	int  DatabaseManager::StepSql( sqlite3_stmt* _st ){
	return sqlite3_step( _st   );
	}
	
	//Destroy a sql statement
	bool  DatabaseManager::DestroySql( sqlite3_stmt* _st){
	if( sqlite3_finalize(_st) == SQLITE_OK )return true ;
	return false;
}
	
	//Reset a sql statement for reuse
	bool  DatabaseManager::ResetSql( sqlite3_stmt* _st ){
	return sqlite3_reset( _st  );
}
	
	//Fetch a stored statement result as a string
	//bool parameter set to false on fail
	bool  DatabaseManager::FetchResult( int stepResult , sqlite3_stmt* _st, std::string& str){
	
	
	}
	
	//Fetch a stored statement result as a int
	//bool parameter set to false on fail
	bool  DatabaseManager::FetchResult(int stepResult , sqlite3_stmt* _st , int& ref){
}
	
	//Fetch a stored statement result as a float
	//bool parameter is false on fail
	bool DatabaseManager::FetchResult(int stepResult , sqlite3_stmt* _st , float& ref){

	
	}
	
	//Retrieve sql statement from sqlite3_stmt
	std::string  DatabaseManager::GetSqlStatement( sqlite3_stmt*  stmt){
	if( ! stmt ){
	return std::string("");
	}
	return std::string(  sqlite3_sql(stmt) );
	
}
	
	//Get columns affected
	int  DatabaseManager::GetColumnCount( sqlite3_stmt*  stmt){
	if (  ! stmt  ){
	return -1; //invalid sql statement
	}
	return sqlite3_column_count( stmt );
	//returns 0 if result query returns no output
}
	

	
	int  DatabaseManager::GetRowsModified()const{
	
}
	
	void  DatabaseManager::SetSQLTimeout(int  ms){
	if( ! this->dbConn )return ; //invalid connection
	sqlite3_busy_timeout(  this->dbConn , ms );
	this->dbTimeout = ms;
}
	
std::string  DatabaseManager::GetError()const{
	return this->lastError;
}
	
	

void DatabaseManager::SetError(std::string _error){
	this->lastError = _error;
};	


 
#endif 


