//
//  GDSaveData.cpp
//  BasketWorldCup2
//
//  Created by ksoo k on 13. 1. 4..
//
//

#include "GDSaveData.h"
#include "JsonBox.h"
#include <sstream>
#include "StringCodec.h"
#include "cocos2d.h"

GDSaveData* gdSaveData = GDSaveData::sharedObject();
GDSaveData* GDSaveData::sharedObject()
{
	static GDSaveData* _ins = 0;
	
	if(_ins == 0)
	{
		_ins = new GDSaveData();
		
	}
	return _ins;
}
void GDSaveData::createJSON()
{
	ablePath = cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath();
	string rawData = gdreadF(ablePath);
	if(rawData == "")
		jsonData.loadFromString("{}");
	else
		jsonData.loadFromString(rawData);
	isInit = true;
}
void GDSaveData::setKeyValue(string _key, string _value)
{
	CCAssert(isInit == true, "call createJSON");
	pthread_mutex_lock(&t_functionMutex);
		string key = stringEnc(_key);
		string value = stringEnc(_value);
		jsonData[key] = value;
		ostringstream oss;
		jsonData.writeToStream(oss);
		gdwriteF(ablePath, oss.str());
	pthread_mutex_unlock(&t_functionMutex);
}
void GDSaveData::setKeyValue(string _key, int _value)
{
	CCAssert(isInit == true, "call createJSON");
	pthread_mutex_lock(&t_functionMutex);
		string key = stringEnc(_key);
		ostringstream valueoss;
		valueoss << _value;
		string value = stringEnc(valueoss.str());
		jsonData[key] = value;
		ostringstream oss;
		jsonData.writeToStream(oss);
		gdwriteF(ablePath, oss.str());
	pthread_mutex_unlock(&t_functionMutex);
}

string GDSaveData::getValue(string _key, string defaultValue)
{
	CCAssert(isInit == true, "call createJSON");

	pthread_mutex_lock(&t_functionMutex);
		string key = stringEnc(_key);
		string v = jsonData[key].getString();
		string v2 = stringDecode(v);
		if(v2 == "")
		{
			pthread_mutex_unlock(&t_functionMutex);
			return defaultValue;
		}
		else
		{
			pthread_mutex_unlock(&t_functionMutex);
			return v2;
		}
}

int GDSaveData::getValue(string _key, int defaultValue)
{
	CCAssert(isInit == true, "call createJSON");
	pthread_mutex_lock(&t_functionMutex);
		string key = stringEnc(_key);
	
		string v = jsonData[key].getString();
		string v2 = stringDecode(v);
		int _v2 = atoi(v2.c_str());
		if(v2 == "")
		{
			pthread_mutex_unlock(&t_functionMutex);
			return defaultValue;
		}
		else
		{
			pthread_mutex_unlock(&t_functionMutex);
			return _v2;
		}

}