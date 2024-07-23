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

#include "FindDialog.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
{
    createWidgets();
    setupLayouts();

    findEdit->setFocus();
    findNextButton->setEnabled(false);
    findNextButton->setDefault(true);
    findPreviousButton->setEnabled(false);
    whatLabel->setBuddy(findEdit);
    findWordsBox->setChecked(true);

    connect(findEdit, SIGNAL(textChanged(QString)),
            this, SLOT(enableFindButtons(QString)));

    connect(findNextButton, SIGNAL(clicked()),
            this, SLOT(findNextButtonClicked()));

    connect(findPreviousButton, SIGNAL(clicked()),
            this, SLOT(findPreviousButtonClicked()));

    connect(closeButton, SIGNAL(clicked()),
            this, SLOT(close()));

    setWindowTitle(tr("Find"));
    setFixedHeight(sizeHint().height());
}

void FindDialog::createWidgets()
{
    findNextButton = new QPushButton(tr("Find &next >>"));
    findPreviousButton = new QPushButton(tr("Find &previous <<"));
    closeButton = new QPushButton(tr("&Close"));
    findEdit = new QLineEdit;
    whatLabel = new QLabel(tr("Find &what"));
    sensitivityBox = new QCheckBox(tr("Case &sesitive"));
    findWordsBox = new QCheckBox(tr("Find w&hole words"));
    topLeftLayout = new QHBoxLayout;
    mainLayout = new QHBoxLayout;
    rightLayout = new QVBoxLayout;
    leftLayout = new QVBoxLayout;
}

void FindDialog::setupLayouts()
{
    topLeftLayout->addWidget(whatLabel);
    topLeftLayout->addWidget(findEdit);

    leftLayout->addLayout(topLeftLayout);
    leftLayout->addStretch();
    leftLayout->addWidget(sensitivityBox);
    leftLayout->addWidget(findWordsBox);

    rightLayout->addWidget(findNextButton);
    rightLayout->addWidget(findPreviousButton);
    rightLayout->addStretch();
    rightLayout->addWidget(closeButton);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);
}

void FindDialog::findNextButtonClicked()
{
    QString searchString = findEdit->text();

    QTextDocument::FindFlags ff;
       if (sensitivityBox->isChecked())
            ff |= QTextDocument::FindCaseSensitively;

       if (findWordsBox->isChecked())
           ff |= QTextDocument::FindWholeWords;

    emit find(searchString, ff);
}

void FindDialog::findPreviousButtonClicked()
{
    QString searchString = findEdit->text();

    QTextDocument::FindFlags ff;
       if (sensitivityBox->isChecked())
            ff |= QTextDocument::FindCaseSensitively;

       if (findWordsBox->isChecked())
           ff |= QTextDocument::FindWholeWords;

       ff |= QTextDocument::FindBackward;

    emit find(searchString, ff);
}
void FindDialog::enableFindButtons(QString string)
{
    findNextButton->setEnabled(!string.isEmpty());
    findPreviousButton->setEnabled(!string.isEmpty());
}
