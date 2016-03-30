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

Date: 21 March 2014

Author: Trevor Sibanda

Email: trevorsibb@gmail.com

*/

#ifdef KSD_AV_SIGNATURE_INFO
	
	//Default Constructor
	SignatureInfo::SignatureInfo():
		bDetected(false ) , name("")  , percent(0.00f) , hash(""){
		//nothing to do
		}
	
	//Overloaded Operator For Signature Comparison amongst two SignatureInfo
	bool SignatureInfo::operator == (const SignatureInfo& ref){
	//Equity is determined by hashes
	if( this->hash ==  ref.GetHashSignature()  ){
	return true;
	}
	//not the same
	return false;
	}
	
	
	//Overloaded Operator For signature comparison between SignatureInfo and std::string
	bool SignatureInfo::operator == (const std::string& ref){
	if( this->hash == ref ){
	return true;
	}
	return false;
	
	}
	
	//Get percent detection for this sample
	float SignatureInfo::GetDetectionPercent()const{
	return this->percent;
	}
	
	//Check if this is a detected file
	bool SignatureInfo::GetIsDetected()const{
	return this->bDetected;
	}
	
	//Retrieve malware name
	std::string SignatureInfo::GetName()const{
	return this->name;
	}
	
	//retrieve malware hash signature
	std::string SignatureInfo::GetHashSignature()const{
	return this->hash;
	}
	
	//Set the hash
	void SignatureInfo::SetHashSignature( std::string&  _hash){
	this->hash = _hash;
	}
	
	//set the malware name
	void SignatureInfo::SetMalwareName( std::string& _name ){
	this->name = _name;
	}
	
	//set the malware detection percentage
	void SignatureInfo::SetDetection( float _percent){
	this->percent = _percent;
	}
	
	//set is hash of a malware
	void SignatureInfo::SetMalware( bool _isMal ){
	this->bDetected = _isMal;
	}

 
 
 #endif
 
 
