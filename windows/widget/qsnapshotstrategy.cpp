#include "qsnapshotstrategy.hpp"

# include <QtCore/QLibrary>

using qsnapshot::widget::QSnapshot;

const bool QSnapshot::Strategy::HACK = inject();

bool QSnapshot::Strategy::inject() {
	QSnapshot::Private::creator() = []( QSnapshot * host )->QSnapshot::Private * {
		return new QSnapshot::Strategy( host );
	};
	return true;
}

QSnapshot::Strategy::Strategy( QSnapshot * host ):
QSnapshot::Private( host ),
// load dll at run-time because Windows XP does not support DWM
f( reinterpret_cast< Function >( QLibrary::resolve( "dwmapi", "DwmSetWindowAttribute" ) ) ) {
}

void QSnapshot::Strategy::fastHide() {
	this->disableDWM( true );
	this->host->hide();
}

void QSnapshot::Strategy::fastShow() {
	this->disableDWM( false );
	this->host->show();
}

void QSnapshot::Strategy::disableDWM( bool disable ) {
	if( this->f == nullptr ) {
		return;
	}
	DWORD attr = disable ? TRUE : FALSE;
	HRESULT hr = f( this->host->winId(), DWMWA_TRANSITIONS_FORCEDISABLED, &attr, sizeof( attr ) );
	// TODO throw warning if failed
	SUCCEEDED( hr );
}
