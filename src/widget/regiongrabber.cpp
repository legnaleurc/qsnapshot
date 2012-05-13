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
#include "regiongrabberprivate.hpp"

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QPainter>
#include <QtGui/QToolTip>
#include <QtGui/QMouseEvent>

#include <algorithm>

namespace {

	void drawRect( QPainter * painter, const QRect & r, const QColor & outline, const QColor & fill = QColor() ) {
		QRegion clip( r );
		clip = clip.subtracted( r.adjusted( 1, 1, -1, -1 ) );

		painter->save();
		painter->setClipRegion( clip );
		painter->setPen( Qt::NoPen );
		painter->setBrush( outline );
		painter->drawRect( r );
		if ( fill.isValid() ) {
			painter->setClipping( false );
			painter->setBrush( fill );
			painter->drawRect( r.adjusted( 1, 1, -1, -1 ) );
		}
		painter->restore();
	}

	const int HANDLE_SIZE = 10;

}

namespace i18n {

	const QString HELP_TEXT = QObject::tr( "Select a region using the mouse. To take the snapshot, press the Enter key or double click. Press Esc to quit." );

}

using namespace qsnapshot::widget;

RegionGrabber::Private::Private( RegionGrabber * host ):
host( host ),
pixmap(),
selection(),
selectionBeforeDrag(),
helpTextRect(),
dragStartPoint(),
mouseOverHandle( NULL ),
grabbing( false ),
showHelp( false ),
mouseDown( false ),
newSelection( false ),
TLHandle( 0, 0, HANDLE_SIZE, HANDLE_SIZE ),
TRHandle( 0, 0, HANDLE_SIZE, HANDLE_SIZE ),
BLHandle( 0, 0, HANDLE_SIZE, HANDLE_SIZE ),
BRHandle( 0, 0, HANDLE_SIZE, HANDLE_SIZE ),
LHandle( 0, 0, HANDLE_SIZE, HANDLE_SIZE ),
THandle( 0, 0, HANDLE_SIZE, HANDLE_SIZE ),
RHandle( 0, 0, HANDLE_SIZE, HANDLE_SIZE ),
BHandle( 0, 0, HANDLE_SIZE, HANDLE_SIZE ) {
	this->handles << &this->TLHandle << &this->TRHandle << &this->BLHandle << &this->BRHandle << &this->LHandle << &this->THandle << &this->RHandle << &this->BHandle;
}

void RegionGrabber::Private::grabRect() {
	QRect r = this->selection;
	if ( !r.isNull() && r.isValid() ) {
		this->finishGrab();
		emit this->regionGrabbed( this->pixmap.copy( r ) );
	}
}

QPoint RegionGrabber::Private::limitPointToRect( const QPoint & p, const QRect & r ) const {
	QPoint q;
	q.setX( p.x() < r.x() ? r.x() : p.x() < r.right() ? p.x() : r.right() );
	q.setY( p.y() < r.y() ? r.y() : p.y() < r.bottom() ? p.y() : r.bottom() );
	return q;
}

QRect RegionGrabber::Private::normalizeSelection( const QRect & r ) const {
	QRect s = r;
	if( s.width() <= 0 ) {
		int l = s.left();
		int w = s.width();
		s.setLeft( l + w - 1 );
		s.setRight( l );
	}
	if( s.height() <= 0) {
		int t = s.top();
		int h = s.height();
		s.setTop( t + h - 1 );
		s.setBottom( t );
	}
	return s;
}

QRegion RegionGrabber::Private::handleMask( MaskType type ) const {
	// note: not normalized QRects are bad here, since they will not be drawn
	QRegion mask;
	foreach( QRect * rect, this->handles ) {
		if( type == StrokeMask ) {
			QRegion r( *rect );
			mask += r.subtracted( rect->adjusted( 1, 1, -1, -1 ) );
		} else {
			mask += QRegion( rect->adjusted( 1, 1, -1, -1 ) );
		}
	}
	return mask;
}

