/*
Name: KSD_AV_FILE_SYSTEM_MANAGER

Description: Manage the filesystem. 
			
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

 
 #ifndef KSD_AV_FS_MANAGER
 
 
 
 #define KSD_AV_FS_MANAGER
 
 //Maximum allowed  file size to scan
 #define KSD_MAX_SCAN_SIZE  52428800 //50 Megabytes
 
 #include <vector>
 #include <string>
 #include <windows.h>
 
 
 //Contains info about file attributes
 struct FileAttributes{
 bool isReadOnly;
 bool isSystem;
 bool isHidden;
 bool isFolder;
 bool isNormal;
 bool isExecutable;
 long fileSize;
 DWORD dwAttributes;
 };
 
 
 //Contains info about each enumerated folder
struct Enum_Folder_Info{

//Number of subfolders
long subFolderCount;

//Number of files
//Can handle quite a number of files 
 long fileCount;
 
//Total size of a folder
//Max handled size is 4 GB
unsigned  long fileSize;



};

 
 //Class will store each scan object
#ifndef KSD_AV_SCAN_OBJ

#include "scan_object.hpp"

#endif 
 
  #ifndef md5_INCLUDED
 
 #include "md5.h"
 
#endif

 
 class FileSystem{
public:
	
	//Automate s the process of populating ScanObject by finding
	//1. File Extension
	//2. File Size
	//3. File Attributes
	//4. File md5 hash
	
	
	//Get Attributes for a file
	FileAttributes GetFileAttributes(const std::string& );
	
	
	//Get File attributes of ScanObject
	bool GetFileAttributes(ScanObject* );
	
	//Set file atributes for file
	bool SetFileAtributes( const std::string& , const  FileAttributes& );
	
	//Delete a file. returns true on success
	bool DeleteFile( const std::string& fName );
	
	//Return a list of all logical drives on the system
	// 0 = all
	// 1 = removable 
	// 2 = fixed
	std::vector<std::string> GetAllDrives(int type = 0);
	
	//Returns a list of all removable drives
	//usb, zip etc
	std::vector<std::string> GetAllRemovableDrives();
	
	
	//Returns a list of all fixed, network and cd drives
	std::vector<std::string> GetAllFixedDrives();	
	
	//Check if the extension of a file is in the list of extensions to scan
	bool CheckExtension( const std::string& );
	
	//Generate the file's av hash signature from a pointer to ScanObject
	bool GenerateHashSignature(ScanObject* );
	
	//Same as above
	bool CheckExtension(const ScanObject& );
	
	//List all the the files in a folder
	//The pointer parameters offer a callback system for the progress of the enumerate process
	//to be tracked in real time.
	Enum_Folder_Info GetDirectoryScanObjects( std::string& dir , std::vector<ScanObject>& refObjects , long* fileCount , bool recursive , bool* condition);
	
	//Same as above, except accepts a vector as input
	Enum_Folder_Info GetDirectoryScanObjects( std::vector<std::string> directories, std::vector<ScanObject>& refObjects , long* fileCount , bool recursive , bool* condition  );
	
	//Return a vector of all scannable extensions
	std::vector<std::string> GetScannableExtensions()const;
	
	//Return a Scan Object from a file name
	//Returns true on success
	bool GetScanObject(const std::string& , ScanObject* );
	
	//Copy a file from one place to another
	bool CopyFile(const std::string& src , const std::string& dest , bool doOverwrite);
	
	//Move a file from one place to another
	bool MoveFile( const std::string& src , const std::string& dest);

	//Check if a file is a windows executable
	bool CheckExecutableFile(const std::string& );
	
	//Same as above
	bool CheckExecutableFile(const ScanObject& );
	
	//Add a file extension to list of scannable extensions
	void AddExtension( const std::string& );
 
	//Get the file extension of a parsed filename
	std::string GetExtension( const std::string& fName);
	
	//Get size of file
	unsigned long GetFileSize( const std::string& );
	
	//Get File Size from handle
	//Increases perfomance when used before or during hash calculation
	unsigned long GetFileSize(  HANDLE );
	
	//Get size of scan object
	unsigned long GetFileSize( ScanObject* );
	
protected:
	std::vector<std::string> vExtensions;
	ScanObject* tmp;
	
 
 };
 
 
 #endif
 
