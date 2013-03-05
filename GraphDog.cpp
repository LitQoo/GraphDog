//
//  Created by LitQoo on 13. 2. 14..
//  www.graphdog.com
//

#include "GraphDog.h"
#include <sstream>
#include <unistd.h>
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
        return "android";
#else
        return "null";
#endif
}

void GraphDog::setCTime(string cTime){
    CCUserDefault::sharedUserDefault()->setStringForKey("GD_CTIME", cTime);
    CCUserDefault::sharedUserDefault()->flush();
}

string GraphDog::getCTime(){
    string ctime= CCUserDefault::sharedUserDefault()->getStringForKey("GD_CTIME");
    if(ctime=="")ctime="9999";
    return ctime;
}

string GraphDog::getToken(){
    string udid=getUdid();
    string auid=getAuID();
    string email=getEmail();
    string nick=getNick();
    string flag=getFlag();
    string platform=getPlatform();
    string cTime=getCTime();
    string token=GraphDogLib::GDCreateToken(auid,udid,flag,nick,email,platform,cTime,sKey);
    return token;
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

bool GraphDog::command(string action, const JsonBox::Object* const param,CCObject *target,GDSelType selector){
    
    string udid=getUdid();
    string email=getEmail();
    string auid=getAuID();
    string token;
    string paramStr;
    
    
    //파라메터를 json string 으로 변환
    if(param!=NULL){
        ostringstream oss;
        oss << *param;
        paramStr = oss.str();
        paramStr = GraphDogLib::base64_encode(paramStr.c_str(),paramStr.length());
    }
    
    //저장되어있는 토큰불러오기. 없으면 생성
    token=getToken();
    
    //HTTP  POST string 으로 조합.
	
	ostringstream url;
	url << "action=" << action << "&aID=" << aID << "&param=" << paramStr << "&token=" << token;
    
    //명령을 등록.
    GDDelegator::getInstance()->addCommand(target,selector,url.str());
    
    //쓰레드가 돌고있지 않으면
    if(GDDelegator::getInstance()->getCommandCount()>0 && isRun==false){
        //쓰레드돌리기
        isRun=true;
        pthread_t p_thread;
        int thr_id;
            
        // 쓰레드 생성 아규먼트로 1 을 넘긴다.
        thr_id = pthread_create(&p_thread, NULL, t_function, NULL);
        if (thr_id < 0)
        {
            isRun=false;
            //쓰레드생성오류시
            GDDelegator::DeleSel cmd = GDDelegator::getInstance()->getCommand();
            JsonBox::Object resultobj;
            resultobj["state"]= JsonBox::Value("error");
            resultobj["errorMsg"]=JsonBox::Value("don't create thread");
            resultobj["errorCode"]=JsonBox::Value(1001);
            if(cmd.target!=0 && cmd.selector!=0)((cmd.target)->*(cmd.selector))(resultobj);
            GDDelegator::getInstance()->removeLastCommand();
            return false;
        }
        
    }
    
    return true;
}

//쓰레드펑션
void* GraphDog::t_function(void *data)
{
    //메모리할당
    GraphDog::get()->gdchunk.memory = (char*)malloc(1);
    GraphDog::get()->gdchunk.size = 0;
    
    //GDDelegator 에 들어있는 명령을 순차적으로 수행한다.
    while (1) {
        GDDelegator::DeleSel command = GDDelegator::getInstance()->getCommand();
        
        //curl으로 명령을 날리고 겨로가를 얻는다.
        CURL *handle = GraphDog::get()->getCURL();
        curl_easy_setopt(handle, CURLOPT_URL, "http://graphdog.com/cmd/");
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(handle, CURLOPT_POST, true);
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS,command.url.c_str());
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&GraphDog::get()->gdchunk);
        CURLcode resultCode = curl_easy_perform(handle);
        
        if(resultCode!=CURLE_OK || GraphDog::get()->gdchunk.size==0){
           //실패시처리해주기
            GraphDog::get()->gdchunk.size=1;
#if COCOS2D_VERSION<0x00020000
            // in cocos2d-x 1.x
            CCScheduler::sharedScheduler()->scheduleSelector(schedule_selector(GraphDog::faildCommand), GraphDog::get(), 0,false);
#else
            // in cocos2d-x 2.x
            CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GraphDog::faildCommand), GraphDog::get(), 0, false, 0, 0);
