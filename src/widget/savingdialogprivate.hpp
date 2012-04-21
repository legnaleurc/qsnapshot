#ifndef QSNAPSHOT_WIDGET_SAVINGDIALOGPRIVATE_HPP
#define QSNAPSHOT_WIDGET_SAVINGDIALOGPRIVATE_HPP

#include "savingdialog.hpp"

namespace qsnapshot {
	namespace widget {

		class SavingDialog::Private {
		public:
			Private();

			QString guessDefaultName() const;

			QString savePath;
		};

	}
}

#endif
