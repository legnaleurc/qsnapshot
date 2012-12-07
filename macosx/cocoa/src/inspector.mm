/*
    QSnapshot, a screen capture tool.
    Copyright (C)  2012 Wei Cheng Pan <legnaleurc@gmail>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

#import "UIElementUtilities.h"
#import "inspector.hpp"

const int MIN_SIZE = 8;

AXUIElementRef lineageOfUIElement( AXUIElementRef element ) {
	AXUIElementRef parent = [UIElementUtilities parentOfUIElement:element];
	while( parent ) {
		NSString * roleName = [UIElementUtilities roleOfUIElement:element];
		if( [roleName isEqualToString:@"AXMenuBar"] || [roleName isEqualToString:@"AXWindow"] ) {
			return element;
		}
		element = parent;
		parent = [UIElementUtilities parentOfUIElement:element];
	}
	return element;
}

NSArray * subelementsFromElement( AXUIElementRef element ) {
	NSArray * subElements = nil;
	CFIndex count = 0;
	AXError result;
	NSString * attribute = @"AXChildren";

	result = AXUIElementGetAttributeValueCount( element, ( CFStringRef )attribute, &count );
	if( result != kAXErrorSuccess ) {
		return nil;
	}
	result = AXUIElementCopyAttributeValues( element, ( CFStringRef )attribute, 0, count, ( CFArrayRef * )&subElements );
	if( result != kAXErrorSuccess ) {
		return nil;
	}

	return [subElements autorelease];
}

Rectangle rectOfElement( AXUIElementRef element ) {
	NSRect bounds = [UIElementUtilities frameOfUIElement:element];
	// NOTE y position is count from bottom in Cocoa
	int realY = getScreenHeight() - bounds.origin.y - bounds.size.height;
	return Rectangle( bounds.origin.x, realY, bounds.size.width, bounds.size.height );
}

void getWindows( std::vector< Rectangle > & windows, AXUIElementRef element ) {
	NSString * roleName = [UIElementUtilities roleOfUIElement:element];
	bool scrollFix = [roleName isEqualToString:@"AXScrollArea"];
	Rectangle parentRect( rectOfElement( element ) );

	NSArray * children = subelementsFromElement( element );
	NSEnumerator * e = [children objectEnumerator];
	AXUIElementRef child = nil;
	while( child = ( AXUIElementRef )[e nextObject] ) {
		Rectangle r( rectOfElement( child ) );
		if( scrollFix ) {
			r = r.intersected( parentRect );
		}
		if( r.width() < MIN_SIZE || r.height() < MIN_SIZE ) {
			continue;
		}
		windows.push_back( r );
		getWindows( windows, child );
	}
}

AXUIElementRef getCurrentUIElement() {
	NSPoint cocoaPoint = [NSEvent mouseLocation];
	CGPoint pointAsCGPoint = [UIElementUtilities carbonScreenPointFromCocoaScreenPoint:cocoaPoint];
	AXUIElementRef systemWideElement = AXUIElementCreateSystemWide();
	AXUIElementRef newElement = nil;

	// Ask Accessibility API for UI Element under the mouse
	// And update the display if a different UIElement
	AXError result = AXUIElementCopyElementAtPosition( systemWideElement, pointAsCGPoint.x, pointAsCGPoint.y, &newElement );
	CFRelease( systemWideElement );
	if( result != kAXErrorSuccess || !newElement ) {
		return nil;
	}
	AXUIElementRef currentUIElement = lineageOfUIElement( newElement );
	return currentUIElement;
}

bool operator< ( const Rectangle & r1, const Rectangle & r2 ) {
	return r1.width() * r1.height() < r2.width() * r2.height();
}

Grabber::Grabber():
windows(),
ui( NULL ) {
	AXUIElementRef currentUIElement = getCurrentUIElement();
	Rectangle r( rectOfElement( currentUIElement ) );
	this->windows.push_back( r );
	this->ui = currentUIElement;
}

bool Grabber::isEmpty() const {
	return this->windows.back().isEmpty();
}

void Grabber::traverse() {
	getWindows( this->windows, ( AXUIElementRef )this->ui );
	std::sort( this->windows.begin(), this->windows.end() );
}
