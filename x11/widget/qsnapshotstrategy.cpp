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
#include "qsnapshotstrategy.hpp"

#include <X11/Xlib.h>

#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtGui/QDesktopWidget>
#include <QtGui/QX11Info>

using qsnapshot::widget::QSnapshot;
using qsnapshot::widget::QSnapshotStrategy;

namespace {

	bool inject() {
		QSnapshot::Strategy::creator() = []( QSnapshot * host )->QSnapshot::Strategy * {
			return new QSnapshotStrategy( host );
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

QSnapshotStrategy::QSnapshotStrategy( QSnapshot * host ):
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

void QSnapshotStrategy::fastHide() {
	this->host->hide();
	// NOTE dirty hack to avoid those window effects provide by window managers
	if( this->compositing ) {
		this->origPos = this->host->pos();
		this->host->move( -10000, -10000 );
		delayGUI( 200 );
	}
}

void QSnapshotStrategy::fastShow() {
	this->host->move( this->origPos );
	this->host->show();
}
