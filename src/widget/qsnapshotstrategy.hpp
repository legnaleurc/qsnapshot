#ifndef QSNAPSHOT_WIDGET_QSNAPSHOT_STRATEGY_HPP
#define QSNAPSHOT_WIDGET_QSNAPSHOT_STRATEGY_HPP

#include "qsnapshot.hpp"

#include <functional>

namespace qsnapshot {
	namespace widget {

		class QSnapshot::Strategy {
		public:
			static std::function< Strategy * ( QSnapshot * ) > & creator();
			static Strategy * createInstance( QSnapshot * host );

			virtual ~Strategy();

			virtual void fastHide();
			virtual void fastShow();
//			virtual void postNew();

		protected:
			explicit Strategy( QSnapshot * host );
			QSnapshot * host;

		private:
			Strategy( const Strategy & );
			Strategy & operator =( const Strategy & );
		};

	}
}

#endif
