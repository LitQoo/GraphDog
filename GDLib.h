#pragma once

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
    
    typedef unsigned long   DWORD;
    typedef unsigned short  WORD;
    typedef unsigned char   BYTE;

    typedef	union	{
        DWORD   dw[2];
        WORD    w[4];
        BYTE	b[8];
    } BIT64;


    void IP(BIT64 *text);
    void FP(BIT64 *text);
    void MakeKey(BIT64 key, BIT64 keys[16]);
    void RoundFunction(BIT64 key, BIT64 *text);
    void des_encrytion(BIT64 key, BIT64 plain, BIT64 *out);
    void des_decrytion(BIT64 key, BIT64 plain, BIT64 *out);
    vector<char> des_enc(string secretKey, string msg);


    std::string base64_encode(char const* bytes_to_encode, unsigned int in_len) ;
    std::string base64_decode(std::string const& encoded_string);
    std::string gdkeyEnc(const std::string& msg,string key);


    std::string GDCreateToken(string auID,string udid,string flag,string lang,string nick,string email,string platform,string cTime,string secretKey,string dInfo);
    
    
    void replaceString( std::string & strCallId, const char * pszBefore, const char * pszAfter );
    string JsonObjectToString(JsonBox::Object _obj);
    JsonBox::Object StringToJsonObject(string _str);
}

#pragma once
using namespace cocos2d;
using namespace std;
typedef void (CCObject::*GDSelType)(JsonBox::Object);
#define gd_selector(_SELECTOR) (GDSelType)(&_SELECTOR)


