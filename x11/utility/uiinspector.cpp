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
#include "utility/uiinspector.hpp"
#include "qsnapshot_config.h"

#ifdef HAVE_X11_EXTENSIONS_SHAPE_H
#include <QtGui/QBitmap>
#include <QtGui/QPainter>
#endif // HAVE_X11_EXTENSIONS_SHAPE_H

#include <X11/Xlib.h>
#ifdef HAVE_X11_EXTENSIONS_SHAPE_H
#include <X11/extensions/shape.h>
#endif // HAVE_X11_EXTENSIONS_SHAPE_H

#include <QtGui/QX11Info>

#include <algorithm>

bool operator< ( const QRect& r1, const QRect& r2 ) {
	return r1.width() * r1.height() < r2.width() * r2.height();
}

namespace {

	const int minSize = 8;

	Window findRealWindow( Window w, int depth = 0 ) {
		if( depth > 5 ) {
			return None;
		}

		static Atom wm_state = XInternAtom( QX11Info::display(), "WM_STATE", False );
		Atom type;
		int format;
		unsigned long nitems, after;
		unsigned char* prop;

		if( XGetWindowProperty( QX11Info::display(), w, wm_state, 0, 0, False, AnyPropertyType, &type, &format, &nitems, &after, &prop ) == Success ) {
			if( prop != NULL ) {
				XFree( prop );
			}

			if( type != None ) {
				return w;
			}
		}

		Window root, parent;
		Window* children;
		unsigned int nchildren;
		Window ret = None;

		if( XQueryTree( QX11Info::display(), w, &root, &parent, &children, &nchildren ) != 0 ) {
			for( unsigned int i = 0;
				 i < nchildren && ret == None;
				 ++i ) {
				ret = findRealWindow( children[ i ], depth + 1 );
			}

			if( children != NULL ) {
				XFree( children );
			}
		}

		return ret;
	}

	QPixmap grabWindow( Window child, int x, int y, uint w, uint h, uint border ) {
		QPixmap pm( QPixmap::grabWindow( QX11Info::appRootWindow(), x, y, w, h ) );

#ifdef HAVE_X11_EXTENSIONS_SHAPE_H
		int tmp1 = 0, tmp2 = 0;
		// Check whether the extension is available
		if( XShapeQueryExtension( QX11Info::display(), &tmp1, &tmp2 ) ) {
			QBitmap mask( w, h );
			// As the first step, get the mask from XShape.
			int count = 0, order = 0;
			XRectangle* rects = XShapeGetRectangles( QX11Info::display(), child, ShapeBounding, &count, &order );
			// The ShapeBounding region is the outermost shape of the window;
			// ShapeBounding - ShapeClipping is defined to be the border.
			// Since the border area is part of the window, we use bounding
			// to limit our work region
			if( rects ) {
				//Create a QRegion from the rectangles describing the bounding mask.
				QRegion contents;
				for( int pos = 0; pos < count; pos++ ) {
					contents += QRegion( rects[pos].x, rects[pos].y, rects[pos].width, rects[pos].height );
				}
				XFree( rects );

				//Create the bounding box.
				QRegion bbox( 0, 0, w, h );

				if( border > 0 ) {
					contents.translate( border, border );
					contents += QRegion( 0, 0, border, h );
					contents += QRegion( 0, 0, w, border );
					contents += QRegion( 0, h - border, w, border );
					contents += QRegion( w - border, 0, border, h );
				}

				//Get the masked away area.
				QRegion maskedAway = bbox - contents;
				if( !maskedAway.isEmpty() ) {
					QVector< QRect > maskedAwayRects = maskedAway.rects();

					//Construct a bitmap mask from the rectangles
					QPainter p( &mask );
					p.fillRect( 0, 0, w, h, Qt::color1 );
					for( auto it = maskedAwayRects.begin(); it != maskedAwayRects.end(); ++it ) {
						p.fillRect( *it, Qt::color0 );
					}
					p.end();

					pm.setMask(mask);
				}
			}
		}
#endif // HAVE_X11_EXTENSIONS_SHAPE_H

		return pm;
	}

	Window windowUnderCursor( bool includeDecorations = true ) {
		Window root;
		Window child;
		uint mask;
		int rootX, rootY, winX, winY;

		XGrabServer( QX11Info::display() );
		XQueryPointer( QX11Info::display(), QX11Info::appRootWindow(), &root, &child,
			   &rootX, &rootY, &winX, &winY, &mask );

		if( child == None ) {
			child = QX11Info::appRootWindow();
		}

		if( !includeDecorations ) {
			Window real_child = findRealWindow( child );

			if( real_child != None ) { // test just in case
				child = real_child;
			}
		}

		return child;
	}

	void getWindowsRecursive( std::vector< QRect > & windows, Window w, int rx = 0, int ry = 0, int depth = 0 ) {
		XWindowAttributes atts;
		XGetWindowAttributes( QX11Info::display(), w, &atts );

		if ( atts.map_state == IsViewable && atts.width >= minSize && atts.height >= minSize ) {
			int x = 0, y = 0;
			if ( depth ) {
				x = atts.x + rx;
				y = atts.y + ry;
			}

			QRect r( x, y, atts.width, atts.height );
			if( std::find( windows.begin(), windows.end(), r ) == windows.end() ) {
				windows.push_back( r );
			}

			Window root, parent;
			Window* children;
			unsigned int nchildren;

			if( XQueryTree( QX11Info::display(), w, &root, &parent, &children, &nchildren ) != 0 ) {
					for( unsigned int i = 0; i < nchildren; ++i ) {
						getWindowsRecursive( windows, children[ i ], x, y, depth + 1 );
					}

					if( children != NULL ) {
						XFree( children );
					}
			}
		}

		if ( depth == 0 ) {
			std::sort( windows.begin(), windows.end() );
		}
	}

}

std::tuple< QPixmap, QRect, std::vector< QRect > > qsnapshot::utility::grabWindow() {
	int x = 0, y = 0;
	uint w = 0, h = 0;
	uint border = 0, depth = 0;
	Window root;
	XGrabServer( QX11Info::display() );
	Window child = windowUnderCursor();
	XGetGeometry( QX11Info::display(), child, &root, &x, &y, &w, &h, &border, &depth );
	XUngrabServer( QX11Info::display() );
	QRect r( x, y, w, h );

	QPixmap pm( ::grabWindow( child, x, y, w, h, border ) );
	std::vector< QRect > windows;
	getWindowsRecursive( windows, child );

	return std::make_tuple( pm, r, windows );
}

std::tuple< QPixmap, QPoint > qsnapshot::utility::grabCurrent( bool includeDecorations ) {
	int x, y;
	Window root;
	uint w, h, border, depth;

	XGrabServer( QX11Info::display() );
	Window child = windowUnderCursor( includeDecorations );
	XGetGeometry( QX11Info::display(), child, &root, &x, &y, &w, &h, &border, &depth );

	Window parent;
	Window* children;
	unsigned int nchildren;

	if( XQueryTree( QX11Info::display(), child, &root, &parent,
					&children, &nchildren ) != 0 ) {
		if( children != NULL ) {
			XFree( children );
		}

		int newx, newy;
		Window dummy;

		if( XTranslateCoordinates( QX11Info::display(), parent, QX11Info::appRootWindow(),
			x, y, &newx, &newy, &dummy )) {
			x = newx;
			y = newy;
		}
	}

	QPixmap pm( ::grabWindow( child, x, y, w, h, border ) );
	XUngrabServer( QX11Info::display() );
	return std::make_tuple( pm, QPoint( x, y ) );
}
