var net = require("net");
var sylvester =require("sylvester"),
    Matrix = sylvester.Matrix;

const MAX_BOARD_SIZE     = 99;
const DEFAULT_BOARD_SIZE = 19;

const port = process.argv[3] ? process.argv[3] : 8037;

// Headers          // Send / Receive
const BOARD  = 'b'; // Send board dimensions
const SCORE  = 'c'; // Send player scores
const END    = 'e'; // End the game
const KILL   = 'k'; // / Kill marked stones
const MARK   = 'm'; // Send marked state / receive mark
const PLAYER = 'p'; // Send player status (white/black/neither)
const RESET  = 'r'; // / Restart game
const SET    = 's'; // Send stone state / receive move
const TURN   = 't'; // Send turn information / switch turn
const UNDO   = 'u'; // Send turn information / switch turn
const KICK_ALL = '\t'; // Kick all clients
var handlers = {}

// Stones
const white =  1;
const black = -1;
const empty =  0;

// Clients list and current players
var clients = {};
var players = {};

// Game variables
var game = {};
game.playing;
game.currentPlayer;
game.pass; // was last move a pass?
game.board; // board array
game.marks; // mark array
game.deltas = []; // move deltas
game.score = []; // black and white captures
game.boardSize;
if (process.argv[2] && process.argv[2] < MAX_BOARD_SIZE) {
    game.boardSize = process.argv[2];
} else {
    game.boardSize = DEFAULT_BOARD_SIZE;
}

