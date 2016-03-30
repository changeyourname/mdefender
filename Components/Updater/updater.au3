;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;
;
;	Author: Trevor Sibanda
; 
;Name: Mthwakazi Defender Database Updater
;
;License: (c) 2014 Trevor Sibanda, Clifford J Mthemba  , ZimCODERZ 	
;				Released under GNU GPL v2
;
;Description:
 ;			Checks if there are any new updates to Mthwakazi Defender, if so, downloads them 
;			and then  updates Mthwakazi Defender
;
;Forgive my comments in writing this code
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


#notrayicon
#requireAdmin

Global $UPDATE_URL = "http://mdefender.is-best.net/download/latest_update.exe" 
Global $UPDATE_CHECK_URL = "http://mdefender.is-best.net/download/latest_update.txt"
Global $CURRENT_VERSION = "1.0"
Global $LATEST_VERSION = 0


Func ReadCurrentVersion()
$CURRENT_VERSION = IniRead( @scriptdir & "\data\version.data" , "info" , "version" , "1.0" )
EndFunc


Func IsUpdateAVailable()
 $data = InetRead( $UPDATE_CHECK_URL );  
  if( @error ) Then
	  MsgBox( 16 , "M-Update Failure" ,  @error & @crlf  &"Failed to check for the latet version of Mthwakazi Defender, Please make sure you have a working internet connection." & @crlf  & " If problem persits report as bug to Mthwkazi Defender Team" )
	 Exit
  EndIf
$LATEST_VERSION = BinaryToString(  $data )
if( $CURRENT_VERSION < $LATEST_VERSION ) Then
   return True
EndIf
MsgBox(64 , "No updates available" , "It seems no updates of Mthwakazi Defender are available at the moment.  Please follow our Facebook and Twitter for more info" & @crlf & "Current Version is " & $LATEST_VERSION )
return False

EndFunc



ReadCurrentVersion()
MsgBox(64 , "Mthwakazi Defender Updater" , "This tool will help you update M-Defender to the latest version. Make sure you have a working internet connection first. "& @crlf & "If the program fails, manually visit the M-Defender website and download the atest version of M-Defedner from there" )
if( IsUpdateAvailable() )Then
   MsgBox( 64 ,"Update Available" ,   "An update from version  Version " & $CURRENT_VERSION & " to  Version " & $LATEST_VERSION & " is available. Press OK to get information about the download"  )
  $size = InetGetSize( $UPDATE_URL )
if($size == 0 or  @error )Then
	  MsgBox( 16 , "Failed to get file size" , "The function failed to get the size of the new download, This may be due to a network error or  outdated updater. Pease try again. If problem persits visit the M-Defender Website"  )
	  Exit
   EndIf
   
  $sizeMB = Round ( ($size / (1024* 1024) ) , 2 )
  $response = MsgBox(4 , "Confirm Update" , "Are you sure you want to start the update ? " & @crlf & $sizeMB & " Mb will be downloaded from the M-Defender servers. Please confirm " )
  if( $response ==  7 )Then
	 MsgBox(32 , "Huh ? But Why ne... ? " , "Update cancelled. Program will now exit" )
	 Exit
EndIf	 
   
$prog =  ProgressOn( "Updating M-Defender" , "Please wait while Mthwakazi Defender is being updated " ,  "0 Bytes of " & $size & "  MegaBytes  " ) 
Local $hDownload = InetGet($UPDATE_URL, @TempDir & "\mdefender_update.exe", 1, 1)
Do
    Sleep(250)
   $in = InetGetInfo( $hDownload ,  0 )
   
   $percent = ( ( 100*$in) / $size )
   $in =  Round ( ($in / (1024* 1024) ) , 2 )
   
   ProgressSet( $percent , $in & " of " & $sizeMB & "Mb downloaded" ,  $percent &" % Downloading update pack" )
   
Until InetGetInfo($hDownload, 2) ; Check if the download is complete.
ProgressOff()
Local $nBytes = InetGetInfo($hDownload, 0)
InetClose($hDownload) ; Close the handle to release resources.

if(  Not $nBytes == $size )Then
   MsgBox( 16 , "Incomplete download" , "The download did not complete ! The update has failed ! WTF happened ?... I dont know " )
   Exit
EndIf   
MsgBox( 64 , "Download complete" , "Mdefender has succesfully downloaded the upadate pack, Press OK to install it. " )
ShellExecute(@tempdir & "\mdefender_update.exe" )
;open temp folder
ShellExecute( @tempdir )
Exit

EndIf


