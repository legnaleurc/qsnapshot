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
#import "inspectorprivate.h"
#import "UIElementUtilities.h"

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

#import <QtGui/QApplication>
#import <QtGui/QDesktopWidget>

#import <QtCore/QtDebug>

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

QRect rectOfElement( AXUIElementRef element ) {
	NSRect bounds = [UIElementUtilities frameOfUIElement:element];
	// NOTE y position is count from bottom in Cocoa
	int realY = QApplication::desktop()->screenGeometry().height() - bounds.origin.y - bounds.size.height;
	return QRect( bounds.origin.x, realY, bounds.size.width, bounds.size.height );
}

void getWindows( std::vector< QRect > & windows, AXUIElementRef element ) {
	NSArray * children = subelementsFromElement( element );
	NSEnumerator * e = [children objectEnumerator];
	AXUIElementRef child;
	while( child = ( AXUIElementRef )[e nextObject] ) {
		QRect r( rectOfElement( child ) );
		if( r.width() < MIN_SIZE || r.height() < MIN_SIZE ) {
			continue;
		}
		windows.push_back( r );
		getWindows( windows, child );
	}
}

bool operator< ( const QRect& r1, const QRect& r2 ) {
	return r1.width() * r1.height() < r2.width() * r2.height();
}

Inspector::Private::Private( Inspector * host ):
host( host ) {
}

AXUIElementRef Inspector::Private::getCurrentUIElement() const {
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

Inspector::Inspector():
p_( new Private( this ) ) {
}

QPixmap Inspector::grabWindow( std::vector< QRect > & windows ) {
	AXUIElementRef currentUIElement = this->p_->getCurrentUIElement();
	QRect r( rectOfElement( currentUIElement ) );
	if ( r.isEmpty() ) {
		return QPixmap();
	}

	windows.push_back( r );
	getWindows( windows, this->p_->getCurrentUIElement() );
	std::sort( windows.begin(), windows.end() );
	return QPixmap::grabWindow( QApplication::desktop()->winId(), r.x(), r.y(), r.width(), r.height() );
}

QPair< QPixmap, QPoint > Inspector::grabCurrent( bool /*includeDecorations*/ ) {
	AXUIElementRef currentUIElement = this->p_->getCurrentUIElement();
	QRect r( rectOfElement( currentUIElement ) );
	if( r.isEmpty() ) {
		return QPair< QPixmap, QPoint >();
	}
	QPixmap pm( QPixmap::grabWindow( QApplication::desktop()->winId(), r.x(), r.y(), r.width(), r.height() ) );
	return qMakePair( pm, r.topLeft() );
}
