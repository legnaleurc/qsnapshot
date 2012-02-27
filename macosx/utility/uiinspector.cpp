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
#include "inspector.hpp"

std::tuple< QPixmap, QRect, std::vector< QRect > > qsnapshot::utility::grabWindow() {
	Inspector inspector;
	std::vector< QRect > windows;
	QPixmap pm( inspector.grabWindow( windows ) );
	QPoint offset( windows.back().topLeft() );
	QRect windowGeometry( offset, pm.size() );
	std::for_each( windows.begin(), windows.end(), [&offset]( QRect & r )->void {
		r.translate( -offset );
	} );
	return std::make_tuple( pm, windowGeometry, windows );
}

std::tuple< QPixmap, QPoint > qsnapshot::utility::grabCurrent( bool includeDecorations ) {
	Inspector inspector;
	QPair< QPixmap, QPoint > ppp( inspector.grabCurrent( includeDecorations ) );
	return std::make_tuple( ppp.first, ppp.second );
}
