/*
The MIT License(MIT)

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

#pragma once

#include <QDateTime>
#include <QUrl>

#include "ljcommentproperties.h"

namespace Mnemosy
{
class LJComment
{
public:
    enum Privilege
    {
        PFreeze = 1,
        PUnbest = 2,
        PBan = 4,
        PEdit = 8,
        PUnspam = 16,
        PUnfreeze = 32,
        PReply = 64,
        PSpam = 128,
        PScreen = 256,
        PDelete = 512,
        PUnscreen = 1024,
        PBest = 2048
    };
    Q_DECLARE_FLAGS(Privileges, Privilege)
private:
    QUrl m_UserPicUrl;
    Privileges m_Priveleges;
    QList<LJComment> m_Children;
    quint64 m_PosterID;
    QString m_State;
    QString m_Subject;
    QString m_Body;
    QUrl m_PosterPicUrl;
    quint64 m_DTalkID;
    QString m_PosterName;
    QDateTime m_DatePostUnix;
    quint64 m_ParentTalkID;
    int m_Level;
    bool m_IsShow;
    bool m_IsLoaded;
    LJCommentProperties m_Properties;

public:
    explicit LJComment();

    QUrl GetUserPicUrl() const;
    void SetUserPicUrl(const QUrl& url);
    LJComment::Privileges GetPrivileges() const;
    void SetPrivileges(Privileges privs);
    QList<LJComment> GetChildren() const;
    void SetChildren(const QList<LJComment>& children);
    quint64 GetPosterID() const;
    void SetPosterID(quint64 id);
    QString GetState() const;
    void SetState(const QString& state);
    QString GetSubject() const;
    void SetSubject(const QString& subject);
    QString GetBody() const;
    void SetBody(const QString& body);
    QUrl GetPosterPicUrl() const;
    void SetPosterPicUrl(const QUrl& url);
    quint64 GetDTalkID() const;
    void SetDTalkID(quint64 id);
    QString GetPosterName() const;
    void SetPosterName(const QString& name);
    QDateTime GetDatePostUnix() const;
    void SetDatePostUnix(const QDateTime& dt);
    quint64 GetParentTalkID() const;
    void SetParentTalkID(quint64 id);
    int GetLevel() const;
    void SetLevel(int level);
    bool IsShown() const;
    void SetShown(bool show);
    bool IsLoaded() const;
    void SetLoaded(bool loaded);
    LJCommentProperties GetProperties() const;
    void SetProperties(const LJCommentProperties& props);
};

typedef QList<LJComment> LJComments_t;

struct LJPostComments
{
    LJPostComments()
    : m_TopItemFirst(0)
    , m_TopItems(0)
    , m_Page(0)
    , m_TopItemLast(0)
    , m_DItemId(0)
    , m_PageSize(0)
    , m_BestDTalkId(0)
    , m_Pages(0)
    {}

    void Reset()
    {
        m_TopItemFirst = 0;
        m_TopItems = 0;
        m_Page = 0;
        m_TopItemLast = 0;
        m_DItemId = 0;
        m_PageSize = 0;
        m_BestDTalkId = 0;
        m_Pages = 0;
        m_LastSync = QDateTime();
        m_Comments.clear();
    }

    quint64 m_TopItemFirst;
    quint64 m_TopItems;
    quint64 m_Page;
    quint64 m_TopItemLast;
    LJComments_t m_Comments;
    quint64 m_DItemId;
    quint64 m_PageSize;
    QDateTime m_LastSync;
    quint64 m_BestDTalkId;
    quint64 m_Pages;
};
}
Q_DECLARE_OPERATORS_FOR_FLAGS(Mnemosy::LJComment::Privileges)
