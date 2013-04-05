var clientId = '968301384489-fv905deok7stelhr7apil483kcjph96v.apps.googleusercontent.com';

// To enter one or more authentication scopes, refer to the documentation for the API.
var scopes = 'https://spreadsheets.google.com/feeds';

var requestURL = 'https://spreadsheets.google.com/feeds/list/0Agxrt1aeoXHOdEI5a215V2tGUXc3c3d0NTc3ZFlDWGc/od6/private/full?reverse=true'

var pin = window.localStorage.getItem('pin');

// URL of the Router -- here protected by pin but if not posting on github, who cares
var updateURL = "http://" + pin + ".selfip.net:89"

function checkAuth() {
    
    if (pin === null) {
        $('#myModal').modal('show');
        $('#savePin').click(function () {
            var pinInput = $('#pinInput').val();
            if (pinInput) {
                localStorage.setItem('pin', pinInput);
                pin = window.localStorage.getItem('pin');
                $('#myModal').modal('hide');
                checkAuth();
            }
        });
    }
    
    else {
    
        window.setTimeout(function () {
            gapi.auth.authorize({
            client_id: clientId,
            scope: scopes,
            immediate: true
        }, handleAuthResult);
        },1);
    
    }
}

function handleAuthResult(authResult) {
    var authorizeButton = document.getElementById('authorize-button');
    var updateButton = document.getElementById('update-button');
    
    if (authResult && !authResult.error) {
        authorizeButton.style.visibility = 'hidden';
        var xhr = new XMLHttpRequest();
        var oauthToken = gapi.auth.getToken();
        xhr.open('GET', requestURL + '&sq=data3=' + pin);
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
                    
                    // show the update button once data is written
                    $('#update-button').click(function() {
                        location.href = updateURL + "/" + $sensor.first().text();
                    });
                    updateButton.style.visibility = '';
                    
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