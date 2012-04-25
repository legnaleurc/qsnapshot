#ifndef QSNAPSHOT_WIDGET_QSNAPSHOTSTRATEGY_HPP
#define QSNAPSHOT_WIDGET_QSNAPSHOTSTRATEGY_HPP

#include "../../src/widget/qsnapshotprivate.hpp"

namespace qsnapshot {
	namespace widget {

		class QSnapshot::Strategy : public QSnapshot::Private {
		public:
			explicit Strategy( QSnapshot * host );

			virtual void fastHide();
			virtual void fastShow();

			bool compositing;
		};

	}
}

#endif
