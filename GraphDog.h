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
#define GRAPHDOG_VERSION    "0.9"
struct GDStruct {
    char *memory;
    size_t size;
};


class GraphDog: public CCObject{
public:
    static GraphDog* get()
	{
		static GraphDog* _ins = 0;
		if(_ins == 0)
			_ins = new GraphDog();
		return _ins;
	}
    
    //시작설정
    void setup(string appID,string secretKey,string deviceId);
    //명령날리기 - 이 함수로 모든 통신을 할수있다. 쓰레드생성 실패시 false 그외 true
    bool command(string action, const JsonBox::Object* const param,CCObject *target,GDSelType selector);
    //닉네임저장
    void setNick(string nick);
    //플레그저장
    void setFlag(string flag);
    //이메일저장
    void setEmail(string email);
	// 딜리게이터 삭제
	void removeCommand(CCObject* target);
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
    GDStruct gdchunk;
    string aID;
    string sKey;
    string udid;
    bool isRun;
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
    void completeCommand(float dt);
    void faildCommand(float dt);
    GraphDog(){
        curl_global_init(CURL_GLOBAL_ALL);
        curl_handle = curl_easy_init();
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "GraphDog-agent/1.0");
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl_handle, CURLOPT_COOKIEJAR,"sessid");
        
        isRun=false;
        isLogin=false;
        errorCount=0;
    }
    
    ~GraphDog(){
        curl_easy_cleanup(curl_handle);
    }
};

extern GraphDog* graphdog;
#endif /* defined(__archervszombie__GraphDog__) */
