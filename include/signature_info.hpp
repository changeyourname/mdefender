/*
Name: KSD_AV_SIGNATURE_INFO_DATA_STRUCTURE
 
Description: Defines a class used to store information about each file hash

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

#ifndef KSD_AV_SIGNATURE_INFO

#include <string>

#define KSD_AV_SIGNATURE_INFO


 class SignatureInfo{
	public:
	
	
	//Default Constructor
	SignatureInfo();
	
	//Overloaded Operator For Signature Comparison amongst two SignatureInfo
	bool operator == (const SignatureInfo&);
	
	
	//Overloaded Operator For signature comparison between SignatureInfo and std::string
	bool operator == (const std::string&);
	
	//Get percent detection for this sample
	float GetDetectionPercent()const;
	
	//Check if this is a detected file
	bool GetIsDetected()const;
	
	//Retrieve malware name
	std::string GetName()const;
	
	//retrieve malware hash signature
	std::string GetHashSignature()const;
	
	//Set the hash
	void SetHashSignature( std::string& );
	
	//set the malware name
	void SetMalwareName( std::string& );
	
	//set the malware detection percentage
	void SetDetection( float );
	
	//set is hash of a malware
	void SetMalware( bool );
	
	
	protected:
	
	std::string name;
	std::string hash;
	bool bDetected;
	float percent;
 
 };
 
 
 #endif
 

 
