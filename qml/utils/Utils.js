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
