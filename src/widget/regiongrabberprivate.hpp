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
#ifndef REGIONGRABBERPRIVATE_HPP
#define REGIONGRABBERPRIVATE_HPP

#include "regiongrabber.hpp"

#include <QtGui/QPixmap>

namespace qsnapshot {
	namespace widget {

		class RegionGrabber::Private : public QObject {
			Q_OBJECT
		public:
			enum MaskType {
				StrokeMask,
				FillMask
			};

			explicit Private( RegionGrabber * host );

			void grabRect();
			QPoint limitPointToRect( const QPoint & p , const QRect & r ) const;
			QRect normalizeSelection( const QRect & r ) const;
			QRegion handleMask( MaskType type ) const;
			void updateHandles();
			void finishGrab();

		signals:
			void regionGrabbed( const QPixmap & pixmap );

		public:
			RegionGrabber * host;
			QPixmap pixmap;
			QRect selection;
			QRect selectionBeforeDrag;
			QRect helpTextRect;
			QPoint dragStartPoint;
			QRect * mouseOverHandle;
			bool grabbing;
			bool showHelp;
			bool mouseDown;
			bool newSelection;
			// naming convention for handles
			// T top, B bottom, R Right, L left
			// 2 letters: a corner
			// 1 letter: the handle on the middle of the corresponding side
			QRect TLHandle, TRHandle, BLHandle, BRHandle;
			QRect LHandle, THandle, RHandle, BHandle;
			QList< QRect * > handles;
		};

	}
}

#endif
