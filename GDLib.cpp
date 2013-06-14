#include "GDLib.h"
#include <sstream>
#include "BaseXX.h"
#include "KSDes.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "jni.h"
#if COCOS2D_VERSION<0x00020000
#include "android/jni/JniHelper.h"
#include "android/jni/SystemInfoJni.h"
#else
#include "platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
#include "platform/android/jni/JniHelper.h"
#endif
#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#import <UIKit/UIKit.h>
#import <StoreKit/SKStoreProductViewController.h>
#import "AppController.h"

@interface AppController(GraphDog) <SKStoreProductViewControllerDelegate>
-(void)productViewControllerDidFinish:(SKStoreProductViewController *)viewController;
@end

@implementation AppController(GraphDog)
-(void)productViewControllerDidFinish:(SKStoreProductViewController *)viewController
{
    [viewController dismissViewControllerAnimated:YES completion:nil];
}
@end

#endif



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

	
string getLocalCode()
{
	
	
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
	{
		const char* tempCode;
		NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
		NSArray *languages = [defaults objectForKey:@"AppleLanguages"];
		NSString *currentLanguage = [languages objectAtIndex:0];
		
		// get the current language code.(such as English is "en", Chinese is "zh" and so on)
		NSDictionary* temp = [NSLocale componentsFromLocaleIdentifier:currentLanguage];
		NSString * languageCode = [temp objectForKey:NSLocaleLanguageCode];
		
		tempCode = [languageCode cStringUsingEncoding:NSASCIIStringEncoding];
		return tempCode;
	}
	
	
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	{
		string tempCode = getCurrentLanguageJNI();
		return tempCode;
	}
#endif
	
}
    
    void openReview(string appid)
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
		NSString *urlstring = [NSString stringWithFormat:@"itms-apps://itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?id=%s&onlyLatestVersion=true&pageNumber=0&sortOrdering=1&type=Purple+Software",appid.c_str()];
		NSURL *url = [NSURL URLWithString:urlstring];
		[[UIApplication sharedApplication] openURL:url];
#endif
	}
	void openUpdate(string appid)
	{
		#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
		NSString *urlstring = [NSString stringWithFormat:@"itms-apps://itunes.apple.com/WebObjects/MZStore.woa/wa/viewSoftware?id=%s",appid.c_str()];
		NSURL *url = [NSURL URLWithString:urlstring];
		[[UIApplication sharedApplication] openURL:url];
		#endif
	}

    void openAppStore(string appid){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        if(NSClassFromString(@"SKStoreProductViewController")) { // iOS6 이상인지 체크
            AppController *appDelegate = (AppController *)[[UIApplication sharedApplication]delegate];
			
            SKStoreProductViewController *storeViewController = [[SKStoreProductViewController alloc] init];
            storeViewController.delegate = appDelegate;
			
            NSDictionary *parameters = @{SKStoreProductParameterITunesItemIdentifier:[NSNumber numberWithInt:atoi(appid.c_str())]};
            
            [storeViewController loadProductWithParameters:parameters completionBlock:^(BOOL result, NSError *error) {
                if (result){
					[appDelegate.viewController presentViewController:storeViewController animated:YES completion:nil];
                }
			}];
        }else{
            NSString *urlstring = [NSString stringWithFormat:@"itms-apps://itunes.apple.com/WebObjects/MZStore.woa/wa/viewSoftware?id=%s",appid.c_str()];
            NSURL *url = [NSURL URLWithString:urlstring];
            [[UIApplication sharedApplication] openURL:url];
        }
#endif
    }

    
}
