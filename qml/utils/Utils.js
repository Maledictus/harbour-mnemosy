/*
The MIT License (MIT)

Copyright (c) 2016-2018 Oleg Linkin <maledictusdemagog@gmail.com>

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

.pragma library

function generateDateString(dt, format) {
    var today = new Date()
    var date = "";
    if (today.getDate() === dt.getDate() &&
            today.getMonth() === dt.getMonth() &&
            today.getFullYear() === dt.getFullYear()) {
        date = qsTr("Today") + ", "
    }
    else if (today.getDate() === dt.getDate() + 1 &&
            today.getMonth() === dt.getMonth() &&
            today.getFullYear() === dt.getFullYear()){
        date = qsTr("Yesterday") + ", "
    }

    if(date !== "") {
        return date + " " + Qt.formatTime(dt, "hh:mm")
    }

    return Qt.formatDateTime(dt, format)
}

function getLJUserFromLink(link) {
    var userPattern = /^(.+:\/\/)?(.+)\.livejournal\.com$/g
    var result
    var journalName
    while (result = userPattern.exec(link)) {
        journalName = result[2]
    }
    return journalName
}

function getLJEntryFromLink(link) {
    var entryPattern = /^(.+:\/\/)?(.+)\.livejournal\.com\/(\d+).html$/ig
    var isEntry = false
    var entryId
    var journalName
    var result
    while (result = entryPattern.exec(link)) {
        journalName = result[2]
        entryId = result[3]
    }
    return [journalName, entryId]
}