#endif
        }else{
        //완료되면 GL쓰레드로 넘어간다.
#if COCOS2D_VERSION<0x00020000
        // in cocos2d-x 1.x
        CCScheduler::sharedScheduler()->scheduleSelector(schedule_selector(GraphDog::completeCommand), GraphDog::get(), 0, false);
#else
        // in cocos2d-x 2.x
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GraphDog::completeCommand), GraphDog::get(), 0, false, 0, 0);
#endif
        }
        //GL쓰레드에서 자료를 처리할때까지 기다린다.
        while (GraphDog::get()->gdchunk.size!=0) {
            usleep(10000);
        }
        
        //명령이 더이상 없으면 종료.
        if(GDDelegator::getInstance()->getCommandCount()<=0)break;
    }
    //메모리해제
    if(GraphDog::get()->gdchunk.memory)free(GraphDog::get()->gdchunk.memory);
    GraphDog::get()->isRun=false;
    return NULL;
}

void GraphDog::removeCommand(cocos2d::CCObject *target)
{
	GDDelegator::getInstance()->removeCommand(target);
}

//명령수행완료.
void GraphDog::completeCommand(float dt){
#if COCOS2D_VERSION<0x00020000
	CCScheduler::sharedScheduler()->unscheduleSelector(schedule_selector(GraphDog::completeCommand), this);
    //명령문자열받기
#endif
    string resultStr = gdchunk.memory;
    GDDelegator::DeleSel command = GDDelegator::getInstance()->getCommand();
    
    //명령문자열 json::value 로 변환
    JsonBox::Value result;
    result.loadFromString(resultStr.c_str());
    
    //명령문자열 json::object 로 변환
    JsonBox::Object resultobj = result.getObject();
    resultobj["resultString"]=JsonBox::Value(resultStr);
    
    //명령이 start였을경우
    string action =result["action"].getString();
    if(action=="start" || result["tokenUpdate"].getString()=="ok"){
        //정상결과시 AuID,Token 다시 세팅
        if(result["state"].getString()=="ok"){
            setAuID(result["auID"].getString());
            setCTime(result["createTime"].getString());
            isLogin=true;
        }else{
            setCTime("9999");
        }
        
        //첫실행일경우 받아온 nick,flag 저장.
        if(result["isFirst"].getBoolean()==true){
            setNick(result["nick"].getString());
            setFlag(result["flag"].getString());
        }
    }
    
    //결과를 지정한 target,selector 으로 넘긴다.
    if(command.target!=0 && command.selector!=0)((command.target)->*(command.selector))(resultobj);
    
    //명령을 삭제한다.
    GDDelegator::getInstance()->removeCommand();
    //메모리도 해제
    if(gdchunk.memory)free(gdchunk.memory);
    //메모리다시 할당받는다 (= t_function에 다시 돌아라는 신호를 준다.)
    gdchunk.memory = (char*)malloc(1);
    gdchunk.size = 0;
}

//실패
void GraphDog::faildCommand(float dt){
#if COCOS2D_VERSION<0x00020000
	CCScheduler::sharedScheduler()->unscheduleSelector(schedule_selector(GraphDog::faildCommand), this);
#endif
    GDDelegator::DeleSel command = GDDelegator::getInstance()->getCommand();
    JsonBox::Object resultobj;
    resultobj["state"]=JsonBox::Value("error");
    resultobj["errorMsg"]=JsonBox::Value("check your network state");
    resultobj["errorCode"]=JsonBox::Value(1002);
    if(command.target!=0 && command.selector!=0)((command.target)->*(command.selector))(resultobj);
    
    //명령을 삭제한다.
    GDDelegator::getInstance()->removeCommand();
    
    //메모리도 해제
    if(gdchunk.memory)free(gdchunk.memory);
    //메모리다시 할당받는다 (= t_function에 다시 돌아라는 신호를 준다.)
    gdchunk.memory = (char*)malloc(1);
    gdchunk.size = 0;
}

GraphDog* graphdog = GraphDog::get();