//
//  Created by LitQoo on 13. 2. 14..
//  www.graphdog.net
//

#include "GraphDog.h"
#include <sstream>
#include <unistd.h>
#include "JNIKelper.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "jni.h"
#if COCOS2D_VERSION<0x00020000
#include "android/jni/JniHelper.h"
#else
#include "platform/android/jni/JniHelper.h"
#endif
#endif


#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include <sys/types.h>
#include <sys/sysctl.h>
#endif


int AutoIncrease::cnt = 0;
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
	string deviceId = getDeviceID();
    aID=appID;
    sKey=secretKey;
    this->setUdid(deviceId);
	
#if COCOS2D_VERSION<0x00020000
	// in cocos2d-x 1.x
	CCScheduler::sharedScheduler()->scheduleSelector(schedule_selector(GraphDog::receivedCommand), this, 0,false);
#else
	// in cocos2d-x 2.x
	CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GraphDog::receivedCommand), this, 0.f, false, kCCRepeatForever, 0);
#endif
}

void GraphDog::setAuID(string appuserID){
    CCUserDefault::sharedUserDefault()->setStringForKey("GRAPHDOG_AUID", appuserID.c_str());
    CCUserDefault::sharedUserDefault()->flush();
}
string GraphDog::getAuID(){
    return CCUserDefault::sharedUserDefault()->getStringForKey("GRAPHDOG_AUID");
}
string GraphDog::getUdid(){
    return udid;
}

void GraphDog::setEmail(string email){
    CCUserDefault::sharedUserDefault()->setStringForKey("GRAPHDOG_EMAIL", email);
    CCUserDefault::sharedUserDefault()->flush();
}

string GraphDog::getEmail(){
    return CCUserDefault::sharedUserDefault()->getStringForKey("GRAPHDOG_EMAIL");
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
    CCUserDefault::sharedUserDefault()->setStringForKey("GRAPHDOG_CTIME", cTime);
    CCUserDefault::sharedUserDefault()->flush();
}

string GraphDog::getCTime(){
    string ctime= CCUserDefault::sharedUserDefault()->getStringForKey("GRAPHDOG_CTIME");
    if(ctime=="")ctime="9999";
    return ctime;
}

string GraphDog::getToken(){
	
    string udid=getUdid();
    string auid=getAuID();
    string email=getEmail();
    string nick=getNick();
    string flag=getFlag();
    string lang=getLanguage();
    string platform=getPlatform();
    string cTime=getCTime();
    string token=GraphDogLib::GDCreateToken(auid,udid,flag,lang,nick,email,platform,cTime,sKey);
	
    return token;
}

void GraphDog::setNick(string nick){
    GraphDogLib::replaceString(nick,"|","l");
    CCUserDefault::sharedUserDefault()->setStringForKey("GRAPHDOG_NICK", nick);
    CCUserDefault::sharedUserDefault()->flush();
}

string GraphDog::getNick(){
    return CCUserDefault::sharedUserDefault()->getStringForKey("GRAPHDOG_NICK");
}

void GraphDog::setFlag(string flag){
    CCUserDefault::sharedUserDefault()->setStringForKey("GRAPHDOG_FLAG", flag);
    CCUserDefault::sharedUserDefault()->flush();
}

string GraphDog::getFlag(){
    return CCUserDefault::sharedUserDefault()->getStringForKey("GRAPHDOG_FLAG");
}

void GraphDog::setLanguage(string lang){
    CCUserDefault::sharedUserDefault()->setStringForKey("GRAPHDOG_LANG", lang);
    CCUserDefault::sharedUserDefault()->flush();
}

string GraphDog::getLanguage(){
    string lang = CCUserDefault::sharedUserDefault()->getStringForKey("GRAPHDOG_LANG");
    if(lang=="")lang="en";
    return lang;
}

CURL* GraphDog::getCURL(){
    return curl_handle;
}


bool GraphDog::command(string action, const JsonBox::Object* const param,CCObject *target,GDSelType selector){
    
	CCLog("action seq : %s", action.c_str());
    string udid=getUdid();
    string email=getEmail();
    string auid=getAuID();
    string token;
    string paramStr="";
    
    //파라메터를 json string 으로 변환
    if(param!=NULL){
        ostringstream oss;
        oss << *param;
        paramStr = oss.str();
		
    }
    
    //저장되어있는 토큰불러오기. 없으면 생성
    token=getToken();
    
    //HTTP  POST string 으로 조합.
	
	ostringstream url;
	url << "action=" << action << "&aID=" << aID;
    
    //명령을 등록.
	int insertIndex = AutoIncrease::get();
	CommandType cmd = {target, selector, url.str(), paramStr, this, {(char*)malloc(1), 0, CURLE_AGAIN}, action};
	commands[insertIndex] = cmd;
    //쓰레드가 돌고있지 않으면
    
    //쓰레드돌리기

	pthread_t p_thread;
	int thr_id;
	
	// 쓰레드 생성 아규먼트로 1 을 넘긴다.
	thr_id = pthread_create(&p_thread, NULL, t_function, (void*)insertIndex);
	if (thr_id < 0)
	{
		//쓰레드생성오류시
		JsonBox::Object resultobj;
		resultobj["state"]= JsonBox::Value("error");
		resultobj["errorMsg"]=JsonBox::Value("don't create thread");
		resultobj["errorCode"]=JsonBox::Value(1001);
		if(cmd.target!=0 && cmd.selector!=0)
			((cmd.target)->*(cmd.selector))(resultobj);
		if(cmd.chunk.memory)
			free(cmd.chunk.memory);
		
		commands.erase(insertIndex);
		return false;
	}
        
		
    
    return true;
}

//쓰레드펑션
void* GraphDog::t_function(void *_insertIndex)
{	
	int insertIndex = (int)_insertIndex;
//	std::map<int, CommandType>& commands = graphdog->commands;
//	pthread_mutex_lock(&graphdog->cmdsMutex);
	CommandType& command = graphdog->commands[insertIndex];
		

	pthread_mutex_lock(&command.caller->t_functionMutex);
	
	string token=GraphDog::get()->getToken();

	string paramStr=GraphDogLib::base64_encode(command.paramStr.c_str(), command.paramStr.length());
	command.url=command.url.append("&token=");
	command.url=command.url.append(token);
	command.url=command.url.append("&param=");
	command.url=command.url.append(paramStr);
	command.url=command.url.append("&version=");
	command.url=command.url.append(GRAPHDOG_VERSION);
	
	
	// << "&param=" << paramStr
	//curl으로 명령을 날리고 겨로가를 얻는다.
	CURL *handle = GraphDog::get()->getCURL();
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS,command.url.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&command.chunk);
	
	//		curl_setopt($ch,CURLOPT_TIMEOUT,1000);
//	pthread_mutex_unlock(&graphdog->cmdsMutex);
	CURLcode resultCode = curl_easy_perform(handle);
	
	string resultStr = command.chunk.memory;// gdchunk.memory;
	JsonBox::Object resultobj = GraphDogLib::StringToJsonObject(resultStr);// result.getObject();
	resultobj["resultString"]=JsonBox::Value(resultStr);
	
	//callbackparam
	if(command.paramStr!=""){
		JsonBox::Object param =  GraphDogLib::StringToJsonObject(command.paramStr);
		resultobj["param"]=JsonBox::Value(param);
	}
	
	// 새토큰발급일 경우
	if(resultobj["tokenUpdate"].getString()=="ok"){
		//정상결과시 AuID,Token 다시 세팅
		if(resultobj["state"].getString()=="ok"){
			command.caller->setAuID(resultobj["auID"].getString());
			command.caller->setCTime(resultobj["createTime"].getString());
			command.caller->isLogin=true;
			CCLog("tokenUpdate");
		}else{
			command.caller->setCTime("9999");
		}
		
		//첫실행일경우 받아온 nick,flag 저장.
		if(resultobj["isFirst"].getBoolean()==true){
			command.caller->setNick(resultobj["nick"].getString());
			command.caller->setFlag(resultobj["flag"].getString());
		}
		//기존명령 다시 등록
		JsonBox::Value param;
		param.loadFromString(command.paramStr);
		command.caller->command(command.action, &param.getObject(), command.target, command.selector);
		command.selector = 0;
		command.target = 0;
	}
	
	if(resultobj["errorcode"].getInt()==9999){
		command.caller->setCTime("9999");
		CCLog("auth error");
		command.caller->errorCount++;
		if(command.caller->errorCount<5){
			JsonBox::Value param;
			param.loadFromString(command.paramStr);
			command.caller->command(command.action, &param.getObject(), command.target, command.selector);
			command.selector = 0;
			command.target = 0;
		}
	}
	
	if(resultobj["state"].getString()=="ok"){
		command.caller->errorCount=0;
	}
	command.result = resultobj;
	command.chunk.resultCode = resultCode;
	
	
	pthread_mutex_unlock(&command.caller->t_functionMutex);
	
	return NULL;
}

void GraphDog::removeCommand(cocos2d::CCObject *target)
{
	//GDDelegator::getInstance()->removeCommand(target);
//	pthread_mutex_lock(&cmdsMutex);
	for(std::map<int, CommandType>::iterator iter = commands.begin(); iter != commands.end(); ++iter)
	{
		if(iter->second.target == target)
		{
			iter->second.target = 0;
			iter->second.selector = 0;
		}
	}
//	pthread_mutex_unlock(&cmdsMutex);
}



//실패
//void GraphDog::faildCommand(float dt){
//#if COCOS2D_VERSION<0x00020000
//	CCScheduler::sharedScheduler()->unscheduleSelector(schedule_selector(GraphDog::faildCommand), this);
//#endif
//    GDDelegator::DeleSel command = GDDelegator::getInstance()->getCommand();
//    JsonBox::Object resultobj;
//    resultobj["state"]=JsonBox::Value("error");
//    resultobj["errorMsg"]=JsonBox::Value("check your network state");
//    resultobj["errorCode"]=JsonBox::Value(1002);
//	//callbackparam
//    if(command.paramStr!=""){
//        JsonBox::Object param =  GraphDogLib::StringToJsonObject(command.paramStr);
//        resultobj["param"]=JsonBox::Value(param);
//    }
//    if(command.target!=0 && command.selector!=0)((command.target)->*(command.selector))(resultobj);
//    
//    //명령을 삭제한다.
//    GDDelegator::getInstance()->removeCommand();
//    
//    //메모리도 해제
//    if(gdchunk.memory)free(gdchunk.memory);
//    //메모리다시 할당받는다 (= t_function에 다시 돌아라는 신호를 준다.)
//    gdchunk.memory = (char*)malloc(1);
//    gdchunk.size = 0;
//}
void GraphDog::receivedCommand(float dt)
{
//	pthread_mutex_lock(&cmdsMutex);
	for(std::map<int, CommandType>::iterator iter = commands.begin(); iter != commands.end();)
	{
		string resultStr = iter->second.paramStr;
		CommandType command = iter->second;
		
		try {
			if(command.chunk.resultCode == CURLE_AGAIN || command.chunk.resultCode != CURLE_OK)
			{
				throw command.chunk.resultCode;
			}
			
			CCLog("%", resultStr.c_str());
			JsonBox::Object resultobj =  command.result; //GraphDogLib::StringToJsonObject(resultStr);// result.getObject();
			resultobj["resultString"]=JsonBox::Value(resultStr);
			
			//결과를 지정한 target,selector 으로 넘긴다.
			if(command.target!=0 && command.selector!=0)
				((command.target)->*(command.selector))(resultobj);
			
			//메모리도 해제
			if(command.chunk.memory)
				free(command.chunk.memory);
			
			//명령을 삭제한다.
			commands.erase(iter++);
			
		} catch (const CURLcode& resultCode) {
			if(resultCode == CURLE_AGAIN)
			{
				++iter;
			}
			else if(resultCode != CURLE_OK)
			{
				JsonBox::Object resultobj;
				resultobj["state"]=JsonBox::Value("error");
				resultobj["errorMsg"]=JsonBox::Value("check your network state");
				resultobj["errorCode"]=JsonBox::Value(1002);
				//callbackparam
				if(command.paramStr!=""){
					JsonBox::Object param =  GraphDogLib::StringToJsonObject(command.paramStr);
					resultobj["param"]=JsonBox::Value(param);
				}
				if(command.target!=0 && command.selector!=0)((command.target)->*(command.selector))(resultobj);
				commands.erase(iter++);
				if(command.chunk.memory)
					free(command.chunk.memory);
			}
		}
	}
//	pthread_mutex_unlock(&cmdsMutex);
}
std::string GraphDog::getDeviceID() {
    string _id;
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    char* macAddress=(char*)malloc(18);
	std::string ifName="en0";
	int  success;
	struct ifaddrs * addrs;
	struct ifaddrs * cursor;
	const struct sockaddr_dl * dlAddr;
	const unsigned char* base;
	int i;
    
	success = getifaddrs(&addrs) == 0;
	if (success) {
		cursor = addrs;
		while (cursor != 0) {
			if ( (cursor->ifa_addr->sa_family == AF_LINK)
				&& (((const struct sockaddr_dl *) cursor->ifa_addr)->sdl_type == 0x06) && strcmp(ifName.c_str(),  cursor->ifa_name)==0 ) {
				dlAddr = (const struct sockaddr_dl *) cursor->ifa_addr;
				base = (const unsigned char*) &dlAddr->sdl_data[dlAddr->sdl_nlen];
				strcpy(macAddress, "");
				for (i = 0; i < dlAddr->sdl_alen; i++) {
					if (i != 0) {
						strcat(macAddress, ":");
					}
					char partialAddr[3];
					sprintf(partialAddr, "%02X", base[i]);
					strcat(macAddress, partialAddr);
					
				}
			}
			cursor = cursor->ifa_next;
		}
		
		freeifaddrs(addrs);
	}
    _id = macAddress;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	//_id = JNIKelper::getInstance()->callJava_getUDID();
	JniMethodInfo minfo;
	jobject jobj;
	
	if(JniHelper::getStaticMethodInfo(minfo, packageName.c_str(), "getActivity", "()Ljava/lang/Object;"))
	{
		jobj = minfo.env->NewGlobalRef(minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID));
		CCLog("%x", jobj);
		JniMethodInfo __minfo;
		__minfo.classID = 0;
		__minfo.env = 0;
		__minfo.methodID = 0;
		
		CCLog("!!!!");
		if(JniHelper::getMethodInfo(__minfo, packageName.c_str(), "getUDID", "()Ljava/lang/String;"))
		{
			jstring jstrTitle = (jstring) __minfo.env->CallObjectMethod(jobj, __minfo.methodID);
			
			if(jstrTitle)
			{
				char* pszTitle = (char*)__minfo.env->GetStringUTFChars(jstrTitle, JNI_FALSE);
				
				_id = pszTitle;
				__minfo.env->ReleaseStringUTFChars(jstrTitle, pszTitle);
				__minfo.env->DeleteLocalRef(jstrTitle);
				
			}
			__minfo.env->DeleteLocalRef(__minfo.classID);
		}
		
		minfo.env->DeleteGlobalRef(jobj);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
	
#endif
    //string _id = base64_decode(macAddress);
    return GraphDogLib::base64_encode(_id.c_str(), _id.length());// _id;
}
std::string	GraphDog::getDeviceInfo()
{
	string ret = "unknown";
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
	size_t size;
	string _h = "h";
	string _w = "w";
	string _dot = ".";
	string _m = "m";
	string _a = "a";
	string _c = "c";
	string _i = "i";
	string _n = "n";
	string _e = "e";
	string hw_machine = _h + _w + _dot + _m + _a + _c + _h + _i + _n + _e;
	
	sysctlbyname(hw_machine.c_str(), NULL, &size, NULL, 0);
	char *machine = (char*)malloc(size);
	sysctlbyname(hw_machine.c_str(), machine, &size, NULL, 0);
	/*
	 Possible values:
	 "iPhone1,1" = iPhone 1G
	 "iPhone1,2" = iPhone 3G
	 "iPod1,1"   = iPod touch 1G
	 "iPod2,1"   = iPod touch 2G
	 */
	ret = machine;
	free(machine);
	return ret;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	JniMethodInfo minfo;
	jobject jobj;
	
	if(JniHelper::getStaticMethodInfo(minfo, packageName.c_str(), "getActivity", "()Ljava/lang/Object;"))
	{
		jobj = minfo.env->NewGlobalRef(minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID));
		CCLog("%x", jobj);
		JniMethodInfo __minfo;
		__minfo.classID = 0;
		__minfo.env = 0;
		__minfo.methodID = 0;
		
		CCLog("!!!!");
		if(JniHelper::getMethodInfo(__minfo, packageName.c_str(), "getDeviceInfo", "()Ljava/lang/String;"))
		{
			jstring jstrTitle = (jstring) __minfo.env->CallObjectMethod(jobj, __minfo.methodID);
			
			if(jstrTitle)
			{
				char* pszTitle = (char*)__minfo.env->GetStringUTFChars(jstrTitle, JNI_FALSE);
				
				ret = pszTitle;
				__minfo.env->ReleaseStringUTFChars(jstrTitle, pszTitle);
				__minfo.env->DeleteLocalRef(jstrTitle);
				
			}
			__minfo.env->DeleteLocalRef(__minfo.classID);
		}
		
		minfo.env->DeleteGlobalRef(jobj);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
	
#endif
	return GraphDogLib::gdkeyEnc(ret, sKey);
}

GraphDog* graphdog = GraphDog::get();
