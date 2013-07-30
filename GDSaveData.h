//
//  SaveData.h
//  BasketWorldCup2
//
//  Created by ksoo k on 13. 1. 4..
//
//

#ifndef __GDSaveData__
#define __GDSaveData__

#include <map>
#include <string>
#include "cocos2d.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
	#include "GDSaveData_ios.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	#include "GDSaveData_android.h"
#endif


#include "JsonBox.h"

using namespace std;
class GDSaveData
{
private:
	GDSaveData(){
		isInit = false;
		pthread_mutex_init(&t_functionMutex, NULL);
	}
	pthread_mutex_t t_functionMutex;
	std::string ablePath;
public:
	static GDSaveData* sharedObject();
//	string loadDatas()
//	{
//		string t = readF();
//		return t;
//	}
	void setKeyValue(string key, string value);
	void setKeyValue(string key, int value);
	string getValue(string key, string defaultValue);
	int getValue(string key, int defaultValue);
	void createJSON();
private:
	bool isInit;
private:
	JsonBox::Value jsonData;
};
extern GDSaveData* gdSaveData;
#endif /* defined(__BasketWorldCup2__SaveData__) */
