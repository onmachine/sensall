var clientId = '968301384489-fv905deok7stelhr7apil483kcjph96v.apps.googleusercontent.com';

// To enter one or more authentication scopes, refer to the documentation for the API.
var scopes = 'https://spreadsheets.google.com/feeds';

var requestURL = 'https://spreadsheets.google.com/feeds/list/0Agxrt1aeoXHOdEI5a215V2tGUXc3c3d0NTc3ZFlDWGc/od6/private/full?reverse=true'



function checkAuth() {
    gapi.auth.authorize({
        client_id: clientId,
        scope: scopes,
        immediate: true
    }, handleAuthResult);
}


function handleAuthResult(authResult) {
    var authorizeButton = document.getElementById('authorize-button');
    if (authResult && !authResult.error) {
        authorizeButton.style.visibility = 'hidden';
        var xhr = new XMLHttpRequest();
        var oauthToken = gapi.auth.getToken();
        xhr.open('GET', requestURL);
        // we could just query for a particular sensor ?sq=data1=garageDoor
        // 
        xhr.setRequestHeader('Authorization', 'Bearer ' + oauthToken.access_token);
        xhr.send();
        
        
        xhr.onreadystatechange = function() {
            
            if (xhr.readyState ==4) {
                
                var xmlDoc = $.parseXML(xhr.responseText);
                var $xml = $( xmlDoc );
                var $timestamp = $xml.find( "gsx\\:timestamp, timestamp" );
                var $sensor = $xml.find( "gsx\\:data1, data1" );
                var $status = $xml.find( "gsx\\:data2, data2" );
                
                
                // Create an HTML table for each row in the spreadsheet
                $timestamp.each(function(i) {
                    var tRow = $('<tr>');
                
                    var timeCell = $('<td>').text($(this).text());
                    var sensorCell = $('<td>').text($($sensor[i]).text());
                    var statusCell = $('<td>').text($($status[i]).text());
                    
                    $("#data").append(tRow.append(timeCell).append(sensorCell).append(statusCell));
                    
                });
                
                // Change Background Color based on Latest status, assumes reverse order
                $('body').css('background-color', function(index) {
                    var latestStatus = $status.first().text();
                    if (latestStatus == 'closed') {
                        return 'lightGreen';
                    }
                    else {
                        return 'IndianRed';
                    }
                });
            }
        };
    }
    else {
        authorizeButton.style.visibility = '';
        authorizeButton.onclick = handleAuthClick;
    }
}

function handleAuthClick(event) {
    gapi.auth.authorize({
        client_id: clientId,
        scope: scopes,
        immediate: false
    }, handleAuthResult);
    return false;
}