var http = require('http'),
    https = require('https'),
    url = require("url"),
    search;


http.createServer(function (req, res) {
   res.writeHead(200, {'Content-Type': 'text/plain'});
   var qs = url.parse(req.url);
   search = qs.search;
   console.log('search: ' + search);
   


https.get('https://docs.google.com/forms/d/1ZXLC7TiT334peJEwNNeAfg0I2XpWBj21NpQFLyRnJCA/formResponse' + search, function(getres) {
  console.log("statusCode: ", getres.statusCode);
  console.log("headers: ", getres.headers);

  getres.on('data', function(d) {
    process.stdout.write(d);
    res.write(d);
  });

}).on('error', function(e) {
  console.error(e);
});


res.end();
}).listen(process.env.PORT);