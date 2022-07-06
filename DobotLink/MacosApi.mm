#import <Foundation/Foundation.h>
#import <Foundation/NSProcessInfo.h>

#include "MacosApi.h"

bool MacosApi::disableAppNap()
{
    if ([[NSProcessInfo processInfo] respondsToSelector:@selector(beginActivityWithOptions:reason:)]) {
        [[NSProcessInfo processInfo] beginActivityWithOptions:0x00FFFFFF reason:@"receiving OSC messages"];
        return true;
    } else {
        return false;
    }
}
