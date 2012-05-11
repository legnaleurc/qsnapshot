#include "x11strategy.hpp"

#include <X11/Xlib.h>

#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtGui/QDesktopWidget>
#include <QtGui/QX11Info>

using qsnapshot::widget::QSnapshot;
using qsnapshot::widget::X11Strategy;

namespace {

	bool inject() {
		QSnapshot::Strategy::creator() = []( QSnapshot * host )->QSnapshot::Strategy * {
			return new X11Strategy( host );
		};
		return true;
	}

	const bool HACK = inject();

	void delayGUI( int msec ) {
		QEventLoop wait;
		QTimer::singleShot( msec, &wait, SLOT( quit() ) );
		wait.exec();
	}

}

X11Strategy::X11Strategy( QSnapshot * host ):
QSnapshot::Strategy( host ),
compositing( false ),
origPos( host->pos() ) {
	Display * dpy = QX11Info::display();
	char net_wm_cm_name[20];
	snprintf( net_wm_cm_name, 20, "_NET_WM_CM_S%d", DefaultScreen( dpy ) );
	Atom net_wm_cm = XInternAtom( dpy, net_wm_cm_name, 0 );
	Window wm_owner = XGetSelectionOwner( dpy, net_wm_cm );
	this->compositing = wm_owner != None;
}

void X11Strategy::fastHide() {
	this->host->hide();
	// NOTE dirty hack to avoid those window effects provide by window managers
	if( this->compositing ) {
		this->origPos = this->host->pos();
		this->host->move( -10000, -10000 );
		delayGUI( 200 );
	}
}

void X11Strategy::fastShow() {
	this->host->move( this->origPos );
	this->host->show();
}

/*
void X11Strategy::postNew() {
	// to avoid grabber remain on screen
	this->grabber->setWindowOpacity( 0.0 );
#ifdef HAVE_X11_EXTENSIONS_XFIXES_H
	int tmp1, tmp2;
	//Check whether the XFixes extension is available
	Display *dpy = QX11Info::display();
	if (!XFixesQueryExtension( dpy, &tmp1, &tmp2 )) {
		mainWidget->cbIncludePointer->hide();
		mainWidget->lblIncludePointer->hide();
	}
#endif
}
*/
