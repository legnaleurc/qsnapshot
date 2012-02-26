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

#include <QtGui/QCursor>

#include <Windows.h>

namespace {

	const int minSize = 8;
	uint cxWindowBorder;
	uint cyWindowBorder;

	QPixmap grabWindow( HWND hWnd ) {
		RECT windowRect;
		GetWindowRect(hWnd, &windowRect);
		int w = windowRect.right - windowRect.left;
		int h = windowRect.bottom - windowRect.top;
		HDC targetDC = GetWindowDC(hWnd);
		HDC hDC = CreateCompatibleDC(targetDC);
		HBITMAP tempPict = CreateCompatibleBitmap(targetDC, w, h);
		HGDIOBJ oldPict = SelectObject(hDC, tempPict);
		BitBlt(hDC, 0, 0, w, h, targetDC, 0, 0, SRCCOPY);
		tempPict = (HBITMAP) SelectObject(hDC, oldPict);
		QPixmap pm = QPixmap::fromWinHBITMAP(tempPict);

		DeleteDC(hDC);
		ReleaseDC(hWnd, targetDC);

		return pm;
	}

	HWND windowUnderCursor( bool includeDecorations = true ) {
		POINT pointCursor;
		QPoint qpointCursor = QCursor::pos();
		pointCursor.x = qpointCursor.x();
		pointCursor.y = qpointCursor.y();
		HWND windowUnderCursor = WindowFromPoint(pointCursor);

		if( includeDecorations ) {
			LONG_PTR style = GetWindowLongPtr( windowUnderCursor, GWL_STYLE );
			if( ( style & WS_CHILD ) != 0 ) {
				windowUnderCursor = GetAncestor( windowUnderCursor, GA_ROOT );
			}
		}
		return windowUnderCursor;
	}

	bool maybeAddWindow( HWND hwnd, std::vector< QRect > * windows) {
		WINDOWINFO wi;
		GetWindowInfo( hwnd, &wi );
		RECT rect = wi.rcClient;

	#if 0
		RECT rect;
		GetWindowRect( hwnd, &rect );
	#endif

		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		// For some reason, rect.left and rect.top are shifted by cxWindowBorders and cyWindowBorders pixels respectively
		// in *every* case (for every window), but cxWindowBorders and cyWindowBorders are non-zero only in the
		// biggest-window case, therefore we need to save the biggest cxWindowBorders and cyWindowBorders to adjust the rect later
		cxWindowBorder = qMax( cxWindowBorder, wi.cxWindowBorders );
		cyWindowBorder = qMax( cyWindowBorder, wi.cyWindowBorders );

		if ( ( ( wi.dwStyle & WS_VISIBLE ) != 0 ) && (width >= minSize ) && (height >= minSize ) ) {
			//QRect r( rect.left + 4, rect.top + 4, width, height); // 4 = max(wi.cxWindowBorders) = max(wi.cyWindowBorders)
			QRect r(rect.left + cxWindowBorder, rect.top + cyWindowBorder, width, height);
			if ( std::find( windows->begin(), windows->end(), r ) == windows->end() ) {
				windows->push_back( r );
				return true;
			}
		}
		return false;
	}

	BOOL CALLBACK getWindowsRecursiveHelper( HWND hwnd, LPARAM lParam ) {
		maybeAddWindow( hwnd, reinterpret_cast< std::vector< QRect > * >( lParam ) );
		return TRUE;
	}

	void getWindowsRecursive( std::vector< QRect > & windows, HWND hwnd, int rx = 0, int ry = 0, int depth = 0 ) {
		maybeAddWindow( hwnd, &windows );

		EnumChildWindows( hwnd, getWindowsRecursiveHelper, reinterpret_cast< LPARAM >( &windows ) );

		std::sort( windows.begin(), windows.end() );
	}

}

bool operator< ( const QRect& r1, const QRect& r2 ) {
	return r1.width() * r1.height() < r2.width() * r2.height();
}

std::tuple< QPixmap, QRect, std::vector< QRect > > qsnapshot::utility::grabWindow() {
	HWND child = windowUnderCursor();

	WINDOWINFO wi;
	GetWindowInfo( child, &wi);

	RECT r;
	GetWindowRect( child, &r);
	int x = r.left;
	int y = r.top;
	uint w = r.right - r.left;
	uint h = r.bottom - r.top;
	cxWindowBorder = wi.cxWindowBorders;
	cyWindowBorder = wi.cyWindowBorders;

	HDC childDC = GetDC(child);
	Q_UNUSED( childDC );

	QPixmap pm( ::grabWindow( child ) );
	std::vector< QRect > windows;
	getWindowsRecursive( windows, child );
	QRect windowGeometry( QPoint( r.left, r.top ), pm.size() );
	QPoint offset( r.left + wi.cxWindowBorders, r.top + wi.cyWindowBorders );
	std::for_each( windows.begin(), windows.end(), [&offset]( QRect & r )->void {
		r.translate( -offset );
	} );

	return std::make_tuple( pm, windowGeometry, windows );
}

std::tuple< QPixmap, QPoint > qsnapshot::utility::grabCurrent( bool includeDecorations ) {
	int x, y;
	HWND hWindow;
	hWindow = windowUnderCursor(includeDecorations);
	Q_ASSERT(hWindow);

	HWND hParent;

// Now find the top-most window
	do {
	  hParent = hWindow;
	} while( (hWindow = GetParent(hWindow)) != NULL );
	Q_ASSERT(hParent);

	RECT r;
	GetWindowRect(hParent, &r);

	x = r.left;
	y = r.top;

	QPixmap pm( ::grabWindow( hParent ) );
	return std::make_tuple( pm, QPoint( x, y ) );
}
