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
const PLAYER = 'p';
const BOARD = 'b';
var handlers = {}

// Clients list and current players
var clients = {}
var players = {}
var currentPlayer = 'b';

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

    // assign client ID
    c.id = this.clientID++;
    clients[c.id] = c;

    // send client game data
    assignColor(c);
    sendBoard(c);

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

    //try {
        handlers[header](this, data.slice(1));
    //}
    //catch (error) {
        //console.log(error);
    //}
}

function sendMessage(client, header, data) {
    let message = header;
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

function sendAll(header, data) {
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

function assignColor(client) {
    let col = 'x';
    if (players['b'] === undefined) {
        players['b'] = client;
        client.color = col = 'b';
    } else if (players['w'] === undefined) {
        players['w'] = client;
        client.color = col = 'w';
    }

    sendMessage(client, PLAYER, col)
}

function validMove(color, x, y) {
    if (outOfBounds(x, y)) return false;
    //if (board[x][y] != 'e') return false; //check happens in handlers[SET]
    if (isSafeNeighbour(color, x + 1, y) ||
        isSafeNeighbour(color, x, y + 1) ||
        isSafeNeighbour(color, x - 1, y) ||
        isSafeNeighbour(color, x, y - 1))
    {
        console.log("Valid: safe neighbour");
        return true;
    }

    console.log("Safe if group alive")
    return isAlive(getChain(x, y));
}

// is this square a safe neighbour
function isSafeNeighbour(color, x, y) {
    if (outOfBounds(x, y)) return false;
    let stone = board[x][y];
    if (stone == 'e') 
    {
        console.log("Safe: neighbour empty");
        return true; // safe if empty
    }
    if (stone == color) { // not necessarily safe if forms chain
        return false; 
    }

    console.log("Safe if neighbour dead:")
    return !isAlive(getChain(x,y)); // safe if kills other stones
}

function getChain(x, y) {
    let chain = [];
    Object.defineProperty(chain, "color", { 
        value: board[x][y],
        enumerable: false,
    });
    if (chain.color == 'e') return chain;

    let findInChain = function (x, y) { 
        for (let stone in chain) {
            let x1 = chain[stone].x;
            let y1 = chain[stone].y;
            let result = chain[stone].x == x && chain[stone].y == y;
            let message = result ? "equal to" : "not equal to";
            console.log("\t" + x + ",",y,message,x1 + ",",y1)
            if (result) return result;
        }
        return false;
    }
    let addToChain = function (x, y) {
        // Naive implementation
        if (outOfBounds(x, y)) return;
        if (board[x][y] == chain.color) {
            console.log("Looking for stone (" + x + ",", y + ") in chain");
            if (!findInChain(x, y)) {
                let stone = {};
                Object.defineProperties(stone, {
                    "x": { value: x, },
                    "y": { value: y, },
                });
                chain.push(stone);
                console.log("\tNot in chain: added (" + x + ",", y + ")");

                addToChain(x + 1, y);
                addToChain(x, y + 1);
                addToChain(x - 1, y);
                addToChain(x, y - 1);
            } else {
                console.log("\tFound in chain: (" + x + ",", y + ")");
            }
        }
    };
    console.log("Starting chain");
    addToChain(x, y);
    return chain;
}

function isAlive(chain) {
    console.log ("Checking state of",chain.length,"stones");
    for (let stone in chain) {
        let x = chain[stone].x;
        let y = chain[stone].y;

        console.log ("\t(" + x +",", y + ")");
        if (isEmpty(x + 1, y) ||
            isEmpty(x, y + 1) ||
            isEmpty(x - 1, y) ||
            isEmpty(x, y - 1)) {
            console.log ("\tAlive");
            return true;
        }
    }
    console.log ("\tDead");
    return false;
}

function isEmpty(x, y) {
    console.log ("\t\tChecking if(" + x +",", y + ") is empty");
    if (outOfBounds(x, y)) return false;

    return board[x][y] == 'e';
}

function resolveMove(color, x, y) {
    board[x][y] = color;
    forAllClients(function(client) {
        sendStone(client, x, y);
    });
}

function switchTurn() {
    currentPlayer = currentPlayer == 'w' ? 'b' : 'w';
    sendAll(TURN);
}

function outOfBounds(x, y) {
    return (x < 0 || x >= boardSize || y < 0 || y >= boardSize);
}

handlers[SET] = function(client, data) {
    let x = data.readUInt32LE(0);
    let y = data.readUInt32LE(4);
    let color = client.color;
    let oldColor = board[x][y];
    board[x][y] = color;

    if (color == currentPlayer) {
        if (oldColor == 'e' && validMove(color, x, y)) {
            resolveMove(color, x, y);
            switchTurn();
        } else {
            // tell the client the move failed
            board[x][y] = oldColor;
            sendStone(client, x, y);
        }
    }
}

handlers[TURN] = switchTurn;
