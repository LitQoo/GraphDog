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

struct GDStruct {
    char *memory;
    size_t size;
};

class GraphDog:public CCNode{
public:
    
    static GraphDog* get()
	{
		static GraphDog* _ins = 0;
		if(_ins == 0)
			_ins = new GraphDog();
		return _ins;
	}
    
    //서버연결 후 인증, 이작업이 무사히 완료되어야 다른작업이 가능함.
    void start(string appID,string secretKey,string deviceId,JsonBox::Object *param,CCObject *target,GDSelType selector);
    //명령날리기 - 이 함수로 모든 통신을 할수있다. 쓰레드생성 실패시 false 그외 true
    bool command(string action,JsonBox::Object *param,CCObject *target,GDSelType selector);
    //닉네임저장
    void setNick(string nick);
    //플레그저장
    void setFlag(string flag);
    //이메일저장
    void setEmail(string email);

    string getNick();
    string getFlag();
    bool isLogin;
private:
    GDStruct gdchunk;
    string aID;
    string sKey;
    string udid;
    bool isRun;
    CURL* getCURL();
    string getToken();
    string getEmail();
    string getAuID();
    string getUdid();
    string getPlatform();
    string getCTime();
    
    void setup(string appID,string secretKey);
    void setAuID(string appuserID);
    void setUdid(string _id);
    void setCTime(string cTime);
    
    CURL *curl_handle;
    static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
    static void* t_function(void *data);
    void completeCommand();
    void faildCommand();
    GraphDog(){
        curl_global_init(CURL_GLOBAL_ALL);
        curl_handle = curl_easy_init();
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "GraphDog-agent/1.0");
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl_handle, CURLOPT_COOKIEJAR,"sessid");
        
        isRun=false;
        isLogin=false;
    }
    
    ~GraphDog(){
        curl_easy_cleanup(curl_handle);
    }
};
#endif /* defined(__archervszombie__GraphDog__) */
