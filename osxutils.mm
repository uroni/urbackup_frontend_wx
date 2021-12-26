#import <Cocoa/Cocoa.h>
#import <ServiceManagement/ServiceManagement.h>
#include <CoreServices/CoreServices.h>

extern "C" void bring_to_foreground()
{
	[NSApp activateIgnoringOtherApps:YES];
}

extern "C" void remove_login_item()
{
	AuthorizationItem right[1] = {{"system.global-login-items.", 0, NULL, 0}};
	AuthorizationRights rights = {1, right};
	AuthorizationRef auth = NULL; 
	AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &auth);
	
	if(auth==NULL)
	{
		return;
	}

	AuthorizationCopyRights(auth, &rights, kAuthorizationEmptyEnvironment,
                              (kAuthorizationFlagDefaults
                               | kAuthorizationFlagInteractionAllowed
                               | kAuthorizationFlagExtendRights), NULL);

	LSSharedFileListRef login_items = LSSharedFileListCreate(NULL, kLSSharedFileListGlobalLoginItems, NULL);
	if(login_items==NULL)
	{
		return;
	}

	LSSharedFileListSetAuthorization(login_items, auth);

	NSString* app_path = [[[NSBundle mainBundle] bundleURL] absoluteString];

	UInt32 seed;
	CFArrayRef login_items_array = LSSharedFileListCopySnapshot(login_items, &seed);
	for (id item in (NSArray *)login_items_array)
	{       
	    LSSharedFileListItemRef item_ref = (LSSharedFileListItemRef)item;
	    CFURLRef item_path;
	    if (LSSharedFileListItemResolve(item_ref, 0, (CFURLRef*) &item_path, NULL) == noErr)
	    {
	        if ([[(NSURL *)item_path path] hasPrefix:app_path])
	        {
	            LSSharedFileListItemRemove(login_items, item_ref);
	        }
	    }       
	}
}