void RegionGrabber::Private::updateHandles() {
	QRect r = this->selection;
	int s2 = HANDLE_SIZE / 2;

	this->TLHandle.moveTopLeft( r.topLeft() );
	this->TRHandle.moveTopRight( r.topRight() );
	this->BLHandle.moveBottomLeft( r.bottomLeft() );
	this->BRHandle.moveBottomRight( r.bottomRight() );

	this->LHandle.moveTopLeft( QPoint( r.x(), r.y() + r.height() / 2 - s2 ) );
	this->THandle.moveTopLeft( QPoint( r.x() + r.width() / 2 - s2, r.y() ) );
	this->RHandle.moveTopRight( QPoint( r.right(), r.y() + r.height() / 2 - s2 ) );
	this->BHandle.moveBottomLeft( QPoint( r.x() + r.width() / 2 - s2, r.bottom() ) );
}

void RegionGrabber::Private::finishGrab() {
	this->host->hide();
	this->host->releaseMouse();
	this->host->releaseKeyboard();
	std::for_each( std::begin( this->handles ), std::end( this->handles ), []( QRect * r )->void {
		r->moveTo( 0, 0 );
	} );
	this->selection = QRect();
}

RegionGrabber::RegionGrabber():
AbstractGrabber( 0, Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint ),
p_( new Private( this ) ) {
	this->setMouseTracking( true );

	this->connect( this->p_, SIGNAL( regionGrabbed( const QPixmap & ) ), SIGNAL( regionGrabbed( const QPixmap & ) ) );
}

void RegionGrabber::grab() {
	this->p_->pixmap = QPixmap::grabWindow( QApplication::desktop()->winId() );
	this->resize( this->p_->pixmap.size() );
	this->move( 0, 0 );
	this->setCursor( Qt::CrossCursor );
	this->show();
	this->grabMouse();
	this->grabKeyboard();
}

void RegionGrabber::paintEvent( QPaintEvent * /*event*/ ) {
	if( this->p_->grabbing ) {	// grabWindow() should just get the background
		return;
	}

	QPainter painter( this );

	QPalette palette( QToolTip::palette() );
	QFont font( QToolTip::font() );

	QColor handleColor = palette.color( QPalette::Active, QPalette::Highlight );
	handleColor.setAlpha( 160 );
	QColor overlayColor( 0, 0, 0, 160 );
	QColor textColor = palette.color( QPalette::Active, QPalette::Text );
	QColor textBackgroundColor = palette.color( QPalette::Active, QPalette::Base );
	painter.drawPixmap( 0, 0, this->p_->pixmap );
	painter.setFont( font );

	QRect r( this->p_->selection );
	if( !this->p_->selection.isNull() ) {
		QRegion grey( this->rect() );
		grey = grey.subtracted( r );
		painter.setClipRegion( grey );
		painter.setPen( Qt::NoPen );
		painter.setBrush( overlayColor );
		painter.drawRect( this->rect() );
		painter.setClipRect( this->rect() );
		drawRect( &painter, r, handleColor );
	} else {
		painter.setPen( Qt::NoPen );
		painter.setBrush( overlayColor );
		painter.drawRect( this->rect() );
	}

	if( this->p_->showHelp ) {
		painter.setPen( textColor );
		painter.setBrush( textBackgroundColor );
		this->p_->helpTextRect = painter.boundingRect( rect().adjusted( 2, 2, -2, -2 ), Qt::TextWordWrap, i18n::HELP_TEXT );
		this->p_->helpTextRect.adjust( -2, -2, 4, 2 );
		drawRect( &painter, this->p_->helpTextRect, textColor, textBackgroundColor );
		painter.drawText( this->p_->helpTextRect.adjusted( 3, 3, -3, -3 ), i18n::HELP_TEXT );
	}

	if( this->p_->selection.isNull() ) {
		return;
	}

	// The grabbed region is everything which is covered by the drawn
	// rectangles (border included). This means that there is no 0px
	// selection, since a 0px wide rectangle will always be drawn as a line.
	QString txt( QString( "%1x%2" ).arg( this->p_->selection.width() ).arg( this->p_->selection.height() ) );
	QRect textRect( painter.boundingRect( this->rect(), Qt::AlignLeft, txt ) );
	QRect boundingRect( textRect.adjusted( -4, 0, 0, 0 ) );

	if( textRect.width() < r.width() - 2 * HANDLE_SIZE
		&&
		textRect.height() < r.height() - 2 * HANDLE_SIZE
		&&
		( r.width() > 100 && r.height() > 100 )
	) {	// center, unsuitable for small selections
		boundingRect.moveCenter( r.center() );
		textRect.moveCenter( r.center() );
	} else if( r.y() - 3 > textRect.height()
			&&
			r.x() + textRect.width() < rect().right()
	) { // on top, left aligned
		boundingRect.moveBottomLeft( QPoint( r.x(), r.y() - 3 ) );
		textRect.moveBottomLeft( QPoint( r.x() + 2, r.y() - 3 ) );
	} else if( r.x() - 3 > textRect.width() ) { // left, top aligned
		boundingRect.moveTopRight( QPoint( r.x() - 3, r.y() ) );
		textRect.moveTopRight( QPoint( r.x() - 5, r.y() ) );
	} else if( r.bottom() + 3 + textRect.height() < rect().bottom()
		&&
		r.right() > textRect.width()
	) { // at bottom, right aligned
		boundingRect.moveTopRight( QPoint( r.right(), r.bottom() + 3 ) );
		textRect.moveTopRight( QPoint( r.right() - 2, r.bottom() + 3 ) );
	} else if( r.right() + textRect.width() + 3 < rect().width() ) { // right, bottom aligned
		boundingRect.moveBottomLeft( QPoint( r.right() + 3, r.bottom() ) );
		textRect.moveBottomLeft( QPoint( r.right() + 5, r.bottom() ) );
	}
	// if the above didn't catch it, you are running on a very tiny screen...
	drawRect( &painter, boundingRect, textColor, textBackgroundColor );

	painter.drawText( textRect, txt );

	if( ( r.height() > HANDLE_SIZE * 2 && r.width() > HANDLE_SIZE * 2 ) || !this->p_->mouseDown ) {
		this->p_->updateHandles();
		painter.setPen( Qt::NoPen );
		painter.setBrush( handleColor );
		painter.setClipRegion( this->p_->handleMask( Private::StrokeMask ) );
		painter.drawRect( rect() );
		handleColor.setAlpha( 60 );
		painter.setBrush( handleColor );
		painter.setClipRegion( this->p_->handleMask( Private::FillMask ) );
		painter.drawRect( this->rect() );
	}
}

