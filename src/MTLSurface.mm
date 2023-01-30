#if RGL_MTL_AVAILABLE
#include "MTLSurface.hpp"

#include <TargetConditionals.h>
#if TARGET_OS_OSX
#include <Cocoa/Cocoa.h>
#elif TARGET_OS_IPHONE
#include <UIKit/UIKit.h>
#else
#endif


std::shared_ptr<RGL::ISurface> CreateMTLSurfaceFromLayer(CAMetalLayer* layer){
	return std::make_shared<RGL::SurfaceMTL>(layer);
}

std::shared_ptr<RGL::ISurface> RGL::CreateMTLSurfaceFromPlatformHandle(void* pointer, bool createSurfaceObject){
	if (createSurfaceObject){
#if TARGET_OS_OSX
		NSWindow* window = (NSWindow*)(pointer);
		NSView *contentView = [window contentView];
		[contentView setWantsLayer:YES];
		CAMetalLayer *res = [CAMetalLayer layer];
		[contentView setLayer:res];
		[res setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        return CreateMTLSurfaceFromLayer(res);
#elif TARGET_OS_IPHONE
        UIWindow* window = (__bridge UIWindow*)pointer;
		UIView* contentView = [[window subviews] lastObject];
		
		CAMetalLayer *res = [CAMetalLayer layer];
		res.frame = window.bounds;
		[contentView.layer addSublayer:res];
		res.needsDisplayOnBoundsChange = true;
        return CreateMTLSurfaceFromLayer(res);
#endif
	}
	
}
#endif
