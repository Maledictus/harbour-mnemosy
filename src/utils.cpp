/*
The MIT License(MIT)

//Copyright (c) 2016 Oleg Linkin <maledictusdemagog@gmail.com>

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

#include "utils.h"

#include <QList>
#include <QRegExp>
#include <QRegularExpression>

namespace Mnemosy
{
namespace Utils
{
void SetImagesWidth(QString& text, bool& hasArg)
{
    QRegExp imgRxp ("\\<img[^\\>]*src\\s*=\\s*\"[^\"]*\"[^\\>]*\\>",
            Qt::CaseInsensitive);
    imgRxp.setMinimal(true);
    int offset = 0;
    QList<std::tuple<QString, QString, int>> matched;
    while ((offset = imgRxp.indexIn(text, offset)) != -1)
    {
        QString imgTag = imgRxp.cap(0);
        if (!imgTag.contains("l-stat.livejournal.net"))
        {
            QRegExp urlRxp("src\\s*=\\s*[\"']([^\"]*)[\"']");
            QString url;
            if (urlRxp.indexIn(imgTag) != -1)
                url = urlRxp.cap(1);
            int width = 0;
            QRegExp widthRxp("width\\s*=\\s*[\"'](\\d+)[\"']");
            if (widthRxp.indexIn(imgTag) != -1)
                width = widthRxp.cap(1).toInt ();

            matched << std::make_tuple(imgTag, url, width);
        }
        offset += imgRxp.matchedLength();
    }

    for (const auto& t : matched)
    {
        text.replace (std::get<0>(t),
                "<img src=\"" + std::get<1>(t) + QString("\" width=\"%1\" />"));
        hasArg = true;
    }
}

void MoveFirstImageToTheTop(QString& text)
{
    QRegularExpression imgRxp("(\\<img[\\w\\W]+?/\\>)",
            QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = imgRxp.match(text);
    if (match.hasMatch())
    {
        QString matched = match.captured(0);
        text.remove(match.capturedStart(0), match.capturedLength(0));
        text.push_front(matched);
    }
}

void RemoveStyleTag(QString& text)
{
    QRegularExpression styleRxp ("style=\"(.+?)\"",
            QRegularExpression::CaseInsensitiveOption);
    text.remove(styleRxp);
}

}
} // namespace Mnemosy

