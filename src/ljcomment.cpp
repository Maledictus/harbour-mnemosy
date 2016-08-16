/*
The MIT License (MIT)

Copyright (c) 2016 Oleg Linkin <maledictusdemagog@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include "ljcomment.h"

namespace Mnemosy
{
LJComment::LJComment()
: m_Priveleges(0)
, m_PosterID(0)
, m_DTalkID(0)
, m_ParentTalkID(0)
, m_Level(0)
, m_IsShow(false)
, m_IsLoaded(false)
, m_ThreadCount(0)
{
}

QUrl LJComment::GetUserPicUrl() const
{
    return m_UserPicUrl;
}

void LJComment::SetUserPicUrl(const QUrl& url)
{
    m_UserPicUrl = url;
}

LJComment::Privileges LJComment::GetPrivileges() const
{
    return m_Priveleges;
}

void LJComment::SetPrivileges(LJComment::Privileges privs)
{
    m_Priveleges = privs;
}

QList<LJComment> LJComment::GetChildren() const
{
    return m_Children;
}

QList<LJComment>& LJComment::GetChildrenRef()
{
    return m_Children;
}

void LJComment::SetChildren(const QList<LJComment>& children)
{
    m_Children = children;
}

int LJComment::GetChildrenCount() const
{
    return m_Children.count();
}

quint64 LJComment::GetPosterID() const
{
    return m_PosterID;
}

void LJComment::SetPosterID(quint64 id)
{
    m_PosterID = id;
}

QString LJComment::GetState() const
{
    return m_State;
}

void LJComment::SetState(const QString& state)
{
    m_State = state;
}

QString LJComment::GetSubject() const
{
    return m_Subject;
}

void LJComment::SetSubject(const QString& subject)
{
    m_Subject = subject;
}

QString LJComment::GetBody() const
{
    return m_Body;
}

void LJComment::SetBody(const QString& body)
{
    m_Body = body;
}

QUrl LJComment::GetPosterPicUrl() const
{
    return m_PosterPicUrl.isEmpty() ?
            QUrl("qrc:/images/blank_boy.png") :
            m_PosterPicUrl;
}

void LJComment::SetPosterPicUrl(const QUrl& url)
{
    m_PosterPicUrl = url;
}

quint64 LJComment::GetDTalkID() const
{
    return m_DTalkID;
}

void LJComment::SetDTalkID(quint64 id)
{
    m_DTalkID = id;
}

QString LJComment::GetPosterName() const
{
    return m_PosterName;
}

void LJComment::SetPosterName(const QString& name)
{
    m_PosterName = name;
}

QDateTime LJComment::GetDatePostUnix() const
{
    return m_DatePostUnix;
}

void LJComment::SetDatePostUnix(const QDateTime& dt)
{
    m_DatePostUnix = dt;
}

quint64 LJComment::GetParentTalkID() const
{
    return m_ParentTalkID;
}

void LJComment::SetParentTalkID(quint64 id)
{
    m_ParentTalkID = id;
}

int LJComment::GetLevel() const
{
    return m_Level;
}

void LJComment::SetLevel(int level)
{
    m_Level = level;
}

bool LJComment::IsShown() const
{
    return m_IsShow;
}

void LJComment::SetShown(bool show)
{
    m_IsShow = show;
}

bool LJComment::IsLoaded() const
{
    return m_IsLoaded;
}

void LJComment::SetLoaded(bool loaded)
{
    m_IsLoaded = loaded;
}

LJCommentProperties LJComment::GetProperties() const
{
    return m_Properties;
}

void LJComment::SetProperties(const LJCommentProperties& props)
{
    m_Properties = props;
}

quint64 LJComment::GetThreadCount() const
{
    return m_ThreadCount;
}

void LJComment::SetThreadCount(quint64 count)
{
     m_ThreadCount = count;
}

bool LJComment::GetHasArgs() const
{
    return m_HasArgs;
}

void LJComment::SetHasArgs(bool has)
{
    m_HasArgs = has;
}
} // namespace Mnemosy
