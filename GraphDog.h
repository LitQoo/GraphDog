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
    GDStruct gdchunk;
    string connect(string url);
    string aID;
    string sKey;
    string udid;
    bool isLogin;
    bool isRun;
    
    static GraphDog* get()
	{
		static GraphDog* _ins = 0;
		if(_ins == 0)
			_ins = new GraphDog();
		return _ins;
	}
    
    void setup(string appID,string secretKey){
        aID=appID;
        sKey=secretKey;
    }
    
    void setAuID(string appuserID){
        CCLog("setAuID:%s",appuserID.c_str());
        CCUserDefault::sharedUserDefault()->setStringForKey("GD_AUID", appuserID.c_str());
        CCUserDefault::sharedUserDefault()->flush();
    }
    
    void setUdid(string _id){
        udid=_id;
    }
    string getAuID(){
        return CCUserDefault::sharedUserDefault()->getStringForKey("GD_AUID");
    }
    string getUdid(){
        return udid;
    }
    string getEmail(){
        return "";
    }
    void setToken(string cTime){
        string udid=getUdid();
        string auid=getAuID();
        string email=getEmail();
        string nick=getNick();
        string flag=getFlag();
        string token=GraphDogLib::GDCreateToken(auid, udid,flag,nick,email,cTime,sKey);
        CCLog("setToken : %s",token.c_str());
        CCUserDefault::sharedUserDefault()->setStringForKey("GD_TOKEN", token);
        CCUserDefault::sharedUserDefault()->flush();
    }
    
    string getToken(){
        return CCUserDefault::sharedUserDefault()->getStringForKey("GD_TOKEN");
    }
    
    void setNick(string nick){
        CCUserDefault::sharedUserDefault()->setStringForKey("GD_NICK", nick);
        CCUserDefault::sharedUserDefault()->flush();
        
    }
    
    string getNick(){
        return CCUserDefault::sharedUserDefault()->getStringForKey("GD_NICK");
    }
    
    void setFlag(string flag){
        CCUserDefault::sharedUserDefault()->setStringForKey("GD_FLAG", flag);
        CCUserDefault::sharedUserDefault()->flush();
    }
    
    string getFlag(){
        return CCUserDefault::sharedUserDefault()->getStringForKey("GD_FLAG");
    }
    
    void start(string appID,string secretKey,CCObject *target,GDSelType selector){
        this->setup(appID,secretKey);
        
        JsonBox::Object abc;
        JsonBox::Array ary;
        
        if(getAuID().length()>0){
            CCLog("login");
            this->command("login", NULL, target, selector);
            
        }else{
            CCLog("join");
            this->command("join", NULL, target, selector);
        }
    }
    void command(string action,JsonBox::Object *param,CCObject *target,GDSelType selector){
        
        //string url = "http://graphdog.net/cmd.php?action";
        
        string udid=getUdid();
        string email=getEmail();
        string auid=getAuID();
        string token;
        string paramStr;
        
        
        if(param!=NULL){
            CCLog("print oss");
            ostringstream oss;
            oss << *param;
            paramStr = oss.str();
            CCLog("paramstr : %s",paramStr.c_str());
            paramStr = GraphDogLib::base64_encode(paramStr.c_str(),paramStr.length());
            CCLog("paramstr base : %s",paramStr.c_str());
        }
        
        if(action=="join")this->setToken("");
        
        token=getToken();
        
        CCString *url = CCString::createWithFormat("http://graphdog.net/cmd.php?action=%s&aID=%s&param=%s&token=%s",action.c_str(),aID.c_str(),paramStr.c_str(),token.c_str());
        
        CCLog("command url : %s",url->getCString());
        
        GDDelegator::getInstance()->addCommand(target,selector,url->getCString());
        
        if(GDDelegator::getInstance()->getCommandCount()>0 && isRun==false){
            //쓰레드돌리기
            if(isRun==false){
                isRun=true;
                pthread_t p_thread;
                int thr_id;
                
                // 쓰레드 생성 아규먼트로 1 을 넘긴다.
                thr_id = pthread_create(&p_thread, NULL, t_function, NULL);
                if (thr_id < 0)
                {
                    perror("thread create error : ");
                    exit(0);
                }
            }
        }
        
        
//        GDDelegator::DeleSel delesel =  GDDelegator::getInstance()->getDeleSel(delekey);
//        ((delesel.target)->*(delesel.selector))(json);
//        GDDelegator::getInstance()->removeDeleSel(delekey);
    }
    CURL* getCURL(){
        return curl_handle;
    }

private:
    CURL *curl_handle;
    static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
    static void* t_function(void *data)
    {
        GraphDog::get()->gdchunk.memory = (char*)malloc(1);
        GraphDog::get()->gdchunk.size = 0;
        while (1) {
            GDDelegator::DeleSel command = GDDelegator::getInstance()->getCommand();
            CURL *handle = GraphDog::get()->getCURL();
            curl_easy_setopt(handle, CURLOPT_URL, command.url.c_str());
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&GraphDog::get()->gdchunk);
            curl_easy_perform(handle);
            
            //CCScheduler::sharedScheduler()->scheduleSelector(schedule_selector(FBConnector::imageDown), FBConnector::get(), 0,false); in cocos2d-x 1.x
            CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GraphDog::completeCommand), GraphDog::get(), 0, false, 0, 0);
            
            while (GraphDog::get()->gdchunk.size!=0) {
                usleep(10000);
            }
            
            if(GDDelegator::getInstance()->getCommandCount()<=0)break;
        }
        GraphDog::get()->isRun=false;
        return NULL;
    }

    void completeCommand(){
        string resultStr = gdchunk.memory;
        CCLog("completeCommand: %s",resultStr.c_str());
        GDDelegator::DeleSel command = GDDelegator::getInstance()->getCommand();
        JsonBox::Value result;
        result.loadFromString(resultStr.c_str());
        
        JsonBox::Object resultobj = result.getObject();
        string action =result["action"].getString();
        if(action=="join"){
            //여기서 auID정상적인가검사.
            setAuID(result["auID"].getString());
            setToken(result["createTime"].getString());
            isLogin=true;
        }else if(action=="login"){
            if(result["createTime"].getString().length()>0){
                CCLog("login get createTime");
                setToken(result["createTime"].getString());
                isLogin=true;
            }else{
                CCLog("login get createTime fail, go join");
                JsonBox::Object param;
                this->command("join",NULL, command.target, command.selector);
            }
        }
        
        
        if(isLogin && command.target!=0 && command.selector!=0)((command.target)->*(command.selector))(result);
        
        
        
        GDDelegator::getInstance()->removeCommand();
        if(gdchunk.memory)free(gdchunk.memory);
        
        gdchunk.memory = (char*)malloc(1);
        gdchunk.size = 0;
    }
    
    GraphDog(){
        curl_global_init(CURL_GLOBAL_ALL);
        curl_handle = curl_easy_init();
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
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
