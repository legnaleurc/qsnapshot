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
#ifndef MAINWINDOWPRIVATE_HPP
#define MAINWINDOWPRIVATE_HPP

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "snapshottimer.hpp"
#include "regiongrabber.hpp"
#include "windowgrabber.hpp"

#include <memory>

namespace qsnapshot {
	namespace widget {

		class MainWindow::Private : public QObject {
			Q_OBJECT
		public:
			explicit Private( MainWindow * host );

			void grabRegion();
			void performGrab();
			void updatePreview();
			void setPreview( const QPixmap & pixmap );

			int delay() const;
			int mode() const;
			bool includeDecorations() const;

		public slots:
			void grab();
			void startUndelayedGrab();
			void onRegionGrabbed( const QPixmap & p );
			void onWindowGrabbed( const QPixmap & p );
			void onGrabTimerTimeout();

		public:
			MainWindow * host;
			Ui::MainWindow ui;
			std::shared_ptr< QWidget > grabber;
			SnapshotTimer * grabTimer;
			RegionGrabber * regionGrabber;
			std::shared_ptr< WindowGrabber > windowGrabber;
			QPixmap snapshot;
			QPixmap pixmap;
			QPoint savedPosition;
			bool modified;
		};

	}
}

#endif
