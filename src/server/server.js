var net = require("net");

//TODO: check args
var boardSize;
if (process.argv[2] && process.argv[2] < 0x40) {
    boardSize = process.argv[2];
} else {
    boardSize = 19;
}
const port = process.argv[3] ? process.argv[3] : 0;

// Headers
const SET = 's';
const TURN = 't';
const CONNECTION = 'c';

// Clients list and current players
var clients = {}
var players = {}

// Initialize board to empty
var board = new Array(boardSize);

for (let i = 0; i < boardSize; ++i) {
    board[i] = new Array(boardSize);
    for (let j = 0; j < boardSize; ++j) {
        board[i][j] = 'e';
    }
}

for (let i = 0; i < boardSize; ++i) {
    board[i][i] = i % 2 ? 'w' : 'b';
}

// Create server
var server = net.createServer(function(c) {
    if (this.clientID == undefined) {
        this.clientID = 0;
    }

    c.id = this.clientID++;
    clients[c.id] = c;

    console.log("Client " + c.id + " connected");

    if (players["b"] === undefined) {
        players["b"] = c;
        c.color = "b";
    } else if (players["w"] === undefined) {
        players["w"] = c;
        c.color = "w";
    }

    sendBoard(c);

    c.on("data", processData);
    c.on("end", function() {
        console.log("Client " + this.id + " disconnecting");
    })
    c.on("close", function() {
        let id = this.id;
        console.log("Client " + id + " disconnected");
        delete players[this.color];
        delete clients[id];
    });
});

server.on("listening", function() {
    console.log("Server listening on", this.address()["port"]);
});

server.listen(port);

function processData(data)
{
    console.log(this.id + ": ", data);
}

function sendMessage(client, header, data)
{
    let message = header + data + "\r\n";

    console.log("Sending",client.id,message);
    client.write(message);
}

function sendAll(header, data)
{
    for(let client in clients) {
        sendMessage(clients[client], header, data);
    }
}

function sendBoard(client) {
    for (let i = 0; i < boardSize; ++i) {
        for (let j = 0; j < boardSize; ++j) {
            let data = new Buffer(12, "hex");
            data.write('s' + board[i][j], 0, 2, "ascii")
            data.writeUInt32LE(i, 2);
            data.writeUInt32LE(j, 6);
            data.write("\r\n", 10, 2, "ascii");

            client.write(data);
        }
    }
}
