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
#import "inspector.h"
#import "UIElementUtilities.h"

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

#import <QtGui/QApplication>
#import <QtGui/QDesktopWidget>
#import <QtCore/QDataStream>

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

void getWindows( QVector< QRect > & windows, AXUIElementRef element ) {
	NSString * roleName = [UIElementUtilities roleOfUIElement:element];
	bool scrollFix = [roleName isEqualToString:@"AXScrollArea"];
	QRect parentRect( rectOfElement( element ) );

	NSArray * children = subelementsFromElement( element );
	NSEnumerator * e = [children objectEnumerator];
	AXUIElementRef child = nil;
	while( child = ( AXUIElementRef )[e nextObject] ) {
		QRect r( rectOfElement( child ) );
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

bool operator< ( const QRect& r1, const QRect& r2 ) {
	return r1.width() * r1.height() < r2.width() * r2.height();
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

/**
 * @brief C wrapper for finding sub-window
 * @return A serialized binary data by QDataStream, format is QPixmap,
 * QVector< QRect > . The allocated memory must free by hand.
 */
int grabWindow( char * * data ) {
	AXUIElementRef currentUIElement = getCurrentUIElement();
	QRect r( rectOfElement( currentUIElement ) );
	if ( r.isEmpty() ) {
		return -1;
	}

	QVector< QRect > windows;
	windows.push_back( r );
	getWindows( windows, currentUIElement );
	std::sort( windows.begin(), windows.end() );
	QPixmap pm( QPixmap::grabWindow( QApplication::desktop()->winId(), r.x(), r.y(), r.width(), r.height() ) );

	QByteArray buffer;
	QDataStream sout( &buffer, QIODevice::WriteOnly );
	sout << pm << windows;
	*data = static_cast< char * >( std::malloc( buffer.size() ) );
	std::memcpy( *data, buffer.constData(), buffer.size() );
	return buffer.size();
}

/**
 * @brief C wrapper for find current window
 * @param includeDecorations include window decorations
 * @return A serialized binary data by QDataStream, format is QPixmap,
 * QPoint . The allocated memory must free by hand.
 */
int grabCurrent( char * * data, int /*includeDecorations*/ ) {
	AXUIElementRef currentUIElement = getCurrentUIElement();
	QRect r( rectOfElement( currentUIElement ) );
	if( r.isEmpty() ) {
		return -1;
	}
	QPixmap pm( QPixmap::grabWindow( QApplication::desktop()->winId(), r.x(), r.y(), r.width(), r.height() ) );

	QByteArray buffer;
	QDataStream sout( &buffer, QIODevice::WriteOnly );
	sout << pm << r.topLeft();
	*data = static_cast< char * >( std::malloc( buffer.size() ) );
	std::memcpy( *data, buffer.constData(), buffer.size() );
	return buffer.size();
}
