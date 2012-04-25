#ifndef QSNAPSHOT_QSNAPSHOTSTRATEGY_HPP
#define QSNAPSHOT_QSNAPSHOTSTRATEGY_HPP

#include "../../src/widget/qsnapshotprivate.hpp"

#include <dwmapi.h>

namespace qsnapshot {
	namespace widget {

		class QSnapshot::Strategy : public QSnapshot::Private {
		public:
			typedef HRESULT ( WINAPI * Function )( HWND, DWORD, LPCVOID, DWORD );

			static const bool HACK;
			static bool inject();

			explicit Strategy( QSnapshot * host );

			virtual void fastHide();
			virtual void fastShow();

			void disableDWM( bool disable );

			Function f;
		};

	}
}

#endif
