#ifndef QSNAPSHOT_WIDGET_X11STRATEGY_HPP
#define QSNAPSHOT_WIDGET_X11STRATEGY_HPP

#include "widget/qsnapshotstrategy.hpp"

namespace qsnapshot {
	namespace widget {

		class X11Strategy : public QSnapshot::Strategy {
		public:
			explicit X11Strategy( QSnapshot * host );

			virtual void fastHide();
			virtual void fastShow();
//			virtual void postNew();

			bool compositing;
			QPoint origPos;
		};

	}
}

#endif
