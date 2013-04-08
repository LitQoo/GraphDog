//
//  GraphDog.h
//  archervszombie
//
//  Created by LitQoo on 13. 2. 14..
//
//

#ifndef __archervszombie__GraphDog__
#define __archervszombie__GraphDog__

#include <iostream>
#include "GDLib.h"
#include "curl/curl.h"
#include <stdlib.h>
#include <queue>
#include <list>
#define GRAPHDOG_VERSION    "0.9"
struct GDStruct {
    char *memory;
    size_t size;
	CURLcode resultCode;
};


struct AutoIncrease
{
	static int cnt;
	static int get(){return cnt++;}
};

class GraphDog: public CCObject{
public:
	struct CommandType
	{
		CCObject* target;
		GDSelType selector;
        string url;
        string paramStr;
		GraphDog* caller;
		GDStruct chunk;
		string action;
		JsonBox::Object result;
	};
	std::map<int, CommandType> commands;
    static GraphDog* get()
	{
		static GraphDog* _ins = 0;
		if(_ins == 0)
			_ins = new GraphDog();
		return _ins;
	}
    void removeCommand(cocos2d::CCObject *target);
	
	std::string getDeviceID();
	void setPackageName(string pkg)
	{
		packageName = pkg;
	}
    //시작설정
    void setup(string appID,string secretKey);
    //명령날리기 - 이 함수로 모든 통신을 할수있다. 쓰레드생성 실패시 false 그외 true
    bool command(string action, const JsonBox::Object* const param,CCObject *target,GDSelType selector);
    //닉네임저장
    void setNick(string nick);
    //플레그저장
    void setFlag(string flag);
    //이메일저장
    void setEmail(string email);
    //언어저장
    void setLanguage(string lang);
    
    string getNick();
    string getFlag();
    string getAuID();
    string getLanguage();
    string getPlatform();
    string getEmail();
    
    bool isLogin;
private:
//    GDStruct gdchunk;
	pthread_mutex_t t_functionMutex;
//	pthread_mutex_t cmdsMutex;
//	pthread_mutex_t authMutex;
    string aID;
    string sKey;
    string udid;
	string packageName;
    CURL* getCURL();
    string getToken();
    string getUdid();
    string getCTime();
    int errorCount;
    void setAuID(string appuserID);
    void setUdid(string _id);
    void setCTime(string cTime);
    
    CURL *curl_handle;
    static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
    static void* t_function(void *data);
    void receivedCommand(float dt);

    GraphDog(){
		pthread_mutex_init(&t_functionMutex, NULL);
//		pthread_mutex_init(&cmdsMutex, NULL);
		//pthread_mutex_lock(&authMutex);
        curl_global_init(CURL_GLOBAL_ALL);
        curl_handle = curl_easy_init();
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "GraphDog-agent/1.0");
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl_handle, CURLOPT_COOKIEJAR,"sessid");
		curl_easy_setopt(curl_handle, CURLOPT_URL, "http://www.graphdog.net/command/");
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl_handle, CURLOPT_POST, true);
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 10 );

        isLogin=false;
        errorCount=0;
    }
    
    ~GraphDog(){
        curl_easy_cleanup(curl_handle);
		curl_global_cleanup();
    }
};

extern GraphDog* graphdog;
#endif /* defined(__archervszombie__GraphDog__) */
