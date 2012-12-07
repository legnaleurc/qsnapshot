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
#ifndef QSNAPSHOT_UTILITY_INSPECTOR_HPP
#define QSNAPSHOT_UTILITY_INSPECTOR_HPP

#include <vector>

class QRect;

struct Rectangle {
	Rectangle( int x, int y, int w, int h );
	Rectangle( const Rectangle & that );
	~Rectangle();
	Rectangle & operator =( const Rectangle & that );
	Rectangle intersected( const Rectangle & that ) const;
	bool isEmpty() const;
	int x() const;
	int y() const;
	int width() const;
	int height() const;
	QRect * r;
};

struct Grabber {
	Grabber();
	bool isEmpty() const;
	void traverse();
	std::vector< Rectangle > windows;
	const void * ui;
};

int getScreenHeight();

#endif
