/*
    FocusGrabber, a screen capture tool.
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

using qsnapshot::widget::FocusGrabber;

std::function< FocusGrabber::Strategy * ( FocusGrabber * ) > & FocusGrabber::Strategy::creator() {
	static std::function< Strategy * ( FocusGrabber * ) > f = nullptr;
	return f;
}

FocusGrabber::Strategy * FocusGrabber::Strategy::createInstance( FocusGrabber * host ) {
	if( Strategy::creator() == nullptr ) {
		return new Strategy( host );
	}
	return Strategy::creator()( host );
}

FocusGrabber::Strategy::Strategy( FocusGrabber * host ):
host( host ) {
}

FocusGrabber::Strategy::~Strategy() {
}

void FocusGrabber::Strategy::postNew() {
	// except X11, somehow opacity == 0.0 will block events
	this->host->setWindowOpacity( 0.1 );
}
