#include "GDLib.h"
#include <sstream>
#include "BaseXX.h"
#include "KSDes.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "jni.h"
#if COCOS2D_VERSION<0x00020000
#include "android/jni/JniHelper.h"
#include "android/jni/SystemInfoJni.h"
#else
#include "platform/android/jni/JniHelper.h"
#endif
#endif
namespace GraphDogLib {
    

std::string GDCreateToken(string auID,string udid,string flag,string lang,string nick,string email,string platform,string cTime,string secretKey,string dInfo){
    string msg = auID;
    msg.append("||");
    msg.append(udid);
    msg.append("||");
    msg.append(flag);
    msg.append("||");
    msg.append(lang);
    msg.append("||");
    msg.append(nick);
    msg.append("||");
    msg.append(email);
    msg.append("||");
    msg.append(platform);
    msg.append("||");
    msg.append(cTime);
    msg.append("||");
    msg.append(dInfo);
    msg.append("||");
	return toBase64(desEncryption(secretKey, msg));
}
    
void replaceString( std::string & strCallId, const char * pszBefore, const char * pszAfter )
{
    size_t iPos = strCallId.find( pszBefore );
    size_t iBeforeLen = strlen( pszBefore );
    while( iPos < std::string::npos )
    {
            strCallId.replace( iPos, iBeforeLen, pszAfter );
            iPos = strCallId.find( pszBefore, iPos );
    }
}
string JsonObjectToString(JsonBox::Object _obj){
    ostringstream oss;
    oss << _obj;
    return oss.str();
}
    
JsonBox::Object StringToJsonObject(string _str){
    //명령문자열 json::value 로 변환
    JsonBox::Value result;
    result.loadFromString(_str.c_str());
    
    //명령문자열 json::object 로 변환
    JsonBox::Object resultobj = result.getObject();
    return resultobj;
}

	
string getLocalCode()
{
	const char* tempCode;
	
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSArray *languages = [defaults objectForKey:@"AppleLanguages"];
	NSString *currentLanguage = [languages objectAtIndex:0];
	
	// get the current language code.(such as English is "en", Chinese is "zh" and so on)
	NSDictionary* temp = [NSLocale componentsFromLocaleIdentifier:currentLanguage];
	NSString * languageCode = [temp objectForKey:NSLocaleLanguageCode];
	
	tempCode = [languageCode cStringUsingEncoding:NSASCIIStringEncoding];
	
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	tempCode = getCurrentLanguageJNI();
	
#endif
	return tempCode;
}

    
}
