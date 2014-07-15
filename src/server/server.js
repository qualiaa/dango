var net = require("net");

var server = net.createServer(function(c) {
    console.log("Client connected");

    c.on("end", function() {
        console.log("Client disconnected");
    });
    c.write("hello\r\n");
    c.pipe(c);
});

server.listen(8124);

server.on("listening", function() { 
    console.log("Server listening");
});
