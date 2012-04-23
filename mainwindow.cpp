/*
  mainwindow.cpp

  Copyright (c) 2012, Jeremiah LaRocco jeremiah.larocco@gmail.com

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <QtGui>
#include <QSettings>

#include <cstdlib>

#include "mainwindow.h"

#include "stlviewer.h"

/*!
  Performs initialization
*/
MainWindow::MainWindow() : QMainWindow() {
  
    // Create STLViewer widget
    stl = new STLViewer(this);
  
    promptExit = true;

    qset = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                         "STLViewer", "STLViewer");

    readSettings();
  
    // Make the STLViewer the central widget 
    setCentralWidget(stl);

    // Initialize GUI stuff
    setWindowTitle(tr("STLViewer"));
    setWindowIcon(QIcon(":/images/icon.png"));
    createActions();
    createMenus();
    createToolbar();
    createStatusBar();
}

MainWindow::~MainWindow() {
    // Delete everything
    delete openFileAction;
    delete aboutAction;
    delete aboutQtAction;
    delete quitAction;
    delete resetViewAction;

    delete theToolbar;
  
    delete fileMenu;
    delete optionsMenu;
    delete helpMenu;
    delete statusLabel;
    delete tbIcon;

    delete stl;
}

/*!
  Initializes all of the QActions used by the game
*/
void MainWindow::createActions() {
    // New game
    openFileAction = new QAction(tr("&Open..."), this);
    openFileAction->setIcon(QIcon(":/images/open.png"));
    openFileAction->setShortcut(tr("Ctrl+O"));
    openFileAction->setStatusTip(tr("Open an STL file"));
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(openFile()));

    // About
    aboutAction = new QAction(tr("About"), this);
    aboutAction->setIcon(QIcon(":/images/about.png"));
    aboutAction->setShortcut(tr("Ctrl+A"));
    aboutAction->setStatusTip(tr("About STLViewer"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    // About QT
    aboutQtAction = new QAction(tr("About Qt"), this);
    aboutQtAction->setIcon(QIcon(":/images/about.png"));
    aboutQtAction->setStatusTip(tr("About Qt"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // Exit
    quitAction = new QAction(tr("Exit"), this);
    quitAction->setIcon(QIcon(":/images/quit.png"));
    quitAction->setShortcut(tr("Ctrl+Q"));
    quitAction->setStatusTip(tr("Quit"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));//quit()));

    // Reset view
    resetViewAction = new QAction(tr("Reset View"), this);
    resetViewAction->setIcon(QIcon(":/images/view.png"));
    resetViewAction->setShortcut(tr("Ctrl+V"));
    resetViewAction->setStatusTip(tr("Reset the view"));
    connect(resetViewAction, SIGNAL(triggered()), this, SLOT(resetView()));
}

/*!
  Initialize menus
*/
void MainWindow::createMenus() {
    // Game menu
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openFileAction);
    fileMenu->addSeparator();
    fileMenu->addAction(openFileAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    // Options menu
    optionsMenu = menuBar()->addMenu(tr("&Options"));

    // Help menu
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutQtAction);
}

/*!
  Initialize toolbar
*/
void MainWindow::createToolbar() {
    theToolbar = addToolBar(tr("File"));
    theToolbar->addAction(openFileAction);
    theToolbar->addAction(resetViewAction);
}

/*!
  Initialize status bar
*/
void MainWindow::createStatusBar() {
    // Make the label as large as it will ever be
    statusLabel = new QLabel("No file loaded");
    statusLabel->setAlignment(Qt::AlignHCenter);
    // Then set its minimum size and add to the status bar
    statusLabel->setMinimumSize(statusLabel->sizeHint());
    statusBar()->addWidget(statusLabel);
}

/*!
  Updates the status bar to display number of bombs left.
*/
void MainWindow::updateStatusBar(QString fileName) {
    statusLabel->setText(fileName);
}

/*!
  Creates a new game in response to the openFile action being triggered
*/
void MainWindow::openFile() {
    QString fileName =
        QFileDialog::getOpenFileName(this,
                                     tr("Choose an input file..."),
                                     tr("."),
                                     tr("STL Stereolithography File (*.stl);;All Files (*)"));
    if (fileName == tr("")) {
        return;
    }
    if (stl->openFile(fileName)) {
        updateStatusBar(fileName);
    }
}

/*!
  Display the about box
*/
void MainWindow::about() {
    QMessageBox::about(this, tr("About STLViewer"),
                       tr("<h2>STLViewer 0.1</h2>"
                          "<p>Copyright &copy; 2012 Jeremiah LaRocco</p>"
                          "<p>An STL file viewer</p>"));
}

/*!
  Prompt before closing
*/
void MainWindow::closeEvent(QCloseEvent *event) {
    event->accept();
}

/*!
  Reset the view
*/
void MainWindow::resetView() {
    if (stl) {
        stl->resetView();
    }
}

void MainWindow::readSettings() {
    qset->sync();
    // For future reference:
    // qset->value("whatever", default_int_value).toInt();
    // qset->value("whatever", default_string_value).toString();
    // qset->value("whatever", QDateTime(2012, 4,2)).toDateTime();
}
