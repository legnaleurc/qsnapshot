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
#ifndef QSNAPSHOT_WIDGET_WINDOWGRABBER_HPP
#define QSNAPSHOT_WIDGET_WINDOWGRABBER_HPP

#include "abstractgrabber.hpp"

#include <QtGui/QPixmap>
#include <QtCore/QPoint>

#include <tuple>

namespace qsnapshot {
	namespace widget {

		class WindowGrabber : public AbstractGrabber {
			Q_OBJECT
		public:
			static std::tuple< QPixmap, QPoint > grabCurrent( bool includeDecorations );

			WindowGrabber( QWidget * parent );

			void grab();

		protected:
			virtual void mousePressEvent( QMouseEvent * event );
			virtual void mouseReleaseEvent( QMouseEvent * event );
			virtual void mouseMoveEvent( QMouseEvent * event );
			virtual void wheelEvent( QWheelEvent * event );
			virtual void paintEvent( QPaintEvent * event );

		signals:
			void windowGrabbed( const QPixmap & pixmap );

		private:
			class Private;
			Private * p_;
		};

	}
}

#endif
