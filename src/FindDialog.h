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

#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QTextDocument>
#include <QUrl>

class QCheckBox;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QVBoxLayout;

class FindDialog : public QDialog
{

    Q_OBJECT

public:
    FindDialog(QWidget *parent=0);

signals:
    void find(QString, QTextDocument::FindFlags);

private slots:
    void findNextButtonClicked();
    void findPreviousButtonClicked();
    void enableFindButtons(QString);

private:
    void createWidgets();
    void setupLayouts();

    QPushButton *findNextButton;
    QPushButton *findPreviousButton;
    QPushButton *closeButton;
    QLineEdit *findEdit;
    QLabel *whatLabel;
    QCheckBox *sensitivityBox;
    QCheckBox *findWordsBox;
    QHBoxLayout *topLeftLayout;
    QHBoxLayout *mainLayout;
    QVBoxLayout *rightLayout;
    QVBoxLayout *leftLayout;

};

#endif // FINDDIALOG_H
