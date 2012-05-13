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
#ifndef QSNAPSHOT_WIDGET_QSNAPSHOT_PRIVATE_HPP
#define QSNAPSHOT_WIDGET_QSNAPSHOT_PRIVATE_HPP

#include "qsnapshot.hpp"
#include "qsnapshotstrategy.hpp"
#include "ui_qsnapshot.h"
#include "focusgrabber.hpp"
#include "snapshottimer.hpp"
#include "regiongrabber.hpp"
#include "windowgrabber.hpp"

#include <memory>

namespace qsnapshot {
	namespace widget {

		class QSnapshot::Private : public QObject {
			Q_OBJECT
		public:
			explicit Private( QSnapshot * host );

			void grabRegion();
			void updatePreview();
			void setPreview( const QPixmap & pixmap );

			int delay() const;
			int mode() const;
			bool includeDecorations() const;

		public slots:
			void onSaveAs();
			void onCopy();
			void onHelp();
			void grab();
			void onRegionGrabbed( const QPixmap & p );
			void onWindowGrabbed( const QPixmap & p );
			void performGrab();
			void startGrab();

		public:
			QSnapshot * host;
			Ui::QSnapshot ui;
			std::unique_ptr< QSnapshot::Strategy > strategy;
			std::unique_ptr< FocusGrabber > grabber;
			SnapshotTimer * grabTimer;
			std::unique_ptr< RegionGrabber > regionGrabber;
			std::shared_ptr< WindowGrabber > windowGrabber;
			QPixmap snapshot;
			QPoint savedPosition;
			bool modified;
		};

	}
}

#endif
