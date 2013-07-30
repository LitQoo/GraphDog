//
//  SaveData.m
//  BasketWorldCup2
//
//  Created by ksoo k on 13. 1. 4..
//
//

#import "GDSaveData_ios.h"

#include <iostream>
using namespace std;
void gdwriteF(string noused, string tt)
{
	string filePath = "GDKSDATA";
	NSArray* paths = NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES );
	NSString* documentsDirectory = [paths objectAtIndex:0];
	NSString* nsfilePath = [NSString stringWithUTF8String:filePath.c_str()];
	NSString* fullFileName = [NSString stringWithFormat:@"%@/%@", documentsDirectory, nsfilePath ];
	const char* szFilePath = [fullFileName UTF8String];
	FILE* fp = fopen( szFilePath, "w" );
	fprintf(fp, "%s", tt.c_str());
	fclose(fp);
}

string gdreadF(string noused)
{
	string filePath = "GDKSDATA";
	NSArray* paths = NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES );
	NSString* documentsDirectory = [paths objectAtIndex:0];
	NSString* nsfilePath = [NSString stringWithUTF8String:filePath.c_str()];
	NSString* fullFileName = [NSString stringWithFormat:@"%@/%@", documentsDirectory, nsfilePath ];
	const char* szFilePath = [fullFileName UTF8String];
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