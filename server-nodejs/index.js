const app = require('express')();
const server = require('http').Server(app);
const io = require('socket.io')(server);
const fs = require('fs');

var mysocket;

const MAX = 14; // incl.
const PROG_INIT = 30;
const DEP = 5;

var curClient = 0;

let progress = 30;
var instructions = new Set();
var clientIDs = new Map();
var clientsSet = new Set();
var clientToRandom = new Map();

var servoActive = false;
var rgbActive = false;
var lastServoID = -1;
var lastRGBID = -1;

server.listen(8080);

function isServo(x) {
  return x >= 7 && x <= 9;
}

function isRGB(x) {
  return x >= 10 && x <= 14;
}

async function getRandomInt(max, lastInstrID) {
  var x = Math.floor(Math.random() * Math.floor(max));
  while (x === lastInstrID ||/*|| (servoActive && isServo(x) && x === lastServo) || (rgbActive && isRGB(x) && x === lastRGB) ||*/ instructions.has(x)) {
    // Do until you find one that can be used
    x = Math.floor(Math.random() * Math.floor(max));
  }
  if (isServo(x)) {
    servoActive = true;
    lastServo = x;
  } else if (isRGB(x)) {
    rgbActive = true;
    lastRGB = x;
  }
  instructions.add(x);
  return x;
}

// Returns a list with randoms
async function generateRandoms() {
  let randoms = [];
  await clientsSet.forEach(async (clientID) => {
    let random = await randomInt(0, MAX, -1, DEP);
    clientIDs.set(random, clientID); // map random to clientID
    clientToRandom.set(clientID, random); // map clientID to random
    randoms.push(random);
    io.sockets.emit('' + clientID, { id: random } );
  });
  console.log(randoms);
  return randoms;
}

// Starts new game
app.get('/init/:num', async (req, res) => {
  // await clientsSet.clear();
  // clientsSet.add('a');
  // clientsSet.add('b');
  // clientsSet.add('c');
  servoActive = false;
  rgbActive = false;
  console.log(clientsSet);
  let nPlayers = parseInt(req.params.num); // Number of players that are there
  if (nPlayers !== clientsSet.size) {
    let error = "ERROR: Got an init request for " + nPlayers + " players, but " + clientsSet.size + " registered.";
    console.log(error);
    return res.status(400).send({err: error});
  }
  progress = PROG_INIT;
  await instructions.clear();
  await clientIDs.clear();
  await clientToRandom.clear();
  let randomsList = await generateRandoms(); // Get all randoms (and send them to corresponding clients)
  res.status(201).send( { randoms: randomsList } ); // Send back list of randoms to Arduino
  // Make clients start their games
  io.sockets.emit('progress_change', { progress: progress }); // Send progress to all the clients
  clientsSet.forEach(clientID => {
    io.sockets.emit('game_start' + clientID, {
      id: clientToRandom.get(clientID)
    });
  });
});

app.get('/:instrID', async (req, res) => {
  let instrID = parseInt(req.params.instrID);
  if (!instructions.has(instrID)) {
    return res.status(400).send( {err: "That instruction ID was not in use" } );
  }
  if (isServo(instrID)) {
    servoActive = false;
  } else if (isRGB(instrID)) {
    rgbActive = false;
  }
  instructions.delete(instrID);
  let newInstrID = await randomInt(0, MAX, parseInt(instrID), DEP);
  instructions.add(newInstrID);
  res.status(200).send( { id: newInstrID } ); // Send new instruction ID back to Arduino
  progress = progress + 5;
  io.sockets.emit('game_start' + clientIDs.get(instrID), { id: newInstrID }); // Send new instruction id to client
  io.sockets.emit('progress_change', { progress: progress } );
  clientIDs.set(newInstrID, clientIDs.get(instrID));
});

function randomInt(min, max, lastInstrID, depth){
  if (depth === 0) {
    // Just get the first value you can find
    for (let x = min; x <= max; ++x) {
      if (x !== lastInstrID && !instructions.has(x) && (!isServo(x) || (!servoActive && lastInstrID !== x)) && (!isRGB(x) || (!rgbActive && lastInstrID !== x)))
        return x;
    }
  }
  while (true) {
    let x = Math.floor(Math.random() * (max - min + 1)) + min;
    if (x === lastInstrID || instructions.has(x) || (isServo(x) && (servoActive || lastInstrID === x)) || (isRGB(x) && (rgbActive || lastInstrID === x))) {
      //return randomInt(min, max, lastInstrID, depth - 1);
    } else {
      if (isServo(x)) {
        servoActive = true;
      } else if (isRGB(x)) {
        rgbActive = true;
      }
      instructions.add(x);
      return x;
    }
  }
}

async function sendProgressOnTimeout(uid) {
  if (progress >= 5) { 
    progress -= 5;
  } else {
    progress = 0;
  }
  let x = 0;
  // REMOVE old id from the array
  let myOldInstr = await clientToRandom.get(uid);
  //instructions.delete(myOldInstr);
  //x = await randomInt(0, MAX, myOldInstr, DEP);
  x = myOldInstr;
  console.log(x);
  io.sockets.emit('game_start' + uid, { id: x } ); // Change instruction for calling client
  io.sockets.emit('progress_change', { progress: progress } ); // Change progress for all
  clientIDs.set(x, uid);
  clientToRandom.set(uid, x);
  instructions.add(x);
  console.log(instructions);
}

function sendProgress(uid) {
  if (progress >= 5) {
    progress -= 5;
  } else {
    progress = 0;
  }
  io.sockets.emit('game_start' + uid, { id: clientToRandom.get(uid) } ); // Change instruction for calling client
  io.sockets.emit('progress_change', { progress: progress } ); // Change progress for all
  console.log(instructions);  
}

io.on('connection', (socket) => {
  // Save next id for client
  //sendProgress('a');
  let handshake = socket.handshake;
  socket.on('skt_init', (data) => {
    clientsSet.add(data.uid);
  });
  console.log("Client with IP address " + handshake.address + " connected.");
  //clientIDs.set(newInstrID, curClient++);
  socket.on('timeOut', (uid) => {
    console.log("Timeout request from: " + uid);
    sendProgress(uid);
  });
});

// Testing

// clientsSet.add('d');
// clientsSet.add('e');
// clientsSet.add('f');
// clientsSet.add('g');
// clientsSet.add('h');
// clientsSet.add('i');
// clientsSet.add('j');
