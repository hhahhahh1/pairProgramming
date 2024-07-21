/*************************************************************************    
 *
 *   Ned - Simple, graphical, cross-platform editor
 *   Copyright (C) 2010 Ákos Kovács <akoskovacs@gmx.com> 
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **************************************************************************/    

#include "MainWindow.h"

#include <QtGui/QApplication>
#include <QStringList>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Removes the first argument
    QStringList arguments = app.arguments();
    arguments.removeFirst();

    QTranslator appTranslator;
    appTranslator.load("ned_" + QLocale::system().name(), ":/translations");
    app.installTranslator(&appTranslator);

    // Create the window(s)
    MainWindow *window = new MainWindow(arguments);
    window->show();
    return app.exec();
}
