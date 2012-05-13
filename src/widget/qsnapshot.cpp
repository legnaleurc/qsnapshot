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
#include "qsnapshotprivate.hpp"
#include "savingdialog.hpp"

#include <QtCore/QTimer>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMessageBox>
#include <QtGui/QMouseEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QClipboard>

namespace {

	const int ChildWindow = 3;
	const int WindowUnderCursor = 1;
	const int CurrentScreen = 0;
	const int Region_ = 2;

	void delayGUI( int msec ) {
		QEventLoop wait;
		QTimer::singleShot( msec, &wait, SLOT( quit() ) );
		wait.exec();
	}

}

namespace i18n {

	const QString DELAY_SUFFIX = QObject::tr( " second(s)", "" );
	const QString REGION_TIP = QObject::tr( "Preview of the snapshot image (%1 x %2)" );
	const QString HELP_TITLE = QObject::tr( "About QSnapshot" );
	const QString VERSION = QObject::tr( "Version" );

}

using namespace qsnapshot::widget;

QSnapshot::Private::Private( QSnapshot * host ):
QObject( host ),
host( host ),
ui(),
strategy( Strategy::createInstance( host ) ),
// NOTE Windows and Mac OS X flag
grabber( new FocusGrabber ),
grabTimer( new SnapshotTimer( host ) ),
regionGrabber( new RegionGrabber ),
windowGrabber( new WindowGrabber( 0 ) ),
snapshot(),
savedPosition(),
modified( false ) {
	this->ui.setupUi( host );

	this->ui.snapshotDelay->setSuffix( i18n::DELAY_SUFFIX );

	if( qApp->desktop()->numScreens() < 2 ) {
//		this->ui->comboMode->removeItem(CurrentScreen);
	}

	this->connect( this->ui.newSnapshot, SIGNAL( clicked() ), SLOT( grab() ) );
	this->connect( this->ui.saveAs, SIGNAL( clicked() ), SLOT( onSaveAs() ) );
	this->connect( this->ui.copy, SIGNAL( clicked() ), SLOT( onCopy() ) );
	this->connect( this->ui.help, SIGNAL( clicked() ), SLOT( onHelp() ) );
	this->connect( this->grabber.get(), SIGNAL( clicked() ), SLOT( performGrab() ) );
	this->connect( this->grabTimer, SIGNAL( timeout() ), SLOT( startGrab() ) );
	this->connect( this->regionGrabber.get(), SIGNAL( regionGrabbed( const QPixmap & ) ), SLOT( onRegionGrabbed( const QPixmap & ) ) );
	this->connect( this->windowGrabber.get(), SIGNAL( windowGrabbed( const QPixmap & ) ), SLOT( onWindowGrabbed( const QPixmap & ) ) );
}

void QSnapshot::Private::onSaveAs() {
	if( this->snapshot.isNull() ) {
		return;
	}
	QString filePath( getSaveFileName( this->host ) );
	this->snapshot.save( filePath );
}

void QSnapshot::Private::onCopy() {
	QClipboard * cb = QApplication::clipboard();
	cb->setPixmap( this->snapshot );
}

void QSnapshot::Private::onHelp() {
	QMessageBox::information( this->host, i18n::HELP_TITLE, QString(
		"<table>"
			"<tr><th>%1</th><td>%2</td></tr>"
		"</table>" ).arg( i18n::VERSION ).arg( qApp->applicationVersion() ) );
}

void QSnapshot::Private::grab() {
	this->savedPosition = this->host->pos();
	this->strategy->fastHide();

	if( this->delay() > 0 ) {
		this->grabTimer->start( this->delay() );
	} else {
		this->startGrab();
	}
}

// FIXME polymorphithm
void QSnapshot::Private::grabRegion() {
	this->regionGrabber->grab();
}

void QSnapshot::Private::performGrab() {
	this->grabber->releaseMouse();
	this->grabber->hide();
	// NOTE this is a dirty hack, wait a little time for grabber layer really hidden
	delayGUI( 20 );
	this->grabTimer->stop();

	// TODO command pattern
	if( this->mode() == ChildWindow ) {
		this->windowGrabber->grab();
	} else if ( this->mode() == WindowUnderCursor ) {
		std::tuple< QPixmap, QPoint > t( WindowGrabber::grabCurrent( this->includeDecorations() ) );
		this->snapshot = std::get< 0 >( t );

		// If we're showing decorations anyway then we'll add the title and window
		// class to the output image meta data.
		if( this->includeDecorations() ) {
//			title = WindowGrabber::lastWindowTitle();
//			windowClass = WindowGrabber::lastWindowClass();
		}
//	} else if ( this->mode() == CurrentScreen ) {
//		QDesktopWidget * desktop = QApplication::desktop();
//		int screenId = desktop->screenNumber( QCursor::pos() );
//		QRect geom = desktop->screenGeometry( screenId );
//		x = geom.x();
//		y = geom.y();
//		this->snapshot = QPixmap::grabWindow( desktop->winId(), x, y, geom.width(), geom.height() );
	} else {
		this->snapshot = QPixmap::grabWindow( QApplication::desktop()->winId() );
	}

	this->updatePreview();
	QApplication::restoreOverrideCursor();
	this->modified = true;
	if( this->savedPosition != QPoint( -1, -1 ) ) {
		this->host->move( this->savedPosition );
	}
	this->strategy->fastShow();
}

void QSnapshot::Private::updatePreview() {
	this->setPreview( this->snapshot );
}

void QSnapshot::Private::setPreview( const QPixmap & pixmap ) {
	this->ui.preview->setToolTip( i18n::REGION_TIP.arg( pixmap.width() ).arg( pixmap.height() ) );

	this->ui.preview->setPixmap( pixmap.scaled( this->ui.preview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	this->ui.preview->adjustSize();
}

int QSnapshot::Private::delay() const {
	return this->ui.snapshotDelay->value();
}

int QSnapshot::Private::mode() const {
	return this->ui.captureMode->currentIndex();
}

bool QSnapshot::Private::includeDecorations() const {
	return this->ui.includeDecorations->isChecked();
}

void QSnapshot::Private::onRegionGrabbed( const QPixmap & p ) {
	if( !p.isNull() ) {
		this->snapshot = p;
		this->updatePreview();
		modified = true;
	}

	QApplication::restoreOverrideCursor();
	this->strategy->fastShow();
}

void QSnapshot::Private::onWindowGrabbed( const QPixmap & p ) {
	if ( !p.isNull() ) {
		this->snapshot = p;
		this->updatePreview();
		this->modified = true;
	}

	QApplication::restoreOverrideCursor();
	this->strategy->fastShow();
}

void QSnapshot::Private::startGrab() {
	// NOTE this is a dirty hack, wait a little time for main dialog really hidden
	delayGUI( 20 );

	if( this->mode() == Region_ ) {
		this->grabRegion();
	} else if( this->mode() == CurrentScreen ) {
		this->performGrab();
	} else {
		this->grabber->show();
		this->grabber->grabMouse( Qt::CrossCursor );
	}
	// TODO system notify
//	KNotification::beep(i18n("The screen has been successfully grabbed."));
}

QSnapshot::QSnapshot() :
QWidget(),
p_( new Private( this ) ) {
}

void QSnapshot::changeEvent( QEvent * e ) {
	this->QWidget::changeEvent( e );
	switch( e->type() ) {
	case QEvent::LanguageChange:
		this->p_->ui.retranslateUi( this );
		break;
	default:
		break;
	}
}
