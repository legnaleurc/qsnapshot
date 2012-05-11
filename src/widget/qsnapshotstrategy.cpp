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

QSnapshot::Strategy::~Strategy() {
}

void QSnapshot::Strategy::fastHide() {
	this->host->hide();
}

void QSnapshot::Strategy::fastShow() {
	this->host->show();
}
