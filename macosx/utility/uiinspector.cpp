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
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

#include "utility/uiinspector.hpp"
#include "inspector.hpp"

Rectangle::Rectangle( int x, int y, int w, int h ):
r( new QRect( x, y, w, h ) ) {
}

Rectangle::Rectangle( const Rectangle & that ):
r( new QRect( *that.r ) ) {
}

Rectangle & Rectangle::operator =( const Rectangle & that ) {
	if( &that != this ) {
		*this->r = *that.r;
	}
	return *this;
}

Rectangle::~Rectangle() {
	delete this->r;
}

Rectangle Rectangle::intersected( const Rectangle & that ) const {
	auto r = this->r->intersected( *that.r );
	return Rectangle( r.x(), r.y(), r.width(), r.height() );
}

bool Rectangle::isEmpty() const {
	return this->r->isEmpty();
}

int Rectangle::x() const {
	return this->r->x();
}

int Rectangle::y() const {
	return this->r->y();
}

int Rectangle::width() const {
	return this->r->width();
}

int Rectangle::height() const {
	return this->r->height();
}

int getScreenHeight() {
	return QApplication::desktop()->screenGeometry().height();
}

std::tuple< QPixmap, QRect, std::vector< QRect > > qsnapshot::utility::grabWindow() {
	Grabber g;
	if ( g.isEmpty() ) {
		// TODO throw
	}

	g.traverse();
	std::vector< QRect > windows;
	std::transform( std::begin( g.windows ), std::end( g.windows ), std::back_inserter( windows ), []( const Rectangle & r )->QRect {
		return *r.r;
	} );

	const auto & r = windows.back();
	QPixmap pm = QPixmap::grabWindow( QApplication::desktop()->winId(), r.x(), r.y(), r.width(), r.height() );
	QPoint offset = r.topLeft();
	QRect windowGeometry( offset, pm.size() );
	std::for_each( windows.begin(), windows.end(), [&offset]( QRect & r )->void {
		r.translate( -offset );
	} );
	return std::make_tuple( pm, windowGeometry, windows );
}

std::tuple< QPixmap, QPoint > qsnapshot::utility::grabCurrent( bool /*includeDecorations*/ ) {
	Grabber g;
	if( g.isEmpty() ) {
		// TODO throw
	}

	const auto & r = *g.windows.back().r;
	QPixmap pm = QPixmap::grabWindow( QApplication::desktop()->winId(), r.x(), r.y(), r.width(), r.height() );
	QPoint offset = r.topLeft();

	return std::make_tuple( pm, offset );
}
