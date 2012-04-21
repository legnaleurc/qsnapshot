#ifndef QSNAPSHOT_WIDGET_SAVINGDIALOG_HPP
#define QSNAPSHOT_WIDGET_SAVINGDIALOG_HPP

#include <QtGui/QFileDialog>

#include <memory>

namespace qsnapshot {
	namespace widget {

		class SavingDialog : public QFileDialog {
		public:
			explicit SavingDialog( QWidget * parent );

			virtual void setVisible( bool visible );

		private:
			class Private;
			std::unique_ptr< Private > p_;
		};

	}
}

#endif
