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
#ifndef WINDOWSGRABBERPRIVATE_HPP
#define WINDOWSGRABBERPRIVATE_HPP

#include "windowgrabber.hpp"

#include <QtCore/QRect>

#include <vector>

namespace qsnapshot {
	namespace widget {

		class WindowGrabber::Private : public QObject {
		public:
			Private( WindowGrabber * host );

			int windowIndex( const QPoint & pos ) const;
			void increaseScope( const QPoint & pos );
			void decreaseScope( const QPoint & pos );
			void finishGrab();

			WindowGrabber * host;
			std::vector< QRect > windows;
			int current;
			int yPos;
			QPoint windowPosition;
		};

	}
}

#endif // WINDOWSGRABBERPRIVATE_HPP
