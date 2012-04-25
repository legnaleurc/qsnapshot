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
#include "../../src/widget/savingdialog.hpp"

#include <QtGui/QDesktopServices>
#include <QtGui/QFileDialog>

#include <memory>

QString qsnapshot::widget::getSaveFileName( QWidget * parent ) {
	static QString initPath( QDesktopServices::storageLocation( QDesktopServices::PicturesLocation ) );

	std::unique_ptr< QFileDialog > dialog( new QFileDialog( parent ) );

	dialog->setAcceptMode( QFileDialog::AcceptSave );
	dialog->setDirectory( initPath );
	// pick a good default name
	QString name( guessDefaultFileName( initPath ) );
	dialog->selectFile( name );
	// TODO add file format support
	dialog->setDefaultSuffix( "png" );
	dialog->setNameFilter( "PNG files (*.png)" );

	if( QDialog::Accepted != dialog->exec() ) {
		return QString();
	}

	initPath = dialog->directory().absolutePath();

	QStringList filePaths( dialog->selectedFiles() );
	if( filePaths.empty() ) {
		return QString();
	}
	QFileInfo fileInfo( filePaths.at( 0 ) );
	if( fileInfo.exists() && !fileInfo.isFile() ) {
		return QString();
	}
	return fileInfo.absoluteFilePath();
}
