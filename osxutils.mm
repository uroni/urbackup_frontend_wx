#import <Cocoa/Cocoa.h>

extern "C" void bring_to_foreground()
{
	[NSApp activateIgnoringOtherApps:YES];
}