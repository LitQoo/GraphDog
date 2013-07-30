//
//  SaveData_android.cpp
//  BasketWorldCup2
//
//  Created by ksoo k on 13. 1. 4..
//
//



#include "GDSaveData_android.h"



/*
 
 string filePath = "KSDATA";
 NSArray* paths = NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES );
 NSString* documentsDirectory = [paths objectAtIndex:0];
 NSString* nsfilePath = [NSString stringWithUTF8String:filePath.c_str()];
 NSString* fullFileName = [NSString stringWithFormat:@"%@/%@", documentsDirectory, nsfilePath ];
 const char* szFilePath = [fullFileName UTF8String];
 FILE* fp = fopen( szFilePath, "w" );
 fprintf(fp, "%s", tt.c_str());
 fclose(fp);
 }
 
 string readF()
 {
 string filePath = "KSDATA";
 NSArray* paths = NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES );
 NSString* documentsDirectory = [paths objectAtIndex:0];
 NSString* nsfilePath = [NSString stringWithUTF8String:filePath.c_str()];
 NSString* fullFileName = [NSString stringWithFormat:@"%@/%@", documentsDirectory, nsfilePath ];
 const char* szFilePath = [fullFileName UTF8String];
 cout << szFilePath << endl;
 FILE* fp = fopen( szFilePath, "r" );
 string result;
 if(fp)
 {
 fseek(fp, 0L, SEEK_END);
 int s = ftell(fp);
 fseek(fp, 0L, SEEK_SET);
 char* buffer = new char[s+1];
 memset(buffer, 0, sizeof(s+1));
 fread(buffer, sizeof(char), s, fp);
 fclose(fp);
 
 
 result = buffer;
 delete [] buffer;
 }
 else
 {
 return "";
 }
 
 return result;
 }
 */



#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
void gdwriteF(string path, string tt)
{
	string path2 = path + "GDKSDATA";
	FILE* fp = fopen( path2.c_str(), "w" );
	if(!fp)
	{
		CCLog("file failure2");
	}
	fprintf(fp, "%s", tt.c_str());
	fclose(fp);
}
string gdreadF(string path)
{
	string path2 = path + "GDKSDATA";
	FILE* fp = fopen( path2.c_str(), "r" );
	string result;
	if(fp)
	{
		fseek(fp, 0L, SEEK_END);
		int s = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		char* buffer = new char[s+1];
		memset(buffer, 0, sizeof(s+1));
		fread(buffer, sizeof(char), s, fp);
		fclose(fp);
		
		
		result = buffer;
		delete [] buffer;
	}
	else
	{
		CCLog("read fail");
		return "";
	}
	return result;
}


#endif
