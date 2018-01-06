// Copyright 2018 Patrick Flynn
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//	this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this
//	list of conditions and the following disclaimer in the documentation and/or
//	other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may
//	be used to endorse or promote products derived from this software
//	without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#pragma once

#include <QMainWindow>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QMenuBar>
#include <QContextMenuEvent>

#include "navbar.hh"
#include "addressbar_text.hh"
#include "tabwidget.hh"

#include "menubar/filemenu.hh"
#include "menubar/editmenu.hh"
#include "menubar/viewmenu.hh"
#include "menubar/helpmenu.hh"

class FileMenu;
class MenuBar;

class Window : public QMainWindow {
    Q_OBJECT
public:
    Window(QWidget *parent = 0);
    ~Window();
    void closeApp();
protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    NavBar *navbar;
    MenuBar *menubar;
    FileMenu *filemenu;
    EditMenu *editmenu;
    ViewMenu *viewmenu;
    HelpMenu *helpmenu;
    AddressBarText *addrTxt;
    TabWidget *tabPane;
};

class MenuBar : public QMenuBar {
    Q_OBJECT
protected:
    void contextMenuEvent(QContextMenuEvent *);
};
