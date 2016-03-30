/*
Name: KSD_SCAN_RESULT_OBJECT

Description This is the general structure returned by either the byte 
				scanner or the hash signature based scanner. these are the 
				results shown to the use at the end of the scan and used 
				to perform actions on the detected file. 
				
				This feature is introduced so that we can use callbacks for
				both the signature and rule based scanners.

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
#ifndef KSD_SCAN_RESULT_OBJECT

#define KSD_SCAN_RESULT_OBJECT

#include <windows.h>
#include <string>


#define KSD_SUSP_FILE_ATTR				3//file has suspicious file attributes on none system drive 
#define KSD_SIG_ENGINE_DETECT			1 //detected by signature scanner
#define KSD_RULE_ENGINE_DETECT		2 //detected by rule based engine


typedef struct ScanResultObject{
//name of detected malware
std::string malwareName;

//Full path to infected file
std::string filePath; 

//signature of detected malware
// as calculated by the filesystem class
std::string signature;

//size of detected file as returned by signature scanner
DWORD fileSize;

//File attributes
DWORD fileAttributes;

//which engine detected this sample
int engineDetected;
 
//action recommended to be taken
//this is calculated by the InteligentThreatAction class
int actionToTake;


} SROB , *PSROB;


#endif

