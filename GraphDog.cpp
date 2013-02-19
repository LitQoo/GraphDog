//
//  GraphDog.cpp
//  archervszombie
//
//  Created by LitQoo on 13. 2. 14..
//JHGJHG//
// fuck litqoo

#include "GraphDog.h"

size_t GraphDog::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp){
    size_t realsize = size * nmemb;
    struct GDStruct *mem = (struct GDStruct *)userp;
    
    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    
    if (mem->memory == NULL) {
        /* out of memory! */
        exit(EXIT_FAILURE);
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

void GraphDog::setUdid(string _id){
    udid=_id;
}
void GraphDog::setup(string appID,string secretKey){
    aID=appID;
    sKey=secretKey;
}
void GraphDog::setAuID(string appuserID){
    CCUserDefault::sharedUserDefault()->setStringForKey("GD_AUID", appuserID.c_str());
    CCUserDefault::sharedUserDefault()->flush();
}
string GraphDog::getAuID(){
    return CCUserDefault::sharedUserDefault()->getStringForKey("GD_AUID");
}
string GraphDog::getUdid(){
    return udid;
}

void GraphDog::setEmail(string email){
    CCUserDefault::sharedUserDefault()->setStringForKey("GD_EMAIL", email);
    CCUserDefault::sharedUserDefault()->flush();
}

string GraphDog::getEmail(){
    return CCUserDefault::sharedUserDefault()->getStringForKey("GD_EMAIL");
}
string GraphDog::getPlatform(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        return "ios";
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        return "android"
#else
        return "null";
#endif
}
void GraphDog::setToken(string cTime){
    string udid=getUdid();
    string auid=getAuID();
    string email=getEmail();
    string nick=getNick();
    string flag=getFlag();
    string flatform=getPlatform();
    string token=GraphDogLib::GDCreateToken(auid, udid,flag,nick,email,flatform,cTime,sKey);
    CCUserDefault::sharedUserDefault()->setStringForKey("GD_TOKEN", token);
    CCUserDefault::sharedUserDefault()->flush();
}

string GraphDog::getToken(){
    return CCUserDefault::sharedUserDefault()->getStringForKey("GD_TOKEN");
}

void GraphDog::setNick(string nick){
    CCUserDefault::sharedUserDefault()->setStringForKey("GD_NICK", nick);
    CCUserDefault::sharedUserDefault()->flush();
}

string GraphDog::getNick(){
    return CCUserDefault::sharedUserDefault()->getStringForKey("GD_NICK");
}

void GraphDog::setFlag(string flag){
    CCUserDefault::sharedUserDefault()->setStringForKey("GD_FLAG", flag);
    CCUserDefault::sharedUserDefault()->flush();
}

string GraphDog::getFlag(){
    return CCUserDefault::sharedUserDefault()->getStringForKey("GD_FLAG");
}

CURL* GraphDog::getCURL(){
    return curl_handle;
}

void GraphDog::start(string appID,string secretKey,string deviceId,JsonBox::Object *param,CCObject *target,GDSelType selector){
    this->setup(appID,secretKey);
    this->setUdid(deviceId);

    JsonBox::Object abc;
    JsonBox::Array ary;
    
    this->command("start", param, target, selector);
}

void GraphDog::command(string action,JsonBox::Object *param,CCObject *target,GDSelType selector){
    
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
    }
    
    token=getToken();
    
    CCString *url = CCString::createWithFormat("action=%s&aID=%s&param=%s&token=%s",action.c_str(),aID.c_str(),paramStr.c_str(),token.c_str());
    
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
}

void* GraphDog::t_function(void *data)
{
    GraphDog::get()->gdchunk.memory = (char*)malloc(1);
    GraphDog::get()->gdchunk.size = 0;
    while (1) {
        GDDelegator::DeleSel command = GDDelegator::getInstance()->getCommand();
        CURL *handle = GraphDog::get()->getCURL();
        curl_easy_setopt(handle, CURLOPT_URL, "http://graphdog.net/cmd.php");
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(handle, CURLOPT_POST, true);
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS,command.url.c_str());
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&GraphDog::get()->gdchunk);
        curl_easy_perform(handle);
        
#if COCOS2D_VERSION<0x00020000
        // in cocos2d-x 1.x
        CCScheduler::sharedScheduler()->scheduleSelector(schedule_selector(GraphDog::completeCommand), GraphDog::get(), 0,false);
#else
        // in cocos2d-x 2.x
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GraphDog::completeCommand), GraphDog::get(), 0, false, 0, 0);
#endif
        while (GraphDog::get()->gdchunk.size!=0) {
            usleep(10000);
        }
        
        if(GDDelegator::getInstance()->getCommandCount()<=0)break;
    }
    GraphDog::get()->isRun=false;
    return NULL;
}

void GraphDog::completeCommand(){
    string resultStr = gdchunk.memory;
    CCLog("completeCommand: %s",resultStr.c_str());
    GDDelegator::DeleSel command = GDDelegator::getInstance()->getCommand();
    JsonBox::Value result;
    result.loadFromString(resultStr.c_str());
    
    JsonBox::Object resultobj = result.getObject();
    string action =result["action"].getString();
    
    if(action=="start"){
        if(result["state"].getString()=="ok"){
            setAuID(result["auID"].getString());
            setToken(result["createTime"].getString());
            isLogin=true;
        }
        
        if(result["isFirst"].getBoolean()==true){
            setNick(result["nick"].getString());
            setFlag(result["flag"].getString());
        }
    }
    
    
    if(command.target!=0 && command.selector!=0)((command.target)->*(command.selector))(resultobj);
    GDDelegator::getInstance()->removeCommand();
    if(gdchunk.memory)free(gdchunk.memory);
    
    gdchunk.memory = (char*)malloc(1);
    gdchunk.size = 0;
}
