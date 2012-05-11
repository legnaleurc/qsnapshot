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
#include "focusgrabberstrategy.hpp"

#include <X11/Xlib.h>

#include <QtGui/QDesktopWidget>

using qsnapshot::widget::FocusGrabber;
using qsnapshot::widget::FocusGrabberStrategy;

namespace {

	bool inject() {
		FocusGrabber::Strategy::creator() = []( FocusGrabber * host )->FocusGrabber::Strategy * {
			return new FocusGrabberStrategy( host );
		};
		return true;
	}

	const bool HACK = inject();

}

FocusGrabberStrategy::FocusGrabberStrategy( FocusGrabber * host ):
FocusGrabber::Strategy( host ) {
}

void FocusGrabberStrategy::postNew() {
	// to avoid grabber remain on screen
	this->host->setWindowOpacity( 0.0 );
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
