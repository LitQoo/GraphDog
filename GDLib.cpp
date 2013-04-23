#include "GDLib.h"
#include <sstream>
#include "BaseXX.h"
#include "KSDes.h"

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

	
	

    
}
