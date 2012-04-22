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
		this->setDirectory( this->p_->savePath );
		// pick a good default name
		QString name( this->p_->guessDefaultName() );
		this->selectFile( name );
	} else {
		this->p_->savePath = this->directory().absolutePath();
	}
	this->QFileDialog::setVisible( visible );
}
