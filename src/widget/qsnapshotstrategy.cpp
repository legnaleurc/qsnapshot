#include "qsnapshotstrategy.hpp"

using qsnapshot::widget::QSnapshot;

std::function< QSnapshot::Strategy * ( QSnapshot * ) > & QSnapshot::Strategy::creator() {
	static std::function< Strategy * ( QSnapshot * ) > f = nullptr;
	return f;
}

QSnapshot::Strategy * QSnapshot::Strategy::createInstance( QSnapshot * host ) {
	if( Strategy::creator() == nullptr ) {
		return new Strategy( host );
	}
	return Strategy::creator()( host );
}

QSnapshot::Strategy::Strategy( QSnapshot * host ):
host( host ) {
}

void QSnapshot::Strategy::fastHide() {
	this->host->hide();
}

void QSnapshot::Strategy::fastShow() {
	this->host->show();
}

/*
void QSnapshot::Strategy::postNew() {
	// except X11, somehow opacity == 0.0 will block events
	this->grabber->setWindowOpacity( 0.1 );
}
*/
