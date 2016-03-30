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

 
 #ifdef KSD_AV_FS_MANAGER
  
 #include <windows.h>
 #include <algorithm>
 #include <assert.h>

 
	
	//Get Attributes for a file
	FileAttributes FileSystem::GetFileAttributes(const std::string&  _filename){
	DWORD dwAttrs = ::GetFileAttributes(  _filename.c_str() );
	FileAttributes _fAttr;
	//file all values with false
	_fAttr.isExecutable = false;
	_fAttr.isNormal = false;
	_fAttr.isHidden = false;
	_fAttr.isSystem = false;
	_fAttr.fileSize = 0;
	_fAttr.isFolder = false;
	_fAttr.isReadOnly = false;
	_fAttr.dwAttributes = dwAttrs;
	
	if (dwAttrs==INVALID_FILE_ATTRIBUTES) return _fAttr; 

         if (   (dwAttrs & FILE_ATTRIBUTE_READONLY)  ) { 
            _fAttr.isReadOnly = true;
         } 
		 if (   (dwAttrs & FILE_ATTRIBUTE_SYSTEM)  ) { 
            _fAttr.isSystem	= true;
         }
		 if (   (dwAttrs & FILE_ATTRIBUTE_HIDDEN)  ) { 
            _fAttr.isHidden	= true;
         }
		 if (   (dwAttrs & FILE_ATTRIBUTE_DIRECTORY)  ) { 
            _fAttr.isFolder = true;
         }
		 if (   (dwAttrs & FILE_ATTRIBUTE_NORMAL)  ) { 
            _fAttr.isNormal= true;
         } 
		 if(   this->CheckExecutableFile(_filename) ){
		 _fAttr.isExecutable = true;
		 }
		 //get file size
		 _fAttr.fileSize = this->GetFileSize(  _filename );
		 return _fAttr;
	}
	
	
	//Get File attributes of ScanObject
	bool FileSystem::GetFileAttributes(ScanObject*  ptrScan){
	//can directly access ScanObject attributes because they are friend classes
	//Only isHidden , isSystem , isNormal , isReadOnly can be changed
	
	FileAttributes* tmp = ptrScan->fAttr;
	if( tmp == NULL  ){
	//not initialised
	tmp = new FileAttributes;
	}
	*tmp = this->GetFileAttributes(  ptrScan->GetFilename() );
	return true;
	}
	
	//Set file atributes for file
	bool FileSystem::SetFileAtributes(const std::string& _filename  , const FileAttributes& _fAttr ){
	//First must get file attributes
	FileAttributes _fAttrCurr = this->GetFileAttributes( _filename );
	
	if(   _fAttrCurr.fileSize == 0 ){
	//file does not exist
	return false;
	}
	//now lets check and only change attributes if different
	if(  _fAttr.dwAttributes == _fAttrCurr.dwAttributes ){
	return true;
	}
	//now set attributes
	DWORD dwAttrs;
	
	if (dwAttrs==INVALID_FILE_ATTRIBUTES) return   false; 

         if (   _fAttr.isReadOnly == true )
		 dwAttrs = _fAttrCurr.dwAttributes | FILE_ATTRIBUTE_READONLY; 
         
		 if (  _fAttr.isSystem )
		 dwAttrs = _fAttrCurr.dwAttributes | FILE_ATTRIBUTE_SYSTEM;
		 
		 if (  _fAttr.isHidden )
		 dwAttrs = _fAttrCurr.dwAttributes | FILE_ATTRIBUTE_HIDDEN; 
         
         if( _fAttr.isNormal )
		 dwAttrs = _fAttrCurr.dwAttributes | FILE_ATTRIBUTE_NORMAL;
		 
		return ::SetFileAttributes(  _filename.c_str() , dwAttrs );
		
	}
	
	//Delete a file. returns true on success
	bool FileSystem::DeleteFile(const std::string& _filename ){
	return ::DeleteFile(_filename.c_str()  );
	}
	
	//Check if the extension of a file is in the list of extensions to scan
	bool FileSystem::CheckExtension(const std::string& _filename){
	//now search in the database
	return std::binary_search (this->vExtensions.begin(), this->vExtensions.end(), this->GetExtension(_filename) );
	}
	
	//Generate the file's av hash signature from a pointer to ScanObject
	//Must have populated FileAttributes
	//If not populated It will be done here
	bool FileSystem::GenerateHashSignature(ScanObject*  ptrScan ){
	// Get file extension
	if( ptrScan == NULL)return false;
	
	std::string fname = ptrScan->GetFilename();
	HANDLE hFile = CreateFile(fname.c_str() , GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;							// Error, cannot open file. Return FALSE

	// Get file size and proceed if file is below 50MB
	DWORD dwFileSize = this->GetFileSize(hFile);
	if( ! ptrScan->fAttr){ //update file size on object 
	ptrScan->fAttr = new FileAttributes();
    }
    ptrScan->fAttr->fileSize = dwFileSize;
	
	if (dwFileSize > KSD_MAX_SCAN_SIZE)					
	return false;							// More than max scan size

	// Start hash
	md5_state_t state;
	md5_byte_t digest[16];
	char buffer[1024];
	char szHash[16*2 + 1];
	DWORD dwRead, dwTotal = 0;

	md5_init(&state);
	do {
		if(! ReadFile(hFile, buffer, 1024, &dwRead, NULL) ){
		break; //error in reading file
		//in future such errors must be logged
		}
		
		md5_append(&state, (const md5_byte_t *)buffer, dwRead);

		dwTotal += dwRead;
	} while (dwTotal < dwFileSize);
	
	md5_finish(&state, digest);

	// Convert hash to hex
	for (int di = 0; di < 16; ++di)
	    sprintf(szHash + di * 2, "%02x", digest[di]);

	CloseHandle(hFile);				// Close file handle
	// End hash

	ptrScan->SetHashSignature(  std::string( szHash) );
	
	// Default return value
	return true;
	
	}
	
	//Same as above
	bool FileSystem::CheckExtension(const ScanObject& _scanObj ){
	return this->CheckExtension(  _scanObj.GetFileExtension() );
	}
	
	//List all the the files in a folder
	//The pointer parameters offer a callback system for the progress of the enumerate process
	//to be tracked in real time.
	Enum_Folder_Info FileSystem::GetDirectoryScanObjects( std::string& dir , std::vector<ScanObject>& refObjects ,  long* _fileCount , bool recursive = true , bool* condition = NULL){
	
	bool dontBreakEnum = false;
	
	
	//Find File Info
	WIN32_FIND_DATA tFindData;
	HANDLE hFind;
	Enum_Folder_Info fInfo; //folder enumeration info
	fInfo.fileCount = 0;
	fInfo.fileSize = 0;
	fInfo.subFolderCount = 0;
	
	if( dir == "")return fInfo;
	
	std::vector <std::string> vFolder;				// Store subfolder. Used to scan subfolder
	ScanObject tmp(""); //temporary scan object
	std::string szFind; //find string
	std::string szFolder;
	
	
	
		//check if a wildcard was given
    
		{
    int last_pos = dir.find_first_of(':');            
    if( last_pos == (dir.length()-1) ){
        dir += "\\";
        }
     last_pos = -1;               
     last_pos = dir.find_last_of('*');
	if( last_pos == (dir.length()-1) ){
        dir = std::string( dir , 0 , last_pos );
        }
}
	
	// If file, just create one object and return
	//ignores scan by extension
	if (  this->GetFileAttributes(  dir ).isFolder == false   ) {
		this->GetScanObject( dir ,   &tmp );
		//now update folder info
		fInfo.fileCount = 1;
		*_fileCount++;
		fInfo.fileSize = tmp.GetFileSize();
		fInfo.subFolderCount = 0;
		//push back
		refObjects.push_back( tmp );
		return fInfo;
	}


    // Copy folder name and add trailing backslash if neccessary
	
	
	if(  dir[  dir.length() - 1 ]  != '\\' ){
	dir = dir + "\\";
	}

	// Add wildcat
	szFind = dir + "*";

	hFind = ::FindFirstFile(szFind.c_str()   , &tFindData);
	if (hFind == INVALID_HANDLE_VALUE){
	//invalid find data
	fInfo.fileCount = 0;
	fInfo.fileSize = 0;
	fInfo.subFolderCount = 0;
	return fInfo;
	}

	do {
	//check if enumerate should be breaked
	if( condition != NULL ){
	
	dontBreakEnum = *condition;
	if( *condition == false){
	
	break;
	}
	}
	
		// Directory, copy to vFolder
		if ( tFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY  )
		{
			// File name is not . or ..
			if (!strcmp(tFindData.cFileName, ".") == 0 &&
				!strcmp(tFindData.cFileName, "..") == 0)
			{
				// Find maximum length with null string
				unsigned int nLen = dir.length() + strlen(tFindData.cFileName) + 1;

				// Create a new string
				char *lpFolder = new char[nLen];
				if (lpFolder == NULL) continue;

				// Construct path
				strcpy(lpFolder, dir.c_str()  );
				strcat(lpFolder, tFindData.cFileName);

				// Add to vector array for later processing
				if( recursive )
				vFolder.push_back(   std::string(lpFolder)   );
				fInfo.subFolderCount++;
				
			}
		}
		else
		{
			
			// Find maximum length with null string
			unsigned int nLen = dir.length() + strlen(tFindData.cFileName) + 1;

			// Create a new string
			char *lpFile = new char[nLen];
			if (lpFile == NULL) {
			continue;
			}

			// Construct path
			strcpy(lpFile, dir.c_str()  );
			strcat(lpFile, tFindData.cFileName);
			//Check if file is of allowable type
			if ( !  this->CheckExtension( lpFile )  )
			continue;
			
			//check if file is a scannable type
			
			this->GetScanObject( std::string(lpFile ) , &tmp );
			
			refObjects.push_back(   tmp );
			fInfo.fileCount++;
			
			
			
			// Free memory
			delete []lpFile;
		}
	*_fileCount = refObjects.size();	
		
	} while (
				(::FindNextFile(hFind, &tFindData) != 0) 
			);	

	// We are done scanning this folder
	::FindClose(hFind);

	// Now, let's scan subfolder
	for (size_t i=0; i<vFolder.size(); i++)
	{
			Enum_Folder_Info efi = GetDirectoryScanObjects(vFolder[i] , refObjects , _fileCount , recursive , condition);			// Call this function
			fInfo.fileCount += efi.fileCount;
			fInfo.subFolderCount += efi.subFolderCount;
			vFolder[i].clear();			// Free memory
		
	}
	
	
	}
	
	//Same as above, except it accepts multiple directories
	Enum_Folder_Info FileSystem::GetDirectoryScanObjects( std::vector<std::string> directories, std::vector<ScanObject>& refObjects , long* fileCount , bool recursive = true , bool* condition = NULL  ){
	assert( condition != NULL );
	//in multiple enumerate mode, user must have option to quit at any time
	int s = directories.size();
	Enum_Folder_Info folderInfo;
	std::vector<ScanObject> tmpScanObjects;
	
	folderInfo.fileCount = 0;
	folderInfo.subFolderCount = 0;
	
	
	for( int x = 0; x < s ; x++ ){
	Enum_Folder_Info tmp = this->GetDirectoryScanObjects( directories[x] , tmpScanObjects , fileCount , recursive , condition );
	
	if(*condition == false)
	break;
	
	folderInfo.fileCount += tmp.fileCount;
	folderInfo.subFolderCount += tmp.subFolderCount;
	
	int y = tmpScanObjects.size();
	for(int c = 0; c < y; c++ )
	refObjects.push_back( tmpScanObjects[c] );
	
	tmpScanObjects.clear();
	}
	//enum folder info is not really necessary anymore
	return folderInfo;
	
	}
	
	//Return a vector of all scannable extensions
	std::vector<std::string> FileSystem::GetScannableExtensions()const{
	return this->vExtensions;
	}
	
	//Return a Scan Object from a file name
	//Returns true on success
	bool FileSystem::GetScanObject(const std::string& _filename, ScanObject*  scanObj){
	/*
	if( scanObj == NULL ){
	scanObj = new ScanObject(_filename  );
	}else{
	delete scanObj;
	scanObj = new ScanObject( _filename );
	}
	return true;
	*/
	ScanObject tmp(_filename);
	*scanObj = tmp;
	return true;
	}
	
	//Copy a file from one place to another
	bool FileSystem::CopyFile(const std::string& src , const std::string& dest , bool doOverwrite = false){
	return ::CopyFile(  src.c_str()   , dest.c_str() , doOverwrite );
	}
	
	//Move a file from one place to another
	bool FileSystem::MoveFile(const std::string& src ,const std::string& dest){
	return ::MoveFile(  src.c_str()   , dest.c_str()  );
	}

	//Check if a file is a windows executable
	bool FileSystem::CheckExecutableFile( const std::string& _filename){
	if(  this->GetExtension(_filename) == "exe" )
	return true; //must work on this one
	return false;
	}
	
	//Same as above
	bool FileSystem::CheckExecutableFile( const ScanObject&  scanO ){
	return this->CheckExecutableFile(  scanO.GetFilename() );
	}
	
	//Add a file extension to list of scannable extensions
	void FileSystem::AddExtension(  const std::string& _ext){
	this->vExtensions.push_back(   _ext );
	std::sort (this->vExtensions.begin(), this->vExtensions.end() );   
	}
 
	//Get the file extension of a parsed filename
	std::string FileSystem::GetExtension(const std::string& _filename){
	int len = _filename.length();
if( len == 0 )return "";

int pos = _filename.find_last_of(".");
if(pos <= 0)return "";


return std::string(_filename , pos+1 , len);
	}
	
	
    unsigned long FileSystem::GetFileSize(const std::string& _filename ){
	HANDLE hFile = ::CreateFile(_filename.c_str()  , GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return 0;							// Error, cannot open file.

	unsigned long dwFileSize = ::GetFileSize(hFile, NULL);
	::CloseHandle(hFile );
	if( dwFileSize == INVALID_FILE_SIZE )
	dwFileSize = 0;
	return dwFileSize;
	}
	
	//Get size of file
	//returns -1 if not exist
   unsigned	long FileSystem::GetFileSize( ScanObject* ptrScan ){
	return this->GetFileSize(   ptrScan->GetFilename() );
	}
	
	unsigned long FileSystem::GetFileSize( HANDLE hFile){
	if (hFile == INVALID_HANDLE_VALUE)
		return 0;							// Error, cannot open file

	unsigned long dwFileSize = ::GetFileSize(hFile, NULL);
	//does not close handle
	if( dwFileSize == INVALID_FILE_SIZE )
	dwFileSize = 0;
	return dwFileSize;
	}
	
	std::vector<std::string> FileSystem::GetAllDrives(int type ){
	std::vector<std::string> drives;
	char		szTemp[514] = {0};
    char 		szDrive[3];
    char		*p;


    szDrive[0] = ' ';
    szDrive[1] = ':';
    szDrive[2] = 0;
	if (GetLogicalDriveStrings(514 - 1, szTemp))
        {
		
            p = szTemp;
			
            do
            {
                *szDrive = *p;
				//don't scan drive A and B
                if (szDrive[0] != 65 && szDrive[0] != 66 )
                {
                    UINT dType = ::GetDriveType( szDrive  );
                    if(  dType == DRIVE_NO_ROOT_DIR )
					continue;
					
					if(type == 2 ){
						if( dType
 == DRIVE_FIXED || dType == DRIVE_REMOTE || dType == DRIVE_CDROM || dType == DRIVE_RAMDISK						)
                       drives.push_back( szDrive );
                    }
					else if( type == 1 ){
					
					if( dType == DRIVE_REMOVABLE)
					drives.push_back( szDrive);
					
					}
					else{
					
					drives.push_back( szDrive );
					}
			
		
				}
                    
			while(*p++);
            }
            while (*p);
	
	}else{
	MessageBox(NULL , "Failed to enumerate all drives on the computer. \r\nPlease report this error to the Mthwakazi Defender Developers Team. Thank you\r\nProgram execution will continue." , "OOPS" , 16);
	}
return drives;	
}

	
	//Returns a list of all removable drives
	//usb, zip etc
	std::vector<std::string> FileSystem::GetAllRemovableDrives(){
	return this->GetAllDrives(1);
	}
	
	
	//Returns a list of all fixed, network and cd drives
	std::vector<std::string> FileSystem::GetAllFixedDrives(){
	return this->GetAllDrives(2);
	}	
 
 
 #endif
 