void RegionGrabber::resizeEvent( QResizeEvent * /*event*/ ) {
	if( this->p_->selection.isNull() ) {
		return;
	}
	QRect r = this->p_->selection;
	r.setTopLeft( this->p_->limitPointToRect( r.topLeft(), rect() ) );
	r.setBottomRight( this->p_->limitPointToRect( r.bottomRight(), rect() ) );
	if( r.width() <= 1 || r.height() <= 1 ) //this just results in ugly drawing...
		this->p_->selection = QRect();
	else
		this->p_->selection = this->p_->normalizeSelection( r );
}

void RegionGrabber::mousePressEvent( QMouseEvent * event ) {
	this->p_->showHelp = !this->p_->helpTextRect.contains( event->pos() );
	if( event->button() == Qt::LeftButton ) {
		this->p_->mouseDown = true;
		this->p_->dragStartPoint = event->pos();
		this->p_->selectionBeforeDrag = this->p_->selection;
		if ( !this->p_->selection.contains( event->pos() ) ) {
			this->p_->newSelection = true;
			this->p_->selection = QRect();
		} else {
			this->setCursor( Qt::ClosedHandCursor );
		}
	} else if( event->button() == Qt::RightButton ) {
		this->p_->newSelection = false;
		this->p_->selection = QRect();
		this->setCursor( Qt::CrossCursor );
	}
	this->update();
}

