/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
 *
 * Copyright (C) 2011  Christian Urich

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef STDREDIRECTOR_H
#define STDREDIRECTOR_H
#include "dmcompilersettings.h"
#include <qdebug.h>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <cstdio>

class  DM_HELPER_DLL_EXPORT Q_DebugStream : public QObject, public std::basic_streambuf<char>
{
    Q_OBJECT
public:

    Q_DebugStream(std::ostream &stream, QPlainTextEdit* text_edit) : m_stream(stream)
    {
        log_window = text_edit;
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
        connect(this , SIGNAL(updateLog(QString)),text_edit, SLOT(appendPlainText(QString)));

    }

    ~Q_DebugStream()
    {
        // output anything that is left
        if (!m_string.empty()) {
            //log_window->appendPlainText(m_string.c_str());
            emit updateLog(m_string.c_str());
        }

        m_stream.rdbuf(m_old_buf);
    }

protected:
    virtual int_type overflow(int_type v)
    {
        if (v == '\n')
        {
            //log_window->appendPlainText(m_string.c_str());
            updateLog(m_string.c_str());
            m_string.erase(m_string.begin(), m_string.end());
        }
        else
            m_string += v;

        return v;
    }

    virtual std::streamsize xsputn(const char *p, std::streamsize n)
    {
        m_string.append(p, p + n);

        int pos = 0;
        while (pos != std::string::npos)
        {
            pos = m_string.find('\n');
            if (pos != std::string::npos)
            {
                std::string tmp(m_string.begin(), m_string.begin() + pos);
                //log_window->appendPlainText(tmp.c_str());
                updateLog(m_string.c_str());
                m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
            }
        }

        return n;
    }

public:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;
    QPlainTextEdit* log_window;

signals:
    void updateLog(QString s) ;




};


#endif // STDREDIRECTOR_H
