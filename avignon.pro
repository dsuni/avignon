TEMPLATE = app
TARGET = avignon
DEPENDPATH += .
INCLUDEPATH += .
QT += core gui svg

# Input
HEADERS += deck.h subtile.h table.h terrain.h tile.h mainwindow.h tileimage.h gamecreation.h tablelabel.h player.h follower.h followertype.h followerplacer.h imagelayer.h occupationmapper.h const.h piece.h dragonmover.h
SOURCES += deck.cpp main.cpp subtile.cpp table.cpp tile.cpp mainwindow.cpp tileimage.cpp gamecreation.cpp tablelabel.cpp player.cpp follower.cpp followerplacer.cpp occupationmapper.cpp piece.cpp dragonmover.cpp
