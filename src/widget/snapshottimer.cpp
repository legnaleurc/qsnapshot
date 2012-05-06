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
#include "snapshottimerprivate.hpp"

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QPainter>
#include <QtGui/QToolTip>

namespace i18n {

	const QString HELP_TEXT = QObject::tr( "Snapshot will be taken in %1 second(s)", "" );

}

using namespace qsnapshot::widget;

SnapshotTimer::Private::Private( SnapshotTimer * host ):
QObject( host ),
timer( new QTimer( this ) ),
time( 0 ),
length( 0 ),
toggle( true ){
}

SnapshotTimer::SnapshotTimer( QWidget * parent ):
QWidget( parent ),
p_( new Private( this ) ) {
	this->setWindowFlags( Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint );
	QFontMetrics m( this->fontMetrics() );
	this->resize( m.width( i18n::HELP_TEXT.arg( 99 ) ) + 6, m.height() + 4 );

	this->connect( this->p_->timer, SIGNAL( timeout() ), SLOT( bell() ) );
}

void SnapshotTimer::start( int seconds ) {
	const QRect SCREEN_GEOMETRY( qApp->desktop()->screenGeometry() );
	this->move( SCREEN_GEOMETRY.width() / 2 - this->size().width() / 2, SCREEN_GEOMETRY.top() );
	this->p_->toggle = true;
	this->p_->time = 0;
	this->p_->length = seconds;
	this->p_->timer->start( 1000 );
	this->show();
}

void SnapshotTimer::stop() {
	this->hide();
	this->p_->timer->stop();
}

void SnapshotTimer::bell() {
	if( this->p_->time == this->p_->length - 1 ) {
		this->hide();
	} else if( this->p_->time == this->p_->length ) {
		emit this->timeout();
		this->p_->timer->stop();
	}
	++this->p_->time;
	this->p_->toggle = !this->p_->toggle;
	this->update();
}

void SnapshotTimer::enterEvent( QEvent * /*event*/ ) {
	const QRect SCREEN_GEOMETRY( qApp->desktop()->screenGeometry() );
	if( this->x() == SCREEN_GEOMETRY.left() ) {
		this->move( SCREEN_GEOMETRY.x() + ( SCREEN_GEOMETRY.width() / 2 - this->size().width() / 2 ), SCREEN_GEOMETRY.top() );
	} else {
		this->move( SCREEN_GEOMETRY.topLeft() );
	}
}

void SnapshotTimer::paintEvent( QPaintEvent * /*event*/ ) {
	if( this->p_->time >= this->p_->length ) {
		return;
	}

	QPainter painter( this );
	QPalette palette( QToolTip::palette() );
	QColor handleColor( palette.color( QPalette::Active, QPalette::Highlight ) );
	handleColor.setAlpha( 160 );
//	QColor overlayColor( 0, 0, 0, 160 );
	QColor textColor = palette.color( QPalette::Active, QPalette::Text );
	QColor textBackgroundColor = palette.color( QPalette::Active, QPalette::Base );
	if( this->p_->toggle ) {
		textColor = palette.color( QPalette::Active, QPalette::Text );
	} else {
		textColor = palette.color( QPalette::Active, QPalette::Base );
	}
	painter.setPen( textColor );
	painter.setBrush( textBackgroundColor );
	QString helpText = i18n::HELP_TEXT.arg( this->p_->length - this->p_->time );
	this->p_->textRect = painter.boundingRect( this->rect().adjusted( 2, 2, -2, -2 ), Qt::AlignHCenter | Qt::TextSingleLine, helpText );
	painter.drawText( this->p_->textRect, helpText );
}
