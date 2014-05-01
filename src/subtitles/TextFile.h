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

#pragma once

#include <afx.h>
#include <vector>

class CTextFile {
	std::vector<wchar_t> outbuf;
	const char *data = nullptr;
	size_t size = 0;
	size_t pos = 0;

public:
	enum enc {ASCII, UTF8, LE16, BE16};

	CTextFile(enc e = ASCII);

	void OpenMem(const char *data, size_t len);

	bool Open(LPCTSTR lpszFileName);
	bool Save(LPCTSTR lpszFileName, enc e /*= ASCII*/) { return false; }

	void SetEncoding(enc) { }
	enc GetEncoding() { return UTF8; }
	bool IsUnicode() { return true; }

	CString GetFilePath() const { return L""; }
	ULONGLONG GetPosition() const { return pos; }

	void WriteString(LPCWSTR lpsz/*CStringW str*/) { }
	BOOL ReadString(CStringW& str);
};

using CWebTextFile = CTextFile;
