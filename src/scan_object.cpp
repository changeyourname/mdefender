/*
Name: KSD_AV_SEARCH_OBJECT_STRUCTURE

Description: Defines a class used to store information about each scanned file object	

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

Date: 15 March 2014

Author: Trevor Sibanda

Email: trevorsibb@gmail.com

*/

 
 #ifdef KSD_AV_SCAN_OBJ
 
 
 //Class stores info about each scanned file
	
	//default constructor
	ScanObject::ScanObject(std::string filename ):
	dwFileSize(0),
	dwBufferSize(0),
	bFileSizeFound(false),
	sExt(""),
	sFilename(filename),
	sigInfo(0),
	fAttr(0) , pid(0) , error_msg("")
	{
	
	}
	
	//destructor
	ScanObject::~ScanObject(){
	delete this->sigInfo;
	delete this->fAttr;
	this->sFilename.clear();
	this->sExt.clear();
	this->error_msg.clear();
	
	}
	
	///		File Operations				///
	
	//returns the filename
	std::string ScanObject::GetFilename()const{
	return this->sFilename;
	}
	
	//return file extension
	std::string ScanObject::GetFileExtension()const{
	return this->sExt;
	}
	
	//return file size in bytes
	long ScanObject::GetFileSize(){
	if( this->fAttr )
	return this->fAttr->fileSize;
	
	return 0;
	}
	
	//Is the file a hidden file
	bool ScanObject::GetIsHiddenFile()const{
	if( this->fAttr )
	return this->fAttr->isHidden;
	
	return false;
	
	}
	
	
	
	///		Process Operations			///
	
	//Set the process id if file is running
	void ScanObject::SetProcessId( unsigned int _pid){
	this->pid = _pid;
	}	
	
	//Get the process id, returns 0 if not running
	unsigned int ScanObject::GetProcessId()const{
	return this->pid;
	}
	
	///		Signature Info				///
	
	//Check if hash signature has been generated
	bool ScanObject::GetIsHashSignatureGenerated()const{
	//check pointer
	if( this->sigInfo ){
	//check if string is empty
	if( this->sigInfo->GetHashSignature() != "" ){
	return true;
	}
	return false;
	}
	return false;
	}
	
	//Get the file av signature
	std::string ScanObject::GetHashSignature()const{
	if(this->sigInfo){
	return this->sigInfo->GetHashSignature();
	}
	return "";
	}
	
	//Get File Attributes
	FileAttributes ScanObject::GetFileAttributes()const{
	if( this->fAttr )
	return *this->fAttr;
	//just return a new empty FileAttribute if not created
	FileAttributes tmp; // = new FileAttribute(); 
	return tmp;
	}
	
	//Returns a pointer to the Signature Info as Contained in the Database
	SignatureInfo* ScanObject::GetSignatureInfo()const{
	return this->sigInfo;
	}
	
	///		Error related Functions		///
	
	//Return a string describing the last error
	std::string ScanObject::GetError()const{
	return this->error_msg;
	}
	
	
	
	
	 
	void ScanObject::SetError(std::string _error){
	this->error_msg = _error;
	}
	
	void ScanObject::SetHashSignature(std::string _hash ){
	if( this->sigInfo == NULL){
	this->sigInfo = new SignatureInfo;
	}
	this->sigInfo->SetHashSignature( _hash );
	return;
	}
 
 

 
#endif 