void RegionGrabber::mouseMoveEvent( QMouseEvent * event ) {
	bool shouldShowHelp = !this->p_->helpTextRect.contains( event->pos() );
	if( shouldShowHelp != this->p_->showHelp ) {
		this->p_->showHelp = shouldShowHelp;
		this->update();
	}

	if( this->p_->mouseDown ) {
		if( this->p_->newSelection ) {
			QPoint p = event->pos();
			QRect r = this->rect();
			this->p_->selection = this->p_->normalizeSelection( QRect( this->p_->dragStartPoint, this->p_->limitPointToRect( p, r ) ) );
		} else if( this->p_->mouseOverHandle == NULL ) { // moving the whole selection
			QRect r = this->rect().normalized(), s = this->p_->selectionBeforeDrag.normalized();
			QPoint p = s.topLeft() + event->pos() - this->p_->dragStartPoint;
			r.setBottomRight( r.bottomRight() - QPoint( s.width(), s.height() ) + QPoint( 1, 1 ) );
			if ( !r.isNull() && r.isValid() ) {
				this->p_->selection.moveTo( this->p_->limitPointToRect( p, r ) );
			}
		} else { // dragging a handle
			QRect r = this->p_->selectionBeforeDrag;
			QPoint offset = event->pos() - this->p_->dragStartPoint;

			if( this->p_->mouseOverHandle == &this->p_->TLHandle || this->p_->mouseOverHandle == &this->p_->THandle || this->p_->mouseOverHandle == &this->p_->TRHandle ) {	// dragging one of the top handles
				r.setTop( r.top() + offset.y() );
			}

			if( this->p_->mouseOverHandle == &this->p_->TLHandle || this->p_->mouseOverHandle == &this->p_->LHandle || this->p_->mouseOverHandle == &this->p_->BLHandle ) {	// dragging one of the left handles
				r.setLeft( r.left() + offset.x() );
			}

			if( this->p_->mouseOverHandle == &this->p_->BLHandle || this->p_->mouseOverHandle == &this->p_->BHandle || this->p_->mouseOverHandle == &this->p_->BRHandle ) {	// dragging one of the bottom handles
				r.setBottom( r.bottom() + offset.y() );
			}

			if( this->p_->mouseOverHandle == &this->p_->TRHandle || this->p_->mouseOverHandle == &this->p_->RHandle || this->p_->mouseOverHandle == &this->p_->BRHandle ) {	// dragging one of the right handles
				r.setRight( r.right() + offset.x() );
			}
			r.setTopLeft( this->p_->limitPointToRect( r.topLeft(), rect() ) );
			r.setBottomRight( this->p_->limitPointToRect( r.bottomRight(), rect() ) );
			this->p_->selection = this->p_->normalizeSelection(r);
		}
		this->update();
	} else {
		if( this->p_->selection.isNull() ) {
			return;
		}
		bool found = false;
		foreach( QRect * r, this->p_->handles ) {
			if( r->contains( event->pos() ) ) {
				this->p_->mouseOverHandle = r;
				found = true;
				break;
			}
		}
		if ( !found ) {
			this->p_->mouseOverHandle = NULL;
			if ( this->p_->selection.contains( event->pos() ) )
				this->setCursor( Qt::OpenHandCursor );
			else
				this->setCursor( Qt::CrossCursor );
		} else {
			if ( this->p_->mouseOverHandle == &this->p_->TLHandle || this->p_->mouseOverHandle == &this->p_->BRHandle ) {
				this->setCursor( Qt::SizeFDiagCursor );
			}
			if ( this->p_->mouseOverHandle == &this->p_->TRHandle || this->p_->mouseOverHandle == &this->p_->BLHandle ) {
				this->setCursor( Qt::SizeBDiagCursor );
			}
			if ( this->p_->mouseOverHandle == &this->p_->LHandle || this->p_->mouseOverHandle == &this->p_->RHandle ) {
				this->setCursor( Qt::SizeHorCursor );
			}
			if ( this->p_->mouseOverHandle == &this->p_->THandle || this->p_->mouseOverHandle == &this->p_->BHandle ) {
				this->setCursor( Qt::SizeVerCursor );
			}
		}
	}
}

void RegionGrabber::mouseReleaseEvent( QMouseEvent * event ) {
	this->p_->mouseDown = false;
	this->p_->newSelection = false;
	if( this->p_->mouseOverHandle == 0 && this->p_->selection.contains( event->pos() ) ) {
		this->setCursor( Qt::OpenHandCursor );
	}
	this->update();
}

void RegionGrabber::mouseDoubleClickEvent( QMouseEvent * /*event*/ ) {
	this->p_->grabRect();
}

void RegionGrabber::keyPressEvent( QKeyEvent * event ) {
	if( event->key() == Qt::Key_Escape ) {
		this->p_->finishGrab();
		emit this->regionGrabbed( QPixmap() );
	} else if ( event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return ) {
		this->p_->grabRect();
	} else {
		event->ignore();
	}
}
