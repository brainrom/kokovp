/*  This file was part of smplayer and now part of KokoVP
    smplayer, GUI front-end for mplayer.
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

#include "filehash.h"
#include <QFile>
#include <QDataStream>
#include <QCryptographicHash>

// From the patch by Kamil Dziobek turbos11(at)gmail.com
// (c) Kamil Dziobek turbos11(at)gmail.com | BSD or GPL or public domain
QString FileHash::calculateHash(QString filename) {
	QFile file(filename);

	if (!file.exists()) {
		qWarning("OSParser:calculateHash: error hashing file. File doesn't exist.");
		return QString();
	}

	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);
	in.setByteOrder(QDataStream::LittleEndian);
	quint64 size=file.size ();
	quint64 hash=size; 
	quint64 a;
	for(int i = 0; i < 8192; i++) {
		in >> a ; hash += a;
	};
	file.seek(size-65536);
	for(int i = 0; i < 8192; i++) {
		in >> a ; hash += a;
	};

	QString hexhash = QString("%1").arg(hash, 16, 16, QChar('0'));

	return hexhash;
}

QString FileHash::calculateHashSHA256(QIODevice *device, const QByteArray &salt)
{
    QCryptographicHash hash(QCryptographicHash::Sha256);

    hash.addData(salt);

    qint64 size = device->size();
    QByteArray sizeBytes;
    QDataStream ds(&sizeBytes, QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::LittleEndian);
    ds << size;
    hash.addData(sizeBytes);

    device->seek(0);
    hash.addData(device->read(8192));

    if (size > 8192) {
        device->seek(qMax<qint64>(0, size - 8192));
        hash.addData(device->read(8192));
    }

    return hash.result().toHex(); // 64 hex chars
}
