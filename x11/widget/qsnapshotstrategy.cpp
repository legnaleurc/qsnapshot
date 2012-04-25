#include "qsnapshotstrategy.hpp"

#include <X11/Xlib.h>

#include <QtGui/QDesktopWidget>
#include <QtGui/QX11Info>

using qsnapshot::widget::QSnapshot;

const bool QSnapshot::Strategy::HACK = QSnapshot::Strategy::inject();

bool QSnapshot::Strategy::inject() {
	QSnapshot::Private::creator() = []( QSnapshot * host )->QSnapshot::Private * {
		return new QSnapshot::Strategy( host );
	};
	return true;
}

QSnapshot::Strategy::Strategy( QSnapshot * host ):
QSnapshot::Private( host ),
compositing( false ),
origPos( host->pos() ) {
	Display * dpy = QX11Info::display();
	char net_wm_cm_name[20];
	snprintf( net_wm_cm_name, 20, "_NET_WM_CM_S%d", DefaultScreen( dpy ) );
	Atom net_wm_cm = XInternAtom( dpy, net_wm_cm_name, 0 );
	Window wm_owner = XGetSelectionOwner( dpy, net_wm_cm );
	this->compositing = wm_owner != None;
}

void QSnapshot::Strategy::fastHide() {
	this->host->hide();
	// NOTE dirty hack to avoid those window effects provide by window managers
	if( this->compositing ) {
		this->origPos = this->host->pos();
		this->host->move( -10000, -10000 );
		delayGUI( 200 );
	}
}

void QSnapshot::Strategy::fastShow() {
	this->host->move( this->origPos );
	this->host->show();
}
