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

 
 #ifndef KSD_AV_SCAN_OBJ
 
 #define KSD_AV_SCAN_OBJ
 
 //Contains info about each signature 
 #ifndef KSD_AV_SIGNATURE_INFO
 
 #include "signature_info.hpp"
 
 #endif
 
 //Include filesystem manager
 #ifndef KSD_AV_FS_MANAGER
 #include "file_system.hpp"
 #endif
 
 //must include scan result object
 #ifndef  KSD_SCAN_RESULT_OBJECT
 #include "scan_result_object.h"
 #endif
 
 
 //#include <windows.h>
 #include <string>
 
 //Class stores info about each scanned file
 class ScanObject{
	public:
	//File System must have access to FileAttributes
	friend class FileSystem;
	
	//Av Signature scanner must have access to sigInfo
	friend class HashSignatureScanner;
	
	
	//default constructor
	ScanObject(std::string filename );
	
	//destructor
	~ScanObject();
	
	///		File Operations				///
	
	//returns the filename
	std::string GetFilename()const;
	
	//return file extension
	std::string GetFileExtension()const;
	
	//return file size in bytes
	long GetFileSize();
	
	//Is the file a hidden file
	bool GetIsHiddenFile()const;
	
	
	///		Process Operations			///
	
	//Set the process id if file is running
	void SetProcessId( unsigned int );	
	
	//Get the process id, returns 0 if not running
	unsigned int GetProcessId()const;
	
	///		Signature Info				///
	
	//Check if hash signature has been generated
	bool GetIsHashSignatureGenerated()const;
	
	//Get the file av signature
	std::string GetHashSignature()const;
	
	
	
	//Get File Attributes
	FileAttributes GetFileAttributes()const;
	
	//Returns a pointer to the Signature Info as Contained in the Database
	SignatureInfo* GetSignatureInfo()const;
	
	///		Error related Functions		///
	
	//Return a string describing the last error
	std::string GetError()const;
	
	protected:
	
	//Set the hash signature
	void SetHashSignature( std::string );
	
	unsigned int pid;
	long dwFileSize;
	long dwBufferSize;
	
	bool bFileSizeFound;
	
	std::string error_msg;
	std::string sExt;
	std::string sFilename;
	
	SignatureInfo* sigInfo;
	FileAttributes* fAttr;
	
	
	private:
	
	
	void SetError(std::string );
	
 
 
 };
 
#endif 
