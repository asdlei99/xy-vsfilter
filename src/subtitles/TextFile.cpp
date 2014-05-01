/* 
 *	Copyright (C) 2003-2006 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "stdafx.h"

#include <atlbase.h>
#include <afxinet.h>

#include "TextFile.h"

#include <algorithm>

CTextFile::CTextFile(enc) { }

bool CTextFile::Open(LPCTSTR lpszFileName) {
	return false;
}

void CTextFile::OpenMem(const char *data, size_t len) {
	this->data = data;
	this->size = len;
}

BOOL CTextFile::ReadString(CStringW& str) {
	if (pos >= size) return false;

	auto begin = data + pos;
	auto end = std::find(begin, data + size, '\n');
	auto next = end - data;
	if (next < size) ++next;

	if (end > data + 1 && end[-1] == '\r') --end;

	if (outbuf.size() < (end - begin) * 2)
		outbuf.resize((end - begin) * 4);

	auto written = MultiByteToWideChar(CP_UTF8, 0, begin, end - begin, &outbuf[0], outbuf.size());
	// Pre-trim since CString::Trim is slow
	while (written > 0 && CStringW::StrTraits::IsSpace(outbuf[written - 1]))
		--written;

	str.Empty();
	str.Append(&outbuf[0], written);
	pos = next;
	return true;
}
