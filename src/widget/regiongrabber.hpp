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
#ifndef QSNAPSHOT_WIDGET_REGIONGRABBER_HPP
#define QSNAPSHOT_WIDGET_REGIONGRABBER_HPP

#include "abstractgrabber.hpp"

namespace qsnapshot {
	namespace widget {

		class RegionGrabber : public AbstractGrabber {
			Q_OBJECT
		public:
			explicit RegionGrabber( QWidget * widget );

			void grab();

		protected:
			virtual void paintEvent( QPaintEvent * event );
			virtual void resizeEvent( QResizeEvent * event );
			virtual void mousePressEvent( QMouseEvent * event );
			virtual void mouseMoveEvent( QMouseEvent * event );
			virtual void mouseReleaseEvent( QMouseEvent * event );
			virtual void mouseDoubleClickEvent( QMouseEvent * event );
			virtual void keyPressEvent( QKeyEvent * event );

		signals:
			void regionGrabbed( const QPixmap & pixmap );

		private:
			class Private;
			Private * p_;
		};

	}
}

#endif
