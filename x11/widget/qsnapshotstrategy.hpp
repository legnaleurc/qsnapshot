#ifndef QSNAPSHOT_WIDGET_QSNAPSHOTSTRATEGY_HPP
#define QSNAPSHOT_WIDGET_QSNAPSHOTSTRATEGY_HPP

#include "../../src/widget/qsnapshotprivate.hpp"

namespace qsnapshot {
	namespace widget {

		class QSnapshot::Strategy : public QSnapshot::Private {
		public:
			static bool inject();
			static const bool HACK;

			explicit Strategy( QSnapshot * host );

			virtual void fastHide();
			virtual void fastShow();
			virtual void postNew();

			bool compositing;
			QPoint origPos;
		};

	}
}

#endif
