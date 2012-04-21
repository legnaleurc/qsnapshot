#include "savingdialogprivate.hpp"

#include <QtGui/QDesktopServices>

using qsnapshot::widget::SavingDialog;

SavingDialog::Private::Private():
savePath( QDesktopServices::storageLocation( QDesktopServices::PicturesLocation ) ) {
}

QString SavingDialog::Private::guessDefaultName() const {
	QDir d( this->savePath );
	QString tpl( "sshot-%1.png" );
	for( int i = 1; ; ++i ) {
		if( !d.exists( tpl.arg( i ) ) ) {
			tpl = tpl.arg( i );
			break;
		}
	}
	return tpl;
}

SavingDialog::SavingDialog( QWidget * parent ):
QFileDialog( parent ),
p_( new Private ) {
	this->setAcceptMode( QFileDialog::AcceptSave );
}

void SavingDialog::setVisible( bool visible ) {
	if( visible ) {
		// pick a good default name
		QString name( this->p_->guessDefaultName() );
		this->selectFile( name );
	}
	this->QFileDialog::setVisible( visible );
}
