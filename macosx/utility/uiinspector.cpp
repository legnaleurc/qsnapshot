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
#include "inspector.h"

#include <QtCore/QDataStream>

std::tuple< QPixmap, QRect, std::vector< QRect > > qsnapshot::utility::grabWindow() {
	char * data = NULL;
	int nData = ::grabWindow( &data );
	QByteArray buffer( data, nData );
	std::free( data );
	QDataStream sin( buffer );
	QPixmap pm;
	QVector< QRect > windows;
	sin >> pm >> windows;

	QPoint offset( windows.back().topLeft() );
	QRect windowGeometry( offset, pm.size() );
	std::for_each( windows.begin(), windows.end(), [&offset]( QRect & r )->void {
		r.translate( -offset );
	} );
	return std::make_tuple( pm, windowGeometry, windows.toStdVector() );
}

std::tuple< QPixmap, QPoint > qsnapshot::utility::grabCurrent( bool includeDecorations ) {
	char * data = NULL;
	int nData = ::grabCurrent( &data, includeDecorations );
	QByteArray buffer( data, nData );
	std::free( data );
	QDataStream sin( buffer );
	QPixmap pm;
	QPoint offset;
	sin >> pm >> offset;
	return std::make_tuple( pm, offset );
}
