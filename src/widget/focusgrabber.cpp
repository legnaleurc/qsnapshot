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
#include "focusgrabber.hpp"
#include "focusgrabberstrategy.hpp"

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMouseEvent>

using qsnapshot::widget::FocusGrabber;

FocusGrabber::FocusGrabber():
QWidget( 0, Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint ),
s_( Strategy::createInstance( this ) ) {
	this->move( 0, 0 );
	this->resize( QApplication::desktop()->screenGeometry().size() );
	QPalette p = this->palette();
	p.setBrush( this->backgroundRole(), QColor( 0, 0, 0 ) );
	this->setPalette( p );

	this->s_->postNew();
}

void FocusGrabber::mousePressEvent( QMouseEvent * event ) {
	if( event->button() == Qt::LeftButton ) {
		emit this->clicked();
	}
}
