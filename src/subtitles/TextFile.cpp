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
#include "Utf8.h"

CTextFile::CTextFile(enc e)
: m_bufferPos(0)
, m_bufferCount(0)
{
}

bool CTextFile::Open(LPCTSTR lpszFileName) {
	m_bufferCount = 0;
	return __super::Open(lpszFileName, modeRead | typeBinary | shareDenyNone);
}

bool CTextFile::Save(LPCTSTR lpszFileName, enc e) {
	return(false);
}

// CFile

CString CTextFile::GetFilePath() const {
	// to avoid a CException coming from CTime
	return m_strFileName; // __super::GetFilePath();
}

// CStdioFile

ULONGLONG CTextFile::GetPosition() const {
	return CStdioFile::GetPosition() - (m_bufferCount ? m_bufferCount - m_bufferPos : 0);
}

void CTextFile::WriteString(LPCWSTR lpsz/*CStringW str*/) { }

bool CTextFile::ReadLine() {
	m_convertedBuffer.clear();
	ULONGLONG startPos = GetPosition();

	while (true) {
		if (m_bufferPos >= m_bufferCount) {
			m_bufferCount = Read(m_readbuffer, sizeof(m_readbuffer));
			if (!m_bufferCount) return true;
			m_bufferPos = 0;
		}

		int c = NextChar();
		if (c == '\r') continue;
		if (c == '\n') return false;
		if (c >= 0 && c < 0x10000) {
			m_convertedBuffer.push_back((wchar_t)c);
			continue;
		}
		if (c > 0 && c <= 0x10FFFF) {
			c -= 0x10000;
			m_convertedBuffer.push_back(0xD800 | (c>>10));
			m_convertedBuffer.push_back(0xDC00 | (c&0x3FF));
			continue;
		}

		if (c == CHARERR_NEED_MORE) {
			size_t count = m_bufferCount - m_bufferPos;
			memmove(m_readbuffer, m_readbuffer + m_bufferPos, count);
			m_bufferCount = count + Read(m_readbuffer + count, sizeof(m_readbuffer) - count);
			m_bufferPos = 0;
			if (m_bufferCount == count) return true;
			continue;
		}

		m_convertedBuffer.push_back(L'?');
	}
}

int CTextFile::NextChar() {
	unsigned char c = m_readbuffer[m_bufferPos++];
	if (Utf8::isSingleByte(c)) return c;

	int nContinuationBytes = Utf8::continuationBytes(c);
	if (m_bufferCount - m_bufferPos < nContinuationBytes) {
		--m_bufferPos;
		return CHARERR_NEED_MORE;
	}

	unsigned char *rdbuf = m_readbuffer + m_bufferPos - 1;
	m_bufferPos += nContinuationBytes;

	switch (nContinuationBytes) {
	case 1: // 110xxxxx 10xxxxxx
		return (rdbuf[0] & 0x1f) << 6 | (rdbuf[1] & 0x3f);
	case 2: // 1110xxxx 10xxxxxx 10xxxxxx
		return (rdbuf[0] & 0x0f) << 12 | (rdbuf[1] & 0x3f) << 6 | (rdbuf[2] & 0x3f);
	case 3: // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		return (rdbuf[0] & 0x07) << 18 | (rdbuf[1] & 0x3f) << 12 | (rdbuf[2] & 0x3f) << 6 | (rdbuf[3] & 0x3f);
	}

	return 0;
}

BOOL CTextFile::ReadString(CStringW& str) {
	bool fEOF = ReadLine();

	str.Empty();
	if (!m_convertedBuffer.empty()) {
		str.Append(&m_convertedBuffer[0], m_convertedBuffer.size());
	}
	return !fEOF;
}
