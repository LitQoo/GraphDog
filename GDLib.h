#pragma once

#ifndef __litqoo__GraphDogLib__
#define __litqoo__GraphDogLib__

#include <vector>
#include <string>
#include "cocos2d.h"
#include "JsonBox.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <net/if_dl.h>
#include <ifaddrs.h>
#endif



using namespace std;
/*
 * DES (Data Encrytion Standard) 
 *
 *    - Yu Jinyang(1998. 7. 8)
 *    - email : jinyang@writeme.com
 */

namespace GraphDogLib {
    std::string GDCreateToken(string auID,string udid,string flag,string lang,string nick,string email,string platform,string cTime,string secretKey,string dInfo);
    void replaceString( std::string & strCallId, const char * pszBefore, const char * pszAfter );
    string JsonObjectToString(JsonBox::Object _obj);
    JsonBox::Object StringToJsonObject(string _str);
	string getLocalCode();
    void openAppStore(string appid);
	void openReview(string);
	void openUpdate(string);
}

using namespace cocos2d;
using namespace std;
typedef void (CCObject::*GDSelType)(JsonBox::Object) const;




#define gd_selector(_SELECTOR) (GDSelType)(&_SELECTOR)


#endif

#pragma once