/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2023 Ricardo Villalba <ricardo@smplayer.info>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include <QStringList>

class ExtensionList : public QStringList
{
public:
    using QStringList::QStringList;

    QString forFilter() const;
    QStringList forDirFilter() const;
    QString forRegExp() const;
};

class ExtensionsClass
{
public:
    ExtensionsClass(const ExtensionsClass& root) = delete;
    ExtensionsClass& operator=(const ExtensionsClass&) = delete;

    static ExtensionsClass& i()
    {
        static ExtensionsClass instance;
        return instance;
    }

    inline const ExtensionList video() const { return _video; };
    inline const ExtensionList audio() const { return _audio; };
    inline const ExtensionList playlist() const { return _playlist; };
    inline const ExtensionList subtitles() const { return _subtitles; };
    inline const ExtensionList multimedia() const { return _multimedia; };
    inline const ExtensionList allPlayable()const { return _all_playable; };

protected:
    ExtensionsClass();
	ExtensionList _video, _audio, _playlist, _subtitles;
	ExtensionList _multimedia; //!< video and audio
	ExtensionList _all_playable; //!< video, audio and playlist
};

#define Extensions ExtensionsClass::i()

#endif
