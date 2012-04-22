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
#include "windowgrabberprivate.hpp"
#include "utility/uiinspector.hpp"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

using namespace qsnapshot::widget;
using qsnapshot::utility::grabWindow;

WindowGrabber::Private::Private( WindowGrabber * host ):
QObject( host ),
host( host ),
windows(),
current( -1 ),
yPos( -1 ),
windowPosition() {
}

// Searches and returns the index of the first (=smallest) window
// containing the mouse pointer.
int WindowGrabber::Private::windowIndex( const QPoint & pos ) const {
	for( std::size_t i = 0; i < this->windows.size(); ++i ) {
		if( this->windows.at( i ).contains( pos ) ) {
			return i;
		}
	}
	return -1;
}

// Increases the scope to the next-bigger window containing the mouse pointer.
// This method is activated by either rotating the mouse wheel forwards or by
// dragging the mouse forwards while keeping the right mouse button pressed.
void WindowGrabber::Private::increaseScope( const QPoint & pos ) {
	for ( std::size_t i = this->current + 1; i < this->windows.size(); ++i ) {
		if ( this->windows[ i ].contains( pos ) ) {
			current = i;
			break;
		}
	}
	this->host->repaint();
}

// Decreases the scope to the next-smaller window containing the mouse pointer.
// This method is activated by either rotating the mouse wheel backwards or by
// dragging the mouse backwards while keeping the right mouse button pressed.
void WindowGrabber::Private::decreaseScope( const QPoint & pos ) {
	for( int i = this->current - 1; i >= 0; --i ) {
		if( this->windows[ i ].contains( pos ) ) {
			this->current = i;
			break;
		}
	}
	this->host->repaint();
}

void WindowGrabber::Private::finishGrab() {
	this->host->hide();
}

std::tuple< QPixmap, QPoint > WindowGrabber::grabCurrent( bool includeDecorations ) {
	return qsnapshot::utility::grabCurrent( includeDecorations );
}

WindowGrabber::WindowGrabber( QWidget * parent ):
AbstractGrabber( parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint ),
p_( new Private( this ) ) {
	this->setWindowModality( Qt::WindowModal );
}

void WindowGrabber::grab() {
	this->p_->windows.clear();
	std::tuple< QPixmap, QRect, std::vector< QRect > > result( grabWindow() );
	this->p_->windows = std::get< 2 >( result );
	this->show();
	this->setGeometry( std::get< 1 >( result ) );
	QPalette p = this->palette();
	p.setBrush( this->backgroundRole(), QBrush( std::get< 0 >( result ) ) );
	this->setPalette( p );
	this->setFixedSize( std::get< 0 >( result ).size() );
	this->setMouseTracking( true );
	this->p_->current = this->p_->windowIndex( this->mapFromGlobal( QCursor::pos() ) );
}

// Draws a border around the (child) window currently containing the pointer
void WindowGrabber::paintEvent( QPaintEvent * /*event*/ ) {
	if( this->p_->current < 0 ) {
		return;
	}
	QPainter p;
	p.begin( this );
	p.fillRect( this->rect(), this->palette().brush( this->backgroundRole()));
	p.setPen( QPen( Qt::red, 3 ) );
	p.drawRect( this->p_->windows[ this->p_->current ].adjusted( 0, 0, -1, -1 ) );
	p.end();
}

void WindowGrabber::mousePressEvent( QMouseEvent * event ) {
	if( event->button() == Qt::RightButton ) {
		this->p_->yPos = event->globalY();
	} else {
		if ( this->p_->current >= 0 ) {
			this->p_->windowPosition = event->globalPos() - event->pos() + this->p_->windows[this->p_->current].topLeft();
			emit this->windowGrabbed( this->palette().brush( this->backgroundRole() ).texture().copy( this->p_->windows[ this->p_->current ] ) );
		} else {
			this->p_->windowPosition = QPoint(0,0);
			emit this->windowGrabbed( QPixmap() );
		}
		this->p_->finishGrab();
	}
}

void WindowGrabber::mouseReleaseEvent( QMouseEvent * event ) {
	if( event->button() == Qt::RightButton ) {
		this->p_->yPos = -1;
	}
}

static
const int minDistance = 10;

void WindowGrabber::mouseMoveEvent( QMouseEvent * event ) {
	if( this->p_->yPos == -1 ) {
		int w = this->p_->windowIndex( event->pos() );
		if( w != -1 && w != this->p_->current ) {
			this->p_->current = w;
			this->repaint();
		}
	} else {
		int y = event->globalY();
		if( y > this->p_->yPos + minDistance ) {
			this->p_->decreaseScope( event->pos() );
			this->p_->yPos = y;
		} else if( y < this->p_->yPos - minDistance ) {
			this->p_->increaseScope( event->pos() );
			this->p_->yPos = y;
		}
	}
}

void WindowGrabber::wheelEvent( QWheelEvent * event ) {
	if ( event->delta() > 0 ) {
		this->p_->increaseScope( event->pos() );
	} else if ( event->delta() < 0 ) {
		this->p_->decreaseScope( event->pos() );
	} else {
		event->ignore();
	}
}