function startGame() {
    game.score[white] = game.score[black] = 0;
    game.currentPlayer = black;
    game.pass = false;
    game.playing = true;

    game.board = Matrix.Zero(game.boardSize, game.boardSize);
    game.marks = new Array(game.boardSize);
    for (let i = 0; i < game.boardSize; ++i) {
        game.marks[i] = new Array(game.boardSize);
        for (let j = 0; j < game.boardSize; ++j) {
            game.marks[i][j] = false;
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
    if (!game.playing) sendMessage(c, END);

    console.log("Client " + c.id + " connected");

    // register event handlers
    c.on("data", processData);
    c.on("end", function() {
        console.log("Client " + this.id + " disconnecting");
    })
    c.on("close", function() {
        let id = this.id;
        delete players[this.color];
        delete clients[id];
        console.log("Client " + id + " disconnected");
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
    console.log(header,data);
    if (data !== undefined) {
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

function sendMark(client, x, y) {
    let data = new Buffer(9, "hex");
    data.writeUInt32LE(x, 0);
    data.writeUInt32LE(y, 4);
    data.writeUInt8(game.marks[x][y], 8);

    sendMessage(client, MARK, data);
}

function sendStone(client, x, y) {
    let stone = game.board.elements[x][y];
    let data = new Buffer(9, "hex");
    data.writeUInt32LE(x, 0);
    data.writeUInt32LE(y, 4);
    data.write(encodeColor(stone), 8, 1, "ascii")

    sendMessage(client, SET, data);
}

function sendDelta(client, delta) {
    // send stones
    for (let i = 0; i < game.boardSize; ++i) {
        for (let j = 0; j < game.boardSize; ++j) {
            if (delta.elements[i][j] != 0) {
                sendStone(client, i, j);
            }
        }
    }
}

function sendBoard(client) {
    // send grid size
    let data = new Buffer(4, "hex");
    data.writeUInt32LE(game.boardSize, 0);
    sendMessage(client, BOARD, data);

    // send grid data
    for (let i = 0; i < game.boardSize; ++i) {
        for (let j = 0; j < game.boardSize; ++j) {
            sendStone(client, i, j);
        }
    }
}

function sendScore(client) {
    let data = new Buffer(8, "hex");
    data.writeUInt32LE(game.score[black], 0);
    data.writeUInt32LE(game.score[white], 4);
    sendMessage(client, SCORE, data);
}

function sendTurn(client) {
    sendMessage(client, TURN, encodeColor(game.currentPlayer));
}

function encodeColor(color) {
    switch (color) {
        case black: return 'b'; break;
        case white: return 'w'; break;
        case empty: return 'e'; break;
    }
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

    sendMessage(client, PLAYER, encodeColor(color));
}

function validMove(color, x, y, delta) {
    if (outOfBounds(x, y)) return false;
    if (!game.playing) return false;
    if (color != game.currentPlayer) return false;
    if (game.board.elements[x][y] != empty) return false;
    if (ko(delta)) return false;

    let board = game.board.add(delta);
    if(isSafeNeighbour(x + 1, y, board) || isSafeNeighbour(x, y + 1, board) ||
       isSafeNeighbour(x - 1, y, board) || isSafeNeighbour(x, y - 1, board)) {
        return true;
    }

    return isAlive(getGroup(x, y, board), board);
}

// is this square a safe neighbour
function isSafeNeighbour(x, y, board) {
    if (outOfBounds(x, y)) return false;
    if (board.elements[x][y] == empty) return true; // safe if empty
    else return false;
}

function isAlive(group, board) {
    if (board === undefined) {
        board = game.board;
    }

    for (let stone in group) {
        let x = group[stone].x;
        let y = group[stone].y;
        if (isEmpty(x + 1, y, board) ||
            isEmpty(x, y + 1, board) ||
            isEmpty(x - 1, y, board) ||
            isEmpty(x, y - 1, board)) {
            return true;
        }
    }
    return false;
}

function isEmpty(x, y, board) {
    if (outOfBounds(x, y)) return false;
    if (board === undefined) {
        board = game.board;
    }
    return board.elements[x][y] == empty;
}

function ko(delta) {
    if (game.deltas.length == 0) return false;
    return game.deltas[game.deltas.length - 1].add(delta)
       .eql(Matrix.Zero(game.boardSize, game.boardSize));
}

function getGroup(x, y, board) {
    if (board === undefined) {
        board = game.board;
    }
    let group = [];
    Object.defineProperty(group, "color", { 
        value: board.elements[x][y],
        enumerable: false,
    });

    let findInGroup = function (x, y) { 
        for (let stone in group) {
            if (group[stone].x == x && group[stone].y == y) return true;
        }
        return false;
    }
    // Naive search
    let addToGroup = function (x, y) {
        if (outOfBounds(x, y)) return;
        if (board.elements[x][y] == group.color) {
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

function resolveMark(x, y) {
    let isMarked = !game.marks[x][y];

    let group = getGroup(x,y);
    // TODO Coordinate class and neighbours()
    for (let stone in group) {
        let x = group[stone].x;
        let y = group[stone].y;
        game.marks[x][y] = isMarked;
        forAllClients(function(client) {
            sendMark(client, x, y);
        });
    }
}

function calculateDelta(color, x, y) {
    let delta = Matrix.Zero(game.boardSize, game.boardSize);
    if (outOfBounds(x, y)) return delta;

    delta.elements[x][y] = color;

    // TODO Coordinate class and neighbours()
    resolveCaptures(color, x + 1, y, delta);
    resolveCaptures(color, x, y + 1, delta);
    resolveCaptures(color, x - 1, y, delta);
    resolveCaptures(color, x, y - 1, delta);

    return delta;
}

function resolveCaptures(color, x, y, delta) {
    if (!outOfBounds(x, y)) {
        let board = game.board.add(delta);
        let stone = board.elements[x][y];
        if (stone != color && stone != empty) {
            let group = getGroup(x,y);
            if (!isAlive(group, board)) {
                for (let stone in group) {
                    let x = group[stone].x;
                    let y = group[stone].y;
                    delta.elements[x][y] -= group.color;
                }
            }
        }
    }
}

function calculateScore(color, delta) {
    let sum = sumOfElements(delta);
    return (sum - color)*color;
}

function sumOfElements(matrix) {
    let sum = 0;
    for (let i = 0; i < matrix.rows(); ++i) {
        for (let j = 0; j < matrix.cols(); ++j) {
            sum += matrix.elements[i][j];
        }
    }
    return sum;
}

function switchTurn() {
    game.currentPlayer = game.currentPlayer == white ? black : white;
    forAllClients(sendTurn);
}

function outOfBounds(x, y) {
    return (x < 0 || x >= game.boardSize || y < 0 || y >= game.boardSize);
}

handlers[SET] = function(client, data) {
    let x = data.readUInt32LE(0);
    let y = data.readUInt32LE(4);
    let color = client.color;

    let delta = calculateDelta(color, x, y);
    if (validMove(color, x, y, delta)) {
        game.pass = false; // last move was not a pass
        console.log("Move:","(" + x +", " + y + ")",color);
        // change board
        game.board = game.board.add(delta);
        // store delta
        game.deltas.push(delta);

        game.score[color] += calculateScore(color, delta);
        forAllClients(function(client) {
            sendDelta(client, delta);
            sendScore(client);
        });
        switchTurn();
    } else {
        console.log("Invalid move:","(" + x +", " + y + ")",color);
        // tell the client the move failed
        sendStone(client, x, y);
    }
};

handlers[MARK] = function(client, data) {
    let x = data.readUInt32LE(0);
    let y = data.readUInt32LE(4);

    if (game.playing == false) {
        if (game.board.elements[x][y] != empty) {
            console.log("Mark:","(" + x +", " + y + ")");
            resolveMark(x, y);

            forAllClients(function(client) {
                sendScore(client);
            });
        }
    }
};

handlers[TURN] = function (client) {
    if (game.playing && client.color == game.currentPlayer) {
        if (game.pass) {
            console.log("Consecutive pass: ending game");
            game.playing = false;
            game.pass = false;
            sendAllClients(END);
        } else {
            console.log("Pass");
            game.pass = true; // last move was a pass
            game.deltas.push(Matrix.Zero(game.boardSize,game.boardSize));
            switchTurn();
        }
    }
};

handlers[RESET] = function () {
    console.log("Starting new game");
    if (!game.playing) {
        startGame();
        sendAllClients(RESET);
        forAllClients(sendBoard);
        forAllClients(sendScore);
        forAllClients(sendTurn);
    }
};

handlers[KICK_ALL] = function () {
    console.log("Kicking all clients");
    delete players[white];
    delete players[black];

    for (let id in clients) {
        clients[id].end();
        clients[id].destroy();
    }
    clients = {}
};

handlers[KILL] = function () {
    console.log("Clearing marked stones");
    for (let i = 0; i < game.boardSize; ++i) {
        for (let j = 0; j < game.boardSize; ++j) {
            if (game.marks[i][j]) {
                game.board.elements[i][j] = empty;
                game.marks[i][j] = false;

                forAllClients(function(client) {
                    sendStone(client, i, j);
                    sendMark(client, i, j);
                });
            }
        }
    }
}

handlers[UNDO] = function () {
    if (game.deltas.length != 0) {
        switchTurn();
        let lastTurn = game.deltas.pop();
        game.board = game.board.subtract(lastTurn);

        // calculate score delta
        let color = game.currentPlayer;
        if (!game.pass) {
            game.score[color] -= calculateScore(color, lastTurn);
        } else {
            game.pass = false;
            // TODO: Allow consecutive passes after undo
        }
        forAllClients(function(client) {
            sendDelta(client, lastTurn);
            sendScore(client);
        });
    }
}
