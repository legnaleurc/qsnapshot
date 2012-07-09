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

#include <Windows.h>
#include <WinUser.h>

# include <QtCore/QLibrary>

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

}

QSnapshotStrategy::QSnapshotStrategy( QSnapshot * host ):
QSnapshot::Strategy( host ),
// load dll at run-time because Windows XP does not support DWM
f( reinterpret_cast< Function >( QLibrary::resolve( "dwmapi", "DwmSetWindowAttribute" ) ) ) {
	RegisterHotKey( host->winId(), 0, MOD_NOREPEAT, VK_SNAPSHOT );
}

QSnapshotStrategy::~QSnapshotStrategy() {
	UnregisterHotKey( this->host->winId(), 0 );
}

void QSnapshotStrategy::fastHide() {
	this->disableDWM( true );
	this->host->hide();
}

void QSnapshotStrategy::fastShow() {
	this->disableDWM( false );
	this->host->show();
}

void QSnapshotStrategy::disableDWM( bool disable ) {
	if( this->f == nullptr ) {
		return;
	}
	DWORD attr = disable ? TRUE : FALSE;
	HRESULT hr = f( this->host->winId(), DWMWA_TRANSITIONS_FORCEDISABLED, &attr, sizeof( attr ) );
	// TODO throw warning if failed
	SUCCEEDED( hr );
}
