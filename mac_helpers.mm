// objective-c++
#include "mac_helpers.h"
#include <SDL.h>
#include <SDL_syswm.h>
#import <Cocoa/Cocoa.h>

void ApplyMacRoundedCorners(SDL_Window* sdlWindow, int radius) {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    if (!SDL_GetWindowWMInfo(sdlWindow, &info) || info.subsystem != SDL_SYSWM_COCOA) return;

    NSWindow* nswin = info.info.cocoa.window;
    if (!nswin) return;

    nswin.opaque = NO;
    nswin.backgroundColor = [NSColor clearColor];

    NSView* view = nswin.contentView;
    [view setWantsLayer:YES];
    view.layer.masksToBounds = YES;
    view.layer.cornerRadius = radius;
}

extern "C" void SetupMenu() {
    @autoreleasepool {
        NSMenu *mainMenu = [[NSMenu alloc] initWithTitle:@""];
        // App menu
        NSMenuItem *appMenuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
        [mainMenu addItem:appMenuItem];
        NSMenu *appMenu = [[NSMenu alloc] initWithTitle:@""];

        [appMenu addItemWithTitle:@"Quit 𝙘𝙤𝙨𝙢𝙤𝙨"
                           action:@selector(terminate:)
                    keyEquivalent:@"q"];

        [appMenuItem setSubmenu:appMenu];

        // Intentionally do NOT add a "Window" menu.
        [NSApp setMainMenu:mainMenu];
    }
}
