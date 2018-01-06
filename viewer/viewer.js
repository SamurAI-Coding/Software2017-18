var svg;
var field;
var courseFig;
var ns;

var raceLog;
var course;
var step;
var gameLength;

const raceLogFileType = 'race log';

var mag = 1;
var ylimit;
const xmargin = 10;
const ymargin = 10;
var fieldHeight;
var fieldWidth;

function gridX(x, y) { return mag*(x+0.5)+xmargin; }
function gridY(x, y) { return mag*(ylimit-y)+ymargin; }
function obstX(x, y) { return gridX(x, y); }
function obstY(x, y) { return gridY(x, y); }

const backgroundColor = 'lightgreen';
const afterGoalColor = 'lightgray';
const goalLineColor = 'white';
const focusedFill = 'yellow';
const obstacleFill = 'brown';

const gridDotColor = 'yellow';
const obstDotColor = 'black';
const gridDotRadiusRatio = '0.15';
var gridDotRadius;

const obstColor = 'brown';
const obstOpacity = '1.0';
const obstLineWidth = '4';

const playerFill = ['pink', 'skyblue'];
const collisionFill = 'gray';
const failedFill = 'tomato';
const playerStroke = ['red', 'blue'];
const playerStrokeWidth = 1;

const moveStroke = ['gray', 'gray' ];
const pastMoveStroke = ['red', 'blue'];
const moveStrokeWidth = 0.1;
const moveDotFill = ['red', 'blue'];
const moveDotRadiusRatio = '0.15';
var moveDotRadius;

var players = [];
var moveTrace;

const logoWidth = 4;
const logoHeight = 2;
const logoMargin = 0.1;
const logoProb = 0.5;
const numLogos = 5;

var camptown = document.createElement('audio');

function loadFile(evt) {
  if (evt.target.files.length === 0) return;
  var file = evt.target.files[0];
  var reader = new FileReader();
  reader.onload =
    function(ev) {
      openingSound.play();
      var newLog = JSON.parse(ev.target.result);
      if (newLog.filetype != raceLogFileType) {
	alert('The file does not contain SamurAI Jockey race log data');
	return;
      }
      raceLog = newLog;
      course = raceLog.course;
      ylimit = course.length;
      for (var y = course.length+1; y < course.obstacles.length; y++) {
	var obsts = course.obstacles[y];
	for (var x = 0; x != obsts.length; x++) {
	  if (obsts[x] != 0 && y > ylimit) ylimit = y;
	}
      }
      ++ylimit;
      gameLength = Math.max(raceLog.log0.length, raceLog.log1.length);
      for (var p = 0; p != 2; p++) {
	var name = raceLog['name'+p];
	var playerName = document.getElementById('name'+p);
	playerName.innerHTML = name;
      }
      drawCourse();
      setStep(0);
    };
  reader.readAsText(file);
}

function windowResized(evt) {
  if (course) {
    drawCourse();
    setStep(step);
  }
}

function makePlayerIcon(player) {
  var g = document.createElementNS(ns, 'g');
  var move = document.createElementNS(ns, 'line');
  move.style.stroke = moveStroke[player.id];
  move.style['stroke-width'] = mag*moveStrokeWidth;
  move.setAttribute('x1', 0);
  move.setAttribute('y1', 0);
  player.move = move;
  g.appendChild(move);
  var body = document.createElementNS(ns, 'polygon');
  body.setAttribute(
    'points',
      -0.4*mag+','+(0.45*mag)+' 0,'+(-0.45*mag)+' '+(0.4*mag)+','+(0.45*mag));
  body.style.fill = playerFill[player.id];
  body.style.stroke = playerStroke[player.id];
  body.style['stroke-width'] = playerStrokeWidth;
  player.body = body;
  g.appendChild(body);
  player.icon = g;
  courseDef.appendChild(g);
  var moveDot = document.createElementNS(ns, 'circle');
  moveDot.setAttribute('r', moveDotRadius);
  moveDot.style.fill = moveDotFill[player.id];
  player.moveDot = moveDot;
  g.appendChild(moveDot);
}

function makeTrace() {
  moveTrace = [];
  for (var p = 0; p != 2; p++) {
    moveTrace[p] = [];
    raceLog['log'+p].forEach(
      function (play) {
	if (play.result === 0) {
	  const bx = play.before.x;
	  const by = play.before.y;
	  const ax = play.after.x;
	  const ay = play.after.y
	  const trace = document.createElementNS(ns, 'line');
	  trace.setAttribute('x1', gridX(bx, by));
	  trace.setAttribute('y1', gridY(bx, by));
	  trace.setAttribute('x2', gridX(ax, ay));
	  trace.setAttribute('y2', gridY(ax, ay));
	  trace.style.stroke = pastMoveStroke[p];
	  trace.style['stroke-width'] = mag*moveStrokeWidth;
	  trace.step = play.step;
	  courseFig.appendChild(trace);
	  moveTrace[p][play.step] = trace;
	}
      });
  }
}

function drawPlayer(p) {
  const player = players[p];
  const log = raceLog['log' + p];
  const play = log[step];
  var failed = false;
  failed |= play && play.result === -1;
  failed |= !play && log.length > 0 && log[log.length - 1].result === -1;
  if (failed) {
    const last = log[log.length - 1];
    var px = last.after.x;
    var py = last.after.y;
    player.icon.setAttribute('display', 'block');
    player.icon.setAttribute(
      'transform', 'translate(' + gridX(px, py)+','+ gridY(px, py)+')');
    player.body.setAttribute(
      'transform', 'rotate(0)');
    player.moveDot.setAttribute('cx', 0);
    player.moveDot.setAttribute('cy', 0);
    player.move.setAttribute('x2', 0);
    player.move.setAttribute('y2', 0);
    player.body.style.fill = failedFill;
    if (play) collisionSound.play();
    var coords = document.getElementById('position'+p);
    coords.innerHTML = 'Goal@'+raceLog['time'+p]+'(failed at '+last.step+')';
    player.icon.setAttribute('display', 'block');
    return;
  }
  if (play) {
    var px = play.before.x;
    var py = play.before.y;
    player.icon.setAttribute('display', 'block');
    player.icon.setAttribute(
      'transform', 'translate(' + gridX(px, py)+','+ gridY(px, py)+')');
    var vx = play.velocity.x+play.acceleration.x;
    var vy = play.velocity.y+play.acceleration.y;
    player.body.setAttribute(
      'transform', 'rotate(' + Math.atan2(vx, vy)/Math.PI*180+ ')');
    player.moveDot.setAttribute('cx', mag*vx);
    player.moveDot.setAttribute('cy', -mag*vy);
    player.move.setAttribute('x2', mag*vx);
    player.move.setAttribute('y2', -mag*vy);
    var ok = play.result === 0;
    player.move.style['stroke-dasharray'] = ok ? '' : ''+mag/5;
    player.body.style.fill = ok ? playerFill[player.id] : collisionFill;
    if (!ok) collisionSound.play();
    var coords = document.getElementById('position'+p);
    if (py+vy >= course.length && ok) {
      coords.innerHTML = 'Goal@'+raceLog['time'+p];
    } else if (step + 1 >= course.stepLimit) {
      coords.innerHTML = 'Goal@'+raceLog['time'+p]+'(step limit over)';
    } else if (ok) {
      coords.innerHTML = '(' + (px+vx) + ',' + (py+vy) + ')';
    } else {
      coords.innerHTML = '<del>(' + (px+vx) + ',' + (py+vy) + ')</del>(' + px + ',' + py + ')';
    }
    player.icon.setAttribute('display', 'block');
  } else {
    player.icon.setAttribute('display', 'none');
  }
}

function drawObstSqr(x, y) {
  var sqr = document.createElementNS(ns, 'rect');
  sqr.setAttribute('x', gridX(x-1, y+1));
  sqr.setAttribute('y', gridY(x-1, y+1));
  sqr.setAttribute('width', mag);
  sqr.setAttribute('height', mag);
  sqr.style.fill = obstColor;
  sqr.style.stroke = obstColor;
  sqr.style.opacity = obstOpacity;
  courseFig.appendChild(sqr);
}

var nextLogo = 0;
function drawLogo(x, y) {
  var sqr = document.createElementNS(ns, 'rect');
  sqr.setAttribute('x', gridX(x-1, y+logoHeight));
  sqr.setAttribute('y', gridY(x-1, y+logoHeight));
  sqr.setAttribute('width', mag*logoWidth);
  sqr.setAttribute('height', mag*logoHeight);
  sqr.style.fill = obstColor;
  sqr.style.stroke = obstColor;
  sqr.style.opacity = obstOpacity;
  courseFig.appendChild(sqr);
  var logo = document.createElementNS(ns, 'image');
  logo.setAttribute('x', gridX(x-1, y+logoHeight)+logoMargin*mag*logoWidth);
  logo.setAttribute('y', gridY(x-1, y+logoHeight)+logoMargin*mag*logoHeight);
  logo.setAttribute('width', (1-2*logoMargin)*mag*logoWidth);
  logo.setAttribute('height', (1-2*logoMargin)*mag*logoHeight);
  logo.setAttribute('href', 'logos/logo'+nextLogo+'.png');
  nextLogo = (nextLogo+1)%numLogos;
  courseFig.appendChild(logo);
}

function drawObstTrgl(x1, y1, x2, y2, x3, y3) {
  var trgl = document.createElementNS(ns, 'polygon');
  trgl.setAttribute(
    'points',
    gridX(x1, y1)+','+gridY(x1, y1)+' '+
      gridX(x2, y2)+','+gridY(x2, y2)+' '+gridX(x3, y3)+','+gridY(x3, y3));
  trgl.style.fill = obstColor;
  trgl.style.opacity = obstOpacity;
  trgl.style.stroke = 'none';
  courseFig.appendChild(trgl);
}

function drawObstLine(x1, y1, x2, y2) {
  var line = document.createElementNS(ns, 'line');
  line.setAttribute('x1', obstX(x1, y1));
  line.setAttribute('y1', obstY(x1, y1));
  line.setAttribute('x2', obstX(x2, y2));
  line.setAttribute('y2', obstY(x2, y2));
  line.style.stroke = obstColor;
  line.style.opacity = obstOpacity;
  line.style['stroke-width'] = obstLineWidth;
  courseFig.appendChild(line);
}

function drawCourse() {
  svg = document.getElementById('svg');
  ns = svg.namespaceURI;
  // Clear the SVG
  while (svg.firstChild) svg.removeChild(svg.firstChild);
  // Size of the SVG
  svgWidth = svg.width.baseVal.value;
  svgHeight = svg.height.baseVal.value;
  // The overview image should fit in the SVG
  var overviewPitch =
      Math.min(svgHeight/(ylimit),
	       0.3*svgWidth/(course.width+1));
  var overviewWidth = overviewPitch*(course.width+1);
  // The rest of the space is used by the magnified display
  fieldWidth = svgWidth-overviewWidth-2*xmargin;
  mag = Math.min(64, Math.min(fieldWidth/course.width, svgHeight/11));
  field = document.createElementNS(ns, 'svg');
  field.setAttribute('height', svgHeight);
  field.setAttribute('width', fieldWidth);
  field.setAttribute('x', Math.max(0, (fieldWidth - (course.width + 1) * mag) / 2 ));
  field.setAttribute('y', 0);
  // Draw the course on an SVG group "courseFig"
  gridDotRadius = mag*gridDotRadiusRatio;
  moveDotRadius = mag*moveDotRadiusRatio;
  var defs = document.createElementNS(ns, 'defs');
  courseFig = document.createElementNS(ns, 'g');
  courseFig.setAttribute('id', 'courseDef');
  defs.appendChild(courseFig);
  var bg = document.createElementNS(ns, 'rect');
  bg.setAttribute('x', xmargin);
  bg.setAttribute('y', mag*(ylimit-course.length)+ymargin);
  bg.setAttribute('width', mag*course.width);
  bg.setAttribute('height', mag*(course.length+1));
  bg.style.fill = backgroundColor;
  courseFig.appendChild(bg);
  var ag = document.createElementNS(ns, 'rect');
  ag.setAttribute('x', xmargin);
  ag.setAttribute('y', ymargin);
  ag.setAttribute('width', mag*course.width);
  ag.setAttribute('height', mag*(ylimit-course.length));
  ag.style.fill = afterGoalColor;
  courseFig.appendChild(ag);
  // draw start, goal line
  var startLine = document.createElementNS(ns, 'line');
  startLine.setAttribute('x1', gridX(-0.5, 0));
  startLine.setAttribute('y1', gridY(-0.5, 0));
  startLine.setAttribute('x2', gridX(course.width-0.5, 0));
  startLine.setAttribute('y2', gridY(course.width-0.5, 0));
  startLine.style.stroke = goalLineColor;
  startLine.style['stroke-width'] = 0.5*mag;
  courseFig.appendChild(startLine);
  var goalLine = document.createElementNS(ns, 'line');
  goalLine.setAttribute('x1', gridX(-0.5, course.length));
  goalLine.setAttribute('y1', gridY(-0.5, course.length));
  goalLine.setAttribute('x2', gridX(course.width-0.5, course.length));
  goalLine.setAttribute('y2', gridY(course.width-0.5, course.length));
  goalLine.style.stroke = goalLineColor;
  goalLine.style['stroke-width'] = 0.5*mag;
  courseFig.appendChild(goalLine);
  var logoCand = [];
  for (var y = 0; y != ylimit-1; y++) {
    logoCand[y] = [];
    for (var x = 1; x != course.width; x++) {
      var nw = (course.obstacles[y+1][x-1]);
      var sw = (course.obstacles[y][x-1]);
      var ne = course.obstacles[y+1][x];
      var se = course.obstacles[y][x];
      if (nw && sw && ne && se) {
	logoCand[y][x] = true;
      } else if (nw && sw) {
	if (ne)	drawObstTrgl(x-1, y+1, x-1, y, x, y+1);
	else if (se) drawObstTrgl(x-1, y+1, x-1, y, x, y);	
	else drawObstLine(x-1, y+1, x-1, y);
      } else if (ne && se) {
	if (nw) drawObstTrgl(x, y+1, x, y, x-1, y+1);
	else if (sw) drawObstTrgl(x, y+1, x, y, x-1, y);
	else if (x == course.width-1) drawObstLine(x, y+1, x, y);
      } else if (nw) {
	if (ne) drawObstLine(x-1, y+1, x, y+1);
	else if (se) drawObstLine(x-1, y+1, x, y);
      } else if (sw) {
	if (se) drawObstLine(x-1, y, x, y);
	else if (ne) drawObstLine(x-1, y, x, y+1);
      }
    }
  }
  for (var x = 0; x != course.width; x++) {
    for (var y = 0; y != ylimit; y++) {
      var dot = document.createElementNS(ns, 'circle');
      dot.setAttribute('cx', gridX(x, y));
      dot.setAttribute('cy', gridY(x, y));
      dot.setAttribute('r', gridDotRadius);
      dot.style.fill = course.obstacles[y][x] ? obstacleFill : gridDotColor;
      dot.style.stroke = 'none';
      courseFig.appendChild(dot);
    }
  }
  for (var y = 0; y != ylimit-1; y++) {
    for (var x = 1; x != course.width; x++) {
      if (logoCand[y][x]) {
	var placeLogo =
      	    x + logoWidth < course.width &&
      	    y + logoHeight < ylimit;
	if (placeLogo) {
      	  for (var k = 0; k != logoHeight; k++) {
      	    for (var j = 0; j != logoWidth; j++) {
      	      placeLogo &= logoCand[y+k][x+j];
      	    }
      	  }
      	  placeLogo &= Math.random() < logoProb;
	}
	if (placeLogo) {
  	  drawLogo(x, y);
  	  for (var k = 0; k != logoHeight; k++) {
  	    for (var j = 0; j != logoWidth; j++) {
  	      logoCand[y+k][x+j] = false;
  	    }
  	  }
	} else {
  	  drawObstSqr(x, y);
	}
      }
    }
  }
  field.appendChild(courseFig);
  // Put the course and overview drawings on the SVG
  var overview = document.createElementNS(ns, 'use');
  overview.setAttribute('href', '#courseDef');
  overview.setAttribute(
    'transform',
    'translate('+(svgWidth-overviewWidth)+',0) '+
      'scale('+overviewPitch/mag+')');
  svg.appendChild(overview);
  svg.appendChild(field);
  players[0] = { x: course.x0, y: 0, vx: 0, vy: 0, id: 0 };
  players[1] = { x: course.x1, y: 0, vx: 0, vy: 0, id: 1 };
  makeTrace();
  for (var p = 0; p != 2; p++) {
    makePlayerIcon(players[p]);
  }
}

var timerPlay = -1;

var viewOption = "bottom";
function changeViewOption(obj) {
  const start = timerPlay != -1;
  if (start) {
    stopPlay();
  }
  viewOption = obj.options[obj.selectedIndex].value;
  setStep(step);
  if (start) {
    startPlay();
  }
}

const initialStepsPerMin = 120;
var stepsPerMin = initialStepsPerMin;
function setPlaybackSpeed(value) {
  document.getElementById('stepsPerMin').innerHTML = value;
  horseSteps.playbackRate = Math.sqrt(value/initialStepsPerMin);
  stepsPerMin = value;
  if (timerPlay != -1) {
    stopPlay();
    startPlay();
  }
}

function startStop(evt) {
  if (timerPlay === -1) {
    startPlay();
  } else {
    stopPlay();
  }
}

function startPlay() {
  document.getElementById('startStop').innerHTML = "Stop";
  timerPlay = setInterval(forward, 60*1000/stepsPerMin);
  if (step == 0) {
    horseSteps.currentTime = 0;
    bgm.currentTime = 0;
  }
  horseSteps.play();
  bgm.play();
}

function stopPlay() {
  clearTimeout(timerPlay);
  timerPlay = -1;
  horseSteps.pause();
  bgm.pause();
  document.getElementById('startStop').innerHTML = "Start";
}

function rewind(evt) {
  stopPlay();
  setStep(0);
}

function stepForward(evt) {
  stopPlay();
  forward();
}

function forward() {
  if (!setStep(step+1)) {
    stopPlay();
    goalSound.play();
  }
}

function stepBackward(evt) {
  stopPlay();
  backward();
}

function backward() {
  if (!setStep(step-1)) stopPlay();
}

function setStep(c) {
  if (c < 0 || gameLength <= c || !raceLog) return false;
  step = c;
  var stepvalue = document.getElementById('stepvalue');
  stepvalue.innerHTML = step;
  for (var p = 0; p != 2; p++) {
    moveTrace[p].forEach(
      function (t) {
	if (t) {
	  t.setAttribute('display', (t.step < c ? 'block' : 'none'));
	}
      });
  }
  drawPlayer(0);
  drawPlayer(1);
  var miny = 1e10;
  var maxy = -1;
  const play0 = raceLog['log0'][step];
  var before0 = course.length;
  var after0 = course.length;
  if (play0) {
    before0 = play0.before.y;
    after0 = before0 + play0.velocity.y + play0.acceleration.y;
    miny = Math.min(Math.min(before0, after0), miny);
    maxy = Math.max(Math.min(before0, after0), maxy);
  } else if (raceLog['log0'].length > 0 && raceLog['log0'][raceLog['log0'].length - 1].result == -1) {
    const last = raceLog['log0'][raceLog['log0'].length - 1].after.y;
    before0 = after0 = last;
    miny = Math.min(last, miny);
    maxy = Math.max(last, maxy);
  }
  const play1 = raceLog['log1'][step];
  var before1 = course.length;
  var after1 = course.length;
  if (play1) {
    before1 = play1.before.y;
    after1 = before1 + play1.velocity.y + play1.acceleration.y;
    miny = Math.min(Math.min(before1, after1), miny);
    maxy = Math.max(Math.min(before1, after1), maxy);
  } else if (raceLog['log1'].length > 0 && raceLog['log1'][raceLog['log1'].length - 1].result == -1) {
    const last = raceLog['log1'][raceLog['log1'].length - 1].after.y;
    befor1 = after1 = last;
    miny = Math.min(last, miny);
    maxy = Math.max(last, maxy);
  }
  var focusY = {
    "top": maxy,
    "bottom": miny,
    "player0": Math.min(before0, after0),
    "player1": Math.min(before1, after1)
  }[viewOption];
  var offset = Math.max(svgHeight, gridY(0, focusY-1));
  field.setAttribute(
    'viewBox',
    "0,"+(offset-svgHeight)+","+fieldWidth+","+svgHeight);
  return true;
}

function addKeyboardListener() {
  document.body.addEventListener('keydown', function(event) {
    var button = 0;
    switch (event.key) {
    case "r": case "R":
      button = 'rewind'; break;
    case "b": case "B":
      button = 'back'; break;
    case "s": case "S":
      button = 'startStop'; break;
    case "f": case "F":
      button = 'forward'; break;
    case "+": case "-":
      const slider = document.getElementById('speedSlider');
      const step = parseInt(slider.step);
      const delta = event.key == "+" ? step : -step;
      const newValue = parseInt(slider.value)+delta;
      if (slider.min <= newValue && newValue <= slider.max) {
	slider.value = newValue;
	setPlaybackSpeed(newValue);
      }
      break;
    }
    if (button != 0) {
      document.getElementById(button).click();
    }
  });
}

function loadAudio(src, volume, loop) {
  const audio = new Audio();
  audio.src = src;
  audio.volume = volume;
  audio.loop = loop;
  return audio;
}

function fileExists(url) {
  var xhr = new XMLHttpRequest();
  xhr.open('HEAD', url, false);
  xhr.send();
  return xhr.status !== "404";
}

var bgm = loadAudio("camptown.wav", 1, true);
var horseSteps = loadAudio("horseSteps.wav", 1, true);
var goalSound = loadAudio("gong.wav", 1, false);
var openingSound = loadAudio("neigh.wav", 1, false);
var collisionSound = loadAudio("boyon.wav", 1, false);
