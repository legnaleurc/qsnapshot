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

#include <tchar.h>
#include <Windows.h>
#include <CommDlg.h>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtGui/QApplication>
#include <QtGui/QDesktopServices>

QString qsnapshot::widget::getSaveFileName( QWidget * parent ) {
	static QString initPath( QDesktopServices::storageLocation( QDesktopServices::PicturesLocation ) );

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof( ofn ) );

	ofn.lStructSize = sizeof( OPENFILENAME );
	ofn.hwndOwner = ( parent == NULL ) ? QApplication::activeWindow()->winId() : parent->winId();
	// TODO custom filters
	ofn.lpstrFilter = _T( "PNG files\0*.png\0\0" );

	TCHAR fileName[MAX_PATH];
	_tcsncpy_s( fileName, QDir::toNativeSeparators( guessDefaultFileName( initPath ) ).toStdWString().c_str(), MAX_PATH );
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;

	TCHAR initDir[MAX_PATH];
	_tcsncpy_s( initDir, initPath.toStdWString().c_str(), MAX_PATH );
	ofn.lpstrInitialDir = initDir;

	// TODO custom title
	ofn.lpstrTitle = NULL;

	ofn.Flags = ( OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_PATHMUSTEXIST );

	BOOL ok = GetSaveFileName( &ofn );
	if( !ok ) {
		return QString();
	}

	QFileInfo info( QDir::fromNativeSeparators( QString::fromWCharArray( fileName ) ) );
	initPath = info.absolutePath();

#ifdef _UNICODE
	QString tmp( QString::fromWCharArray( fileName ) );
#else
	QString tmp( QString::fromLocal8Bit( fileName ) );
#endif
	return tmp;
}
