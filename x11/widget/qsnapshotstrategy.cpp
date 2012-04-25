#include "qsnapshotstrategy.hpp"

#include <X11/Xlib.h>

#include <QtGui/QDesktopWidget>
#include <QtGui/QX11Info>

using qsnapshot::widget::QSnapshot;

QSnapshot::Strategy::Strategy( QSnapshot * host ):
QSnapshot::Private( host ),
compositing( false ) {
	Display * dpy = QX11Info::display();
	char net_wm_cm_name[20];
	snprintf( net_wm_cm_name, 20, "_NET_WM_CM_S%d", DefaultScreen( dpy ) );
	Atom net_wm_cm = XInternAtom( dpy, net_wm_cm_name, 0 );
	Window wm_owner = XGetSelectionOwner( dpy, net_wm_cm );
	this->compositing = wm_owner != None;
}

void QSnapshot::Strategy::fastHide() {
	// NOTE dirty hack
	this->host->hide();
	if( this->compositing ) {
		this->host->move( -10000, -10000 );
		delayGUI( 200 );
	}
}

void QSnapshot::Strategy::fastShow() {
	// TODO restore original position
	this->host->move( 0, 0 );
	this->host->move( QApplication::desktop()->geometry().center() - this->host->geometry().center() );
	this->host->show();
}
