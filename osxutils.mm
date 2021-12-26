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

// check_full_disk_access() based on MacPaw's PermissionsKit
// https://github.com/MacPaw/PermissionsKit
// PermissionsKit Copyright (c) 2018 MacPaw
// MIT License
//

extern "C" void check_full_disk_access()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    if (@available(macOS 10.14, *))
    {
//        Full Disk Access only required for macOS 10.14+
        NSLog(@"[macOS] macOS 10.14 or higher installed - need to check for Full Disk Access");
    } else {
        NSLog(@"[macOS] macOS 10.13 or lower installed - no need to check for Full Disk Access");
        return;
    }
    
//    Check for file access
    NSFileManager *filemanager = [NSFileManager defaultManager];
    NSString *path;
    BOOL fileExists;

    path = [NSHomeDirectory()  stringByAppendingPathComponent:@"Library/Safari/CloudTabs.db"];
    fileExists = [filemanager fileExistsAtPath:path];
    if (fileExists)
    {
        NSLog(@"[macOS] ~/Library/Safari/CloudTabs.db exists - use for Full Disk Access check");
    }
    else
    {
        path = [NSHomeDirectory()  stringByAppendingPathComponent:@"Library/Safari/Bookmarks.plist"];
        fileExists = [filemanager fileExistsAtPath:path];
        if (fileExists)
        {
            NSLog(@"[macOS] ~/Library/Safari/Bookmarks.plist exists - use for Full Disk Access check");
        }
        else
        {
            path = [NSHomeDirectory()  stringByAppendingPathComponent:@"Library/Accounts/Accounts4.sqlite"];
            fileExists = [filemanager fileExistsAtPath:path];
            if (fileExists)
            {
                NSLog(@"[macOS] ~/Library/Accounts/Accounts4.sqlite exists - use for Full Disk Access check");
            }
        }
    }
    
    NSData* data = [NSData dataWithContentsOfFile:path];
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setAlertStyle:NSAlertStyleCritical];
    if (data != nil && fileExists)
    {
//        Full Disk Access Allowed
        NSLog(@"[macOS] Full Disk Access Allowed");
    }
    else
    {
        if (data == nil && fileExists)
        {
//        Full Disk Access Denied
            NSLog(@"[macOS] Full Disk Access Denied");
            [alert setMessageText:@"Full Disk Access Denied"];
            [alert setInformativeText:@"UrBackup needs Full Disk Access to operate.\n\nPlease use System Preferences to allow the UrBackup Client application Full Disk Access.\n\nSystem Preferences will open when you press OK."];
        }
        else
        {
//        Cannot determine Full Disk Access status
            NSLog(@"[macOS] Full Disk Access cannot be determined");
            [alert setMessageText:@"Cannot determine Full Disk Access status"];
            [alert setInformativeText:@"UrBackup needs Full Disk Access to operate.\n\nPlease try using System Preferences to allow the UrBackup Client application Full Disk Access.\n\nSystem Preferences will open when you press OK."];
        }
//        Display dialogue
        [alert runModal];
//        Open System Preferences
        NSWorkspace *workspace = [[NSWorkspace alloc] init];
        [workspace openURL:[NSURL URLWithString:@"x-apple.systempreferences:com.apple.preference.security?Privacy_AllFiles"]];
    }
    [pool drain];
    return;
}