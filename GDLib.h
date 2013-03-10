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


std::string GDCreateToken(string auID,string udid,string flag,string nick,string email,string platform,string cTime,string secretKey);
std::string getDeviceID();
}

#pragma once
using namespace cocos2d;
using namespace std;
typedef void (CCObject::*GDSelType)(JsonBox::Object);
#define gd_selector(_SELECTOR) (GDSelType)(&_SELECTOR)
class GDDelegator
{
    
public:
	
	struct DeleSel
	{
		CCObject* target;
		GDSelType selector;
        string url;
	};
    
    
public:
	
    void removeCommand(CCObject* target)
	{
		for(auto iter = cmdTable.begin(); iter != cmdTable.end(); ++iter)
		{
			if(iter->target == target)
			{
				cmdTable.erase(iter);
				break;
			}
		}
	}
    void addCommand(CCObject* target, GDSelType selector,string url){
        DeleSel temp;
		temp.target = target;
		temp.selector = selector;
        temp.url = url;
        cmdTable.push_back(temp);
    }
    
    void removeCommand(){
		if(!cmdTable.empty())
			cmdTable.erase(cmdTable.begin());
    }
    
    void removeLastCommand(){
		if(!cmdTable.empty())
			cmdTable.erase(cmdTable.end());
    }
    
    DeleSel getCommand(){
		if(!cmdTable.empty())
			return cmdTable[0];
		else
		{
			DeleSel ttt;
			ttt.target = NULL;
			return ttt;
		}
    }
    
    int getCommandCount(){
        return cmdTable.size();
    }
    
//	void removeTarget(CCObject* _obj)
//	{
//		// DeleTable<int, DeleSel>
//		for(auto iter = DeleSelTable.begin(); iter != DeleSelTable.end();)
//		{
//			if(iter->second.target == _obj)
//			{
//				DeleSelTable.erase(iter++);
//			}
//			else
//				++iter;
//		}
//	}
//	int addDeleSel(CCObject* target, GDSelType selector,string action,string param)
//	{
//		key++;
//		DeleSel temp;
//		temp.target = target;
//		temp.selector = selector;
//        
//		DeleSelTable[key] = temp;
//		return key;
//	}
//	void removeDeleSel(int _key)
//	{
//		if(DeleSelTable.find(_key) != DeleSelTable.end())
//			DeleSelTable.erase(_key);
//	}
//	DeleSel getDeleSel(int _key)
//	{
//		if(DeleSelTable.find(_key) != DeleSelTable.end())
//		{
//			
//			return DeleSelTable[_key];
//		}
//		else
//		{
//			//CCLog("not found!!!");
//			DeleSel _t;
//			_t.target = 0;
//			_t.selector = 0;
//			return _t;
//		}
//	}
    
public:
	static GDDelegator* getInstance()
	{
		static GDDelegator* _ins = 0;
		if(_ins == 0)
			_ins = new GDDelegator();
		return _ins;
	}
    
private:
	GDDelegator()
	{
		//key = 0;
        //        buff = new CCDictionary;
        //buff=JsonBox::Object();
	}
	~GDDelegator(){}
private:
    vector<DeleSel> cmdTable;
	//map<int, DeleSel> DeleSelTable;
	//int key;
    
public:
    //JsonBox::Object buff;
};



