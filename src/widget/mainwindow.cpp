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
#include "mainwindowprivate.hpp"

#include <QtGui/QDesktopWidget>
#include <QtCore/QTimer>
#include <QtGui/QMouseEvent>

#include <QtCore/QtDebug>

namespace {
const int ChildWindow = 3;
const int WindowUnderCursor = 1;
const int CurrentScreen = 0;
const int Region_ = 2;
}

using namespace qsnapshot::widget;

MainWindow::Private::Private( MainWindow * host ):
QObject( host ),
host( host ),
ui(),
// NOTE Windows and Mac OS X flag
grabber( new QWidget( 0, Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint ) ),
grabTimer( new SnapshotTimer( host ) ),
regionGrabber( new RegionGrabber( this->host ) ),
windowGrabber( new WindowGrabber( 0 ) ),
snapshot(),
pixmap(),
savedPosition(),
modified( false ) {
	this->ui.setupUi( host );

	this->ui.snapshotDelay->setSuffix( QObject::tr( " second(s)", "" ) );

//	this->grabber->move( -10000, -10000 );
	this->grabber->move( 0, 0 );
	this->grabber->setWindowOpacity( 0.1 );
	this->grabber->resize( QApplication::desktop()->screenGeometry().size() );

#ifdef HAVE_X11_EXTENSIONS_XFIXES_H
	{
		int tmp1, tmp2;
		//Check whether the XFixes extension is available
		Display *dpy = QX11Info::display();
		if (!XFixesQueryExtension( dpy, &tmp1, &tmp2 )) {
			mainWidget->cbIncludePointer->hide();
			mainWidget->lblIncludePointer->hide();
		}
	}
#elif !defined(Q_WS_WIN)
//	this->ui.cbIncludePointer->hide();
//	this->ui.lblIncludePointer->hide();
#endif

	if( qApp->desktop()->numScreens() < 2 ) {
//		this->ui->comboMode->removeItem(CurrentScreen);
	}

	this->connect( this->ui.newSnapshot, SIGNAL( clicked() ), SLOT( grab() ) );
	this->connect( this->grabTimer, SIGNAL( timeout() ), SLOT( onGrabTimerTimeout() ) );
	this->connect( this->regionGrabber, SIGNAL( regionGrabbed( const QPixmap & ) ), SLOT( onRegionGrabbed( const QPixmap & ) ) );
	this->connect( this->windowGrabber.get(), SIGNAL( windowGrabbed( const QPixmap & ) ), SLOT( onWindowGrabbed( const QPixmap & ) ) );
}

void MainWindow::Private::grab() {
	this->savedPosition = this->host->pos();
	this->host->hide();

	if( this->delay() > 0 ) {
		this->grabTimer->start( this->delay() );
	} else {
		QTimer::singleShot( 0, this, SLOT( startUndelayedGrab() ) );
	}
}

// FIXME polymorphithm
void MainWindow::Private::grabRegion() {
	this->regionGrabber->grab();
}

void MainWindow::Private::performGrab() {
	int x = 0;
	int y = 0;

	this->grabber->releaseMouse();
	this->grabber->hide();
	this->grabTimer->stop();

//	title.clear();
//	windowClass.clear();

	// TODO command pattern
	if( this->mode() == ChildWindow ) {
		this->windowGrabber->grab();
		// TODO event scope
//		QPoint offset = WindowGrabber::lastWindowPosition();
//		x = offset.x();
//		y = offset.y();
//		qDebug() << "last window position is" << offset;
	} else if ( this->mode() == WindowUnderCursor ) {
		std::tuple< QPixmap, QPoint > t( WindowGrabber::grabCurrent( this->includeDecorations() ) );
		this->snapshot = std::get< 0 >( t );

		QPoint offset = std::get< 1 >( t );
		x = offset.x();
		y = offset.y();

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
#ifdef HAVE_X11_EXTENSIONS_XFIXES_H
	if( haveXFixes && includePointer() ) {
		grabPointerImage(x, y);
	}
#endif

	this->updatePreview();
	QApplication::restoreOverrideCursor();
	this->modified = true;
//	updateCaption();
	if( this->savedPosition != QPoint( -1, -1 ) ) {
		this->host->move( this->savedPosition );
	}
	this->host->show();
}

void MainWindow::Private::updatePreview() {
	this->setPreview( this->snapshot );
}

void MainWindow::Private::setPreview( const QPixmap & pixmap ) {
	this->ui.preview->setToolTip( QObject::tr( "Preview of the snapshot image (%1 x %2)" ).arg( pixmap.width() ).arg( pixmap.height() ) );

	this->ui.preview->setPixmap( pixmap.scaled( this->ui.preview->size(), Qt::KeepAspectRatio ) );
	this->ui.preview->adjustSize();
}

int MainWindow::Private::delay() const {
	return this->ui.snapshotDelay->value();
}

int MainWindow::Private::mode() const {
	return this->ui.captureMode->currentIndex();
}

bool MainWindow::Private::includeDecorations() const {
	return this->ui.includeDecorations->isChecked();
}

void MainWindow::Private::startUndelayedGrab() {
	if( this->mode() == Region_ ) {
		this->grabRegion();
	} else if( this->mode() == CurrentScreen ) {
		this->performGrab();
	} else {
		this->grabber->show();
		this->grabber->grabMouse( Qt::CrossCursor );
	}
}

void MainWindow::Private::onRegionGrabbed( const QPixmap & p ) {
	if( !p.isNull() ) {
		this->snapshot = p;
		this->updatePreview();
		modified = true;
//		updateCaption();
	}

	// FIXME polymorphithm
//	if( this->mode() == KSnapshotObject::Region ) {
//		this->regionGrabber->deleteLater();
//	}

	QApplication::restoreOverrideCursor();
	this->host->show();
}

void MainWindow::Private::onWindowGrabbed( const QPixmap & p ) {
	if ( !p.isNull() ) {
		this->snapshot = p;
		this->updatePreview();
		this->modified = true;
//		updateCaption();
	}

	QApplication::restoreOverrideCursor();
	this->host->show();
}

void MainWindow::Private::onGrabTimerTimeout() {
	if( this->mode() == Region_ ) {
		this->grabRegion();
	} else {
		this->performGrab();
	}
	// TODO system notify
//	KNotification::beep(i18n("The screen has been successfully grabbed."));
}

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
p_( new Private( this ) ) {
	// NOTE somehow eventFilter will be triggered between
	// new Private( this ) and p_ = new Private
	this->p_->grabber->installEventFilter( this );
}

void MainWindow::changeEvent( QEvent * e ) {
	this->QMainWindow::changeEvent( e );
	switch( e->type() ) {
	case QEvent::LanguageChange:
		this->p_->ui.retranslateUi( this );
		break;
	default:
		break;
	}
}

bool MainWindow::eventFilter( QObject * object, QEvent * event ) {
	if( object == this->p_->grabber.get() && event->type() == QEvent::MouseButtonPress ) {
		QMouseEvent * me = dynamic_cast< QMouseEvent * >( event );
		if( this->QWidget::mouseGrabber() != this->p_->grabber.get() ) {
			return false;
		}
		if( me->button() == Qt::LeftButton ) {
			this->p_->performGrab();
		}
	}
	return false;
}
