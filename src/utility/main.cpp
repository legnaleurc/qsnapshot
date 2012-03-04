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
#include "widget/qsnapshot.hpp"
#include "qsnapshot_config.h"

#include <QtCore/QTextCodec>
#include <QtGui/QApplication>
#include <QtGui/QIcon>

int main( int argc, char * argv[] ) {
	QApplication a( argc, argv );

	QTextCodec::setCodecForTr( QTextCodec::codecForName( "UTF-8" ) );
	QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "UTF-8" ) );
	QApplication::setApplicationName( "QSnapshot" );
	QApplication::setApplicationVersion( QSNAPSHOT_STRINGIZER( QSNAPSHOT_VERSION ) );
	QApplication::setWindowIcon( QIcon( ":/share/pixmap/logo.png" ) );

	qsnapshot::widget::QSnapshot w;
	w.show();

	return a.exec();
}
