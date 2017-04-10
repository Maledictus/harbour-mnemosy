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
