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

/**
 * @class qsnapshot::widget::QSnapshot::Strategy
 * @brief Strategy of each window system for qsnapshot::widget::QSnapshot.
 */

/**
 * @brief Current Strategy instance creator.
 * @return A creator functor. May be nullptr.
 * @todo return a smart pointer
 *
 * If the functor is nullptr, createInstance will use default Strategy.
 */
std::function< QSnapshot::Strategy * ( QSnapshot * ) > & QSnapshot::Strategy::creator() {
	static std::function< Strategy * ( QSnapshot * ) > f = nullptr;
	return f;
}

/**
 * @brief Creates Strategy instance.
 * @return Strategy instance
 */
QSnapshot::Strategy * QSnapshot::Strategy::createInstance( QSnapshot * host ) {
	if( Strategy::creator() == nullptr ) {
		return new Strategy( host );
	}
	return Strategy::creator()( host );
}

/**
 * @brief Constructor.
 * @param host The strategy host
 */
QSnapshot::Strategy::Strategy( QSnapshot * host ):
host( host ) {
}

/**
 * @brief Destructor.
 */
QSnapshot::Strategy::~Strategy() {
}

/**
 * @brief Hides QSnapshot immediatly.
 * @sa fastShow
 *
 * This function should disable all window effects temporary.
 */
void QSnapshot::Strategy::fastHide() {
	this->host->hide();
}

/**
 * @brief Shows QSnapshot immediatly.
 * @sa fastHide
 *
 * This function should restore window effects which disabled by fastHide.
 */
void QSnapshot::Strategy::fastShow() {
	this->host->show();
}
