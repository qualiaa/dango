var net = require("net");

const MAX_BOARD_SIZE     = 99;
const DEFAULT_BOARD_SIZE = 19;

var boardSize;
if (process.argv[2] && process.argv[2] < MAX_BOARD_SIZE) {
    boardSize = process.argv[2];
} else {
    boardSize = DEFAULT_BOARD_SIZE;
}
const port = process.argv[3] ? process.argv[3] : 8037;

// Headers
const SET    = 's';
const TURN   = 't';
const PLAYER = 'p';
const BOARD  = 'b';
const SCORE  = 'c';
const RESET  = 'r';
var handlers = {}

// Stones
const white = 'w';
const black = 'b';
const empty = 'e';

// Clients list and current players
var clients = {}
var players = {}

// Game variables
var board;
var score = [];
var currentPlayer;

function startGame() {
    score[white] = score[black] = 0;
    currentPlayer = black;

    board = new Array(boardSize);
    for (let i = 0; i < boardSize; ++i) {
        board[i] = new Array(boardSize);
        for (let j = 0; j < boardSize; ++j) {
            board[i][j] = empty;
        }
    }
}

startGame();

// Create server
var server = net.createServer(function(c) {
    if (this.clientID == undefined) {
        this.clientID = 0;
    }

    // assign client ID
    c.id = this.clientID++;
    clients[c.id] = c;

    // send client game data
    sendBoard(c);
    sendScore(c);
    sendTurn(c);
    assignColor(c);

    console.log("Client " + c.id + " connected");

    // register event handlers
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

function processData(data) {
    let header = data.toString("ascii", 0, 1);

    handlers[header](this, data.slice(1));
}

function sendMessage(client, header, data) {
    let message = header;
    if (data !== undefined) {
        console.log(header,data);
        message += data;
    }
    message += "\r\n"

    client.write(message);
}

function forAllClients(func) {
    for (let client in clients) {
        func(clients[client]);
    }
}

function sendAllClients(header, data) {
    for(let client in clients) {
        sendMessage(clients[client], header, data);
    }
}

function sendStone(client, x, y) {
    let data = new Buffer(9, "hex");
    data.write(board[x][y], 0, 1, "ascii")
    data.writeUInt32LE(x, 1);
    data.writeUInt32LE(y, 5);

    sendMessage(client, SET, data);
}

function sendBoard(client) {
    // send grid size
    let data = new Buffer(4, "hex");
    data.writeUInt32LE(boardSize, 0);
    sendMessage(client, BOARD, data);

    // send grid data
    for (let i = 0; i < boardSize; ++i) {
        for (let j = 0; j < boardSize; ++j) {
            sendStone(client, i, j);
        }
    }
}

function sendScore(client) {
    let data = new Buffer(8, "hex");
    data.writeUInt32LE(score[black], 0);
    data.writeUInt32LE(score[white], 4);
    sendMessage(client, SCORE, data);
}

function sendTurn(client) {
    sendMessage(client, TURN, currentPlayer);
}

function assignColor(client) {
    let color = empty;
    if (players[black] === undefined) {
        players[black] = client;
        client.color = color = black;
    } else if (players[white] === undefined) {
        players[white] = client;
        client.color = color = white;
    }

    sendMessage(client, PLAYER, color)
}

function validMove(color, x, y) {
    if (outOfBounds(x, y)) return false;
    if (color != currentPlayer) return false;
    if (board[x][y] != empty) return false;

    // set board for checks
    board[x][y] = color;
    let result = false;
    result |= isSafeNeighbour(color, x + 1, y) ||
              isSafeNeighbour(color, x, y + 1) ||
              isSafeNeighbour(color, x - 1, y) ||
              isSafeNeighbour(color, x, y - 1);

    if(!result) result = isAlive(getGroup(x, y));
    // reset board
    board[x][y] = empty;
    return result;
}

// is this square a safe neighbour
function isSafeNeighbour(color, x, y) {
    if (outOfBounds(x, y)) return false;
    let stone = board[x][y];
    if (stone == empty) return true; // safe if empty
    if (stone == color) return false; 
    else return !isAlive(getGroup(x,y)); // safe if kills other stones
}

function getGroup(x, y) {
    let group = [];
    Object.defineProperty(group, "color", { 
        value: board[x][y],
        enumerable: false,
    });
    if (group.color == empty) return group;

    let findInGroup = function (x, y) { 
        for (let stone in group) {
            if (group[stone].x == x && group[stone].y == y) return true;
        }
        return false;
    }
    // Naive search
    let addToGroup = function (x, y) {
        if (outOfBounds(x, y)) return;
        if (board[x][y] == group.color) {
            if (!findInGroup(x, y)) {
                let stone = {};
                Object.defineProperties(stone, {
                    "x": { value: x, },
                    "y": { value: y, },
                });
                group.push(stone);

                addToGroup(x + 1, y);
                addToGroup(x, y + 1);
                addToGroup(x - 1, y);
                addToGroup(x, y - 1);
            }
        }
    };
    addToGroup(x, y);
    return group;
}

function isAlive(group) {
    for (let stone in group) {
        let x = group[stone].x;
        let y = group[stone].y;
        if (isEmpty(x + 1, y) ||
            isEmpty(x, y + 1) ||
            isEmpty(x - 1, y) ||
            isEmpty(x, y - 1)) {
            return true;
        }
    }
    return false;
}

function isEmpty(x, y) {
    if (outOfBounds(x, y)) return false;
    return board[x][y] == empty;
}

function resolveMove(color, x, y) {
    board[x][y] = color;

    // TODO Coordinate class and neighbours()
    score[color] += resolveCaptures(color, x + 1, y);
    score[color] += resolveCaptures(color, x, y + 1);
    score[color] += resolveCaptures(color, x - 1, y);
    score[color] += resolveCaptures(color, x, y - 1);
}

function resolveCaptures(color, x, y) {
    if (!outOfBounds(x, y)) {
        let stone = board[x][y];
        if (stone != color && stone != empty) {
            let group = getGroup(x,y);
            if (!isAlive(group)) {
                for (let stone in group) {
                    let x = group[stone].x;
                    let y = group[stone].y;
                    board[x][y] = empty;
                    forAllClients(function(client) {
                        sendStone(client, x, y);
                    });
                }
                return group.length;
            }
        }
    }
    return 0;
}

function switchTurn() {
    currentPlayer = currentPlayer == white ? black : white;
    console.log("Switching turn:", currentPlayer);
    sendAllClients(TURN, currentPlayer);
}

function outOfBounds(x, y) {
    return (x < 0 || x >= boardSize || y < 0 || y >= boardSize);
}

handlers[SET] = function(client, data) {
    let x = data.readUInt32LE(0);
    let y = data.readUInt32LE(4);
    let color = client.color;

    if (validMove(color, x, y)) {
        console.log("Move:","(" + x +", " + y + ")",color);
        resolveMove(color, x, y);

        forAllClients(function(client) {
            sendStone(client, x, y);
            sendScore(client);
        });
        switchTurn();
    } else {
        console.log("Invalid move:","(" + x +", " + y + ")",color);
        // tell the client the move failed
        sendStone(client, x, y);
    }
}

handlers[TURN] = switchTurn;

handlers[RESET] = function () {
    startGame();
    forAllClients(sendBoard);
    forAllClients(sendScore);
    forAllClients(sendTurn);
}
