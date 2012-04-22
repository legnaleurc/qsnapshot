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
#include "ui_qsnapshot.h"
#include "savingdialog.hpp"
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
			void performGrab();
			void updatePreview();
			void setPreview( const QPixmap & pixmap );
			void setFastHide( bool fastHide );

			int delay() const;
			int mode() const;
			bool includeDecorations() const;

		public slots:
			void onSaveAs();
			void onCopy();
			void grab();
			void onRegionGrabbed( const QPixmap & p );
			void onWindowGrabbed( const QPixmap & p );
			void startGrab();

		public:
			QSnapshot * host;
			Ui::QSnapshot ui;
			std::shared_ptr< QWidget > grabber;
			SnapshotTimer * grabTimer;
			RegionGrabber * regionGrabber;
			SavingDialog * savingDialog;
			std::shared_ptr< WindowGrabber > windowGrabber;
			QPixmap snapshot;
			QPoint savedPosition;
			bool modified;
		};

	}
}

#endif
