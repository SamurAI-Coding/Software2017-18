var svg;

const courseFileType = 'race course';
const editingCourseFileType = 'race course for editing';

var mag = 1;
const maxMag = 30;
const minMag = 8;

function gridX(x) { return mag*(x+0.5); }
function gridY(y) { return mag*(ylimit-y+0.5); }
function obstX(x) { return gridX(x); }
function obstY(y) { return gridY(y); }

const meshStroke = 'black';
const meshStrokeWidth = '1';

const backgroundColor = 'lightgreen';
const afterGoalColor = 'gray';
const focusedFill = 'green';
const obstacleFill = 'white';
const obstacleOpacity = '0.5';
const obstacleStrokeWidth = '3';
const obstacleStroke = 'black';

const gridDotColor = 'yellow';
const obstDotColor = 'black';
const gridDotRadiusRatio = '0.15';
var gridDotRadius;

const obstColor = 'brown';
const obstOpacity = '1.0';
const obstLineWidth = '4';

const vertexMaxRadius = '20';
var vertexRadius;
const vertexColor = 'red';
const vertexOpacity = '1';
const vertexStroke = 'black';
const vertexStrokeWidth = '2';
const focusedVertexColor = 'cyan';

const startFill = 'white';
const focusedStartFill = 'cyan';
const startStroke = 'black';
const startStrokeWidth = 1;

var courseElements;
var courseBeingEdited = false;
var polygons;
var obstArea;
var startMarks;
var startMarkMoving = false;

var startAnew;

var course = {
  filetype: editingCourseFileType,
  width: 15, length: 30, vision: 10,
  thinkTime: 20000, stepLimit: 50,
  x0: 6, x1: 8,
  obstacles: []
};

var ylimit = course.length+course.vision;

function zoom(diff) {
  var newMag = mag+diff;
  if (newMag < minMag) return;
  if (newMag > maxMag) return;
  mag = newMag;
  drawCourse();
}

function newCourse(evt) {
  startAnew = true;
  document.getElementById('sizeInputForm').style.display = 'block';
  document.getElementById('courseWidth').focus();
}

function resizeCourse(evt) {
  startAnew = false;
  document.getElementById('sizeInputForm').style.display = 'block';
  document.getElementById('courseWidth').focus();
}

function sizeSet(evt) {
  document.getElementById("sizeInputForm").style.display = 'none';
  var l = parseInt(document.getElementById("courseLength").value, 10);
  var w = parseInt(document.getElementById("courseWidth").value, 10);
  var v = parseInt(document.getElementById("vision").value, 10);
  var tl = parseInt(document.getElementById("thinkTime").value, 10);
  var sl = parseInt(document.getElementById("stepLimit").value, 10);
  if (startAnew) {
    course = { obstacles: [] };
  }
  course.filetype = editingCourseFileType;
  course.length = l;
  course.width = w;
  course.vision = v;
  ylimit = course.length+course.vision-1;
  course.thinkTime = tl;
  course.stepLimit = sl;
  course.x0 = Math.floor((w-1)/2);
  course.x1 = Math.floor((w+1)/2);
  course.obstacles.forEach(function (o) {
    o.forEach(function (p) {
      if (p.x > w) p.x = w;
      if (p.y > l+v) p.y = l+v;
    });
  });
  buildCourse();
}

function loadFile(evt) {
  if (evt.target.files.length === 0) return;
  var file = evt.target.files[0];
  var reader = new FileReader();
  reader.onload =
    function(ev) {
      var newCourse = JSON.parse(ev.target.result);
      if (newCourse.filetype != editingCourseFileType) {
	alert('The file does not contain SamurAIJockey race course data');
      } else {
	course = newCourse;
	document.getElementById("courseLength").value = course.length;
	document.getElementById("courseWidth").value = course.width;
	document.getElementById("vision").value = course.vision;
	document.getElementById("thinkTime").value = course.thinkTime;
	document.getElementById("stepLimit").value = course.stepLimit;
	ylimit = course.length+course.vision-1;
	buildCourse();
      }
    };
  reader.readAsText(file);
}

function saveFile(evt) {
  document.getElementById('saveFileForm').style.display = 'block';
  document.getElementById('saveFileName').focus();
}

function doSaveFile(evt) {
  var fileName = document.getElementById("saveFileName").value;
  document.getElementById('saveFileForm').style.display = 'none';
  var file = new Blob([JSON.stringify(course)], {type: 'application/json'});
  if (window.navigator.msSaveOrOpenBlob) {// IE10+
    window.navigator.msSaveOrOpenBlob(file, fileName);
  } else { // Others
    var a = document.createElement("a"),
        url = URL.createObjectURL(file);
    a.href = url;
    a.download = fileName;
    document.body.appendChild(a);
    a.click();
    setTimeout(function() {
      document.body.removeChild(a);
      window.URL.revokeObjectURL(url);
    }, 0);
  }
}

function writeFile(evt) {
  document.getElementById('writeFileForm').style.display = 'block';
  document.getElementById('writeFileName').focus();
}

function encodeCourse() {
  var obsts = [];
  for (var y = 0; y != ylimit+1; y++) {
    var o = [];
    for (var x = 0; x != course.width; x++) {
      o.push(obstacled[x][y] ? 1 : 0);
    }
    obsts.push(o);
  }
  return {
    "filetype": courseFileType,
    "width": course.width,
    "length": course.length,
    "vision": course.vision,
    "thinkTime": course.thinkTime,
    "stepLimit": course.stepLimit,
    "x0": course.x0,
    "x1": course.x1,
    "obstacles": obsts
  };
}
  
function doWriteFile(evt) {
  var fileName = document.getElementById("writeFileName").value;
  document.getElementById('writeFileForm').style.display = 'none';
  var file = new Blob([JSON.stringify(encodeCourse())], {type: 'application/json'});
  if (window.navigator.msSaveOrOpenBlob) {// IE10+
    window.navigator.msSaveOrOpenBlob(file, fileName);
  } else { // Others
    var a = document.createElement("a"),
        url = URL.createObjectURL(file);
    a.href = url;
    a.download = fileName;
    document.body.appendChild(a);
    a.click();
    setTimeout(function() {
      document.body.removeChild(a);
      window.URL.revokeObjectURL(url);
    }, 0);
  }
}

function editCourse(evt) {
  courseBeingEdited = !courseBeingEdited;
  if (courseBeingEdited) {
    polygons.style.display = 'block';
    evt.target.innerHTML = 'Done';
  } else {
    polygons.style.display = 'none';
    evt.target.innerHTML = 'Edit';
  }
}

function drawObstaclePolygon(o) {
  obst = document.createElementNS(ns, 'polygon');
  var ps = "";
  o.forEach(function (p) { ps += obstX(p.x) + "," + obstY(p.y) + " "; });
  obst.setAttribute("points", ps);
  obst.setAttribute("fill", obstacleFill);
  obst.setAttribute("fill-opacity", obstacleOpacity);
  obst.setAttribute("stroke", obstacleStroke);
  obst.setAttribute("stroke-width", obstacleStrokeWidth);
  polygons.appendChild(obst);
  o.forEach(function (p) {
    var c = document.createElementNS(ns, 'circle');
    c.setAttribute('cx', obstX(p.x));
    c.setAttribute('cy', obstY(p.y));
    c.setAttribute('r', vertexRadius);
    c.setAttribute('fill', vertexColor);
    c.setAttribute('fill-opacity', vertexOpacity);
    c.setAttribute('stroke', vertexStroke);
    c.setAttribute('stroke-width', vertexStrokeWidth);
    c.point = p;
    c.parent = o;
    c.onmousedown = selectVertex;
    c.onmouseover = function(evt) {
      evt.target.setAttribute('fill', focusedVertexColor);
    };
    c.onmouseout = function(evt) {
      evt.target.setAttribute('fill', vertexColor);
    };
    polygons.appendChild(c);
  });
}

var focusedItem;
var startx, starty;
var originalx, originaly;

function moveVertex(evt) {
  var newx = originalx + Math.round((evt.clientX - startx)/mag);
  var newy = originaly - Math.round((evt.clientY - starty)/mag);
  if ((focusedItem.point.x != newx || focusedItem.point.y != newy) &&
       0 <= newx && newx < course.width &&
       0 <= newy && newy < ylimit+1) {
    focusedItem.point.x = newx;
    focusedItem.point.y = newy;
    reviseCourse();
  }
}

function endMove(evt) {
  svg.removeEventListener('mousemove', moveVertex);
  svg.removeEventListener('mouseup', endMove);
  var point = focusedItem.point;
  var poly = focusedItem.parent;
  for (var k = 0; k != poly.length; k++) {
    if (poly[k] != point && poly[k].x == point.x && poly[k].y == point.y) {
      poly.splice(k, 1); break;
    }
  }
  svg.style.cursor = 'default';
}

function selectVertex(evt) {
  focusedItem = evt.target;
  var point = focusedItem.point;
  if (evt.ctrlKey) {
    // Delete the vertex
    var poly = focusedItem.parent;
    for (var k = 0; k != poly.length; k++) {
      if (poly[k] == point) {
	poly.splice(k, 1); break;
      }
    }
    if (poly.length === 0) {
      for (var k = 0; k != course.obstacles.length; k++) {
	if (course.obstacles[k] == poly) {
	  course.obstacles.splice(k, 1); break;
	}
      }
    }
    reviseCourse();
  } else {
    if (evt.shiftKey) {
      // Duplicate before moving
      var poly = focusedItem.parent;
      var dup = { x: point.x, y: point.y };
      var k = 0;
      while (poly[k] != point) k++;
      poly.splice(k, 0, dup);
    }
    startx = evt.clientX;
    starty = evt.clientY;
    originalx = point.x;
    originaly = point.y;
    svg.addEventListener('mouseup', endMove);
    svg.addEventListener('mousemove', moveVertex);
    svg.style.cursor = 'move';
  }
}

function moveStartMark(evt) {
  var newx = originalx + Math.round((evt.clientX - startx)/mag);
  if (0 <= newx && newx < course.width) {
    course[focusedItem.name] = newx;
    reviseCourse();
  }
}

function endMoveStartMark(evt) {
  svg.removeEventListener('mousemove', moveStartMark);
  svg.removeEventListener('mouseup', endMoveStartMark);
  startMarkMoving = false;
  svg.style.cursor = 'default';
}

function selectStartMark(evt) {
  if (courseBeingEdited) {
    focusedItem = evt.target;
    startMarkMoving = true;
    startx = evt.clientX;
    originalx = course[focusedItem.name];
    svg.addEventListener('mouseup', endMoveStartMark);
    svg.addEventListener('mousemove', moveStartMark);
    svg.style.cursor = 'move';
  }
}

function windowResized() {
  buildCourse();
}

function addObstacle(evt) {
  if (courseBeingEdited) {
    course.obstacles.push([evt.target.point]);
    reviseCourse();
  }
}

function addStart(what, name) {
  var x = course[what];
  var g = document.createElementNS(ns, 'g');
  var startMark = document.createElementNS(ns, 'polygon');
  var half = mag/2;
  var third = mag/3;
  var quarter = mag/4;
  startMark.setAttribute(
    'points',
      -half+','+third+' '+(-half)+','+(-quarter)+' 0,'+(-half)+' '+
      half+','+(-quarter)+' '+half+','+third);
  startMark.style.fill = startFill;
  startMark.style.stroke = startStroke;
  startMark.style['stroke-width'] = startStrokeWidth;
  startMark.name = what;
  startMark.onmousedown = selectStartMark;
  startMark.onmouseover = function(evt) { evt.target.style.fill = focusedStartFill; };
  startMark.onmouseout = function(evt) { evt.target.style.fill = startFill; };
  g.appendChild(startMark);
  var label = document.createElementNS(ns, 'text');
  label.textContent = name;
  label.setAttribute('font-size', 0.8*mag);
  label.setAttribute('text-anchor', 'middle');
  label.setAttribute('alignment-baseline', 'middle');
  label.setAttribute('pointer-events', 'none');
  g.appendChild(label);
  g.setAttribute('transform', 'translate('+gridX(x)+','+gridY(0)+')');
  startMarks.appendChild(g);
}

function buildCourse() {
  svg = document.getElementById('course');
  var svgWidth = svg.width.baseVal.value;
  if (!svgWidth) svgWidth = 1024;
  mag = Math.min(svgWidth/(course.width+2), maxMag);
  drawCourse();
}

function drawCourse() {
  while (svg.firstChild) svg.removeChild(svg.firstChild);
  vertexRadius = Math.min(vertexMaxRadius, mag/2) - 1;
  gridDotRadius = mag*gridDotRadiusRatio;
  svg.setAttribute('height', mag*(ylimit+1));
  ns = svg.namespaceURI;
  var bg = document.createElementNS(ns, 'rect');
  bg.setAttribute('x', 0);
  bg.setAttribute('y', mag*(course.vision-0.5));
  bg.setAttribute('width', mag*course.width);
  bg.setAttribute('height', mag*(course.length+1.5));
  // bg.setAttribute('height', svg.getAttribute('height'));
  bg.style.fill = backgroundColor;
  svg.appendChild(bg);
  var ag = document.createElementNS(ns, 'rect');
  ag.setAttribute('x', 0);
  ag.setAttribute('y', mag*(-0.5));
  ag.setAttribute('width', mag*course.width);
  ag.setAttribute('height', mag*(course.vision));
  ag.style.fill = afterGoalColor;
  svg.appendChild(ag);
  courseElements = document.createElementNS(ns, 'g');
  var invisCircles = document.createElementNS(ns, 'g');
  for (var y = 0; y != ylimit+1; y++) {
    for (var x = 0; x != course.width; x++) {
      var invis = document.createElementNS(ns, 'circle');
      invis.setAttribute('cx', obstX(x));
      invis.setAttribute('cy', obstY(y));
      invis.setAttribute('r', vertexRadius);
      invis.point = { x: x, y: y };
      invis.style.fill = focusedVertexColor;
      invis.style['fill-opacity'] = '0';
      invis.style.stroke = vertexStroke;
      invis.style['stroke-width'] = vertexStrokeWidth;
      invis.style['stroke-opacity'] = '0';
      invis.onclick = addObstacle;
      invis.onmouseover = makeVisible;
      invis.onmouseout = makeInvisible;
      invisCircles.appendChild(invis);
    }
  }
  courseElements.appendChild(invisCircles);
  svg.appendChild(courseElements);
  obstArea = document.createElementNS(ns, 'g');
  courseElements.appendChild(obstArea);
  reviseCourse();
}

function covers(p, x, y) {
  var r = false;
  var i;
  for (i=0; i!=p.length; i++) {
    var j = (i+1)%p.length;
    var x1 = p[i].x, x2 = p[j].x, y1 = p[i].y, y2 = p[j].y;
    if (x == x1 && y == y1) return true; // on a vertex
    if (y1 == y2 && y == y1 &&
	(x1 <= x && x < x2 || x2 <= x && x < x1))
      return true;		// on a horizontal edge
    if (y1 <= y && y < y2 || y2 <= y && y < y1) {
      var v1 = (y-y1)*(x2-x1);
      var v2 = (y2-y1)*(x-x1);
      if (v1 == v2) return true; // on an edge
      var cmp = v1 < v2;
      if (y2 < y1) cmp = !cmp;
      r ^= cmp;
    }
  }
  return r;
}

var obstacled;
var afterGoal;

function placeObstacle(obst) {
  const BIG = 1e99;
  var minx = BIG, maxx = -BIG, miny = BIG, maxy = -BIG;
  obst.forEach(function (p) {
    minx = Math.min(p.x, minx); maxx = Math.max(p.x, maxx);
    miny = Math.min(p.y, miny); maxy = Math.max(p.y, maxy);
  });
  for (var y = miny; y <= maxy+course.vision; y++) {
    for (var x = minx; x <= maxx; x++) {
      if (covers(obst, x, y)) obstacled[x][y] = true;
    }
  }
}

function drawObstSqr(x, y) {
  var sqr = document.createElementNS(ns, 'rect');
  sqr.setAttribute('x', gridX(x-1));
  sqr.setAttribute('y', gridY(y+1));
  sqr.setAttribute('width', mag);
  sqr.setAttribute('height', mag);
  sqr.style.fill = obstColor;
  sqr.style.opacity = obstOpacity;
  sqr.style.stroke = 'none';
  obstArea.appendChild(sqr);
}

function drawObstTrgl(x1, y1, x2, y2, x3, y3) {
  var trgl = document.createElementNS(ns, 'polygon');
  trgl.setAttribute(
    'points',
    gridX(x1)+','+gridY(y1)+' '+
      gridX(x2)+','+gridY(y2)+' '+gridX(x3)+','+gridY(y3));
  trgl.style.fill = obstColor;
  trgl.style.opacity = obstOpacity;
  trgl.style.stroke = 'none';
  obstArea.appendChild(trgl);
}

function drawObstLine(x1, y1, x2, y2) {
  var line = document.createElementNS(ns, 'line');
  line.setAttribute('x1', obstX(x1));
  line.setAttribute('y1', obstY(y1));
  line.setAttribute('x2', obstX(x2));
  line.setAttribute('y2', obstY(y2));
  line.style.stroke = obstColor;
  line.style.opacity = obstOpacity;
  line.style['stroke-width'] = obstLineWidth;
  obstArea.appendChild(line);
}

function makeVisible(evt) {
  if (courseBeingEdited && !startMarkMoving) {
    evt.target.style['fill-opacity'] = '1';
    evt.target.style['stroke-opacity'] = '1';
  }
}

function makeInvisible(evt) {
  evt.target.style['fill-opacity'] = '0';
  evt.target.style['stroke-opacity'] = '0';
}

function reviseCourse() {
  while (obstArea.firstChild) obstArea.removeChild(obstArea.firstChild);
  obstacled = [];
  for (var x = 0; x != course.width; x++) {
    obstacled[x] = [];
    for (var y = 0; y != ylimit; y++) {
      obstacled[x][y] = false;
    }
    obstacled[x][-1] = true;
    obstacled[x][ylimit+1] = true;
  }
  course.obstacles.forEach(function (o) { placeObstacle(o); });
  for (var y = 0; y != ylimit; y++) {
    for (var x = 1; x != course.width; x++) {
      var nw = (obstacled[x-1][y+1]);
      var sw = (obstacled[x-1][y]);
      var ne = obstacled[x][y+1];
      var se = obstacled[x][y];
      if (nw && sw && ne && se) {
	drawObstSqr(x, y);
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
  for (var y = 0; y != ylimit+1; y++) {
    for (var x = 0; x != course.width; x++) {
      var dot = document.createElementNS(ns, 'circle');
      dot.setAttribute('cx', gridX(x));
      dot.setAttribute('cy', gridY(y));
      dot.setAttribute('r', gridDotRadius);
      dot.style.fill = obstacled[x][y] ? obstDotColor : gridDotColor;
      dot.style.stroke = 'none';
      obstArea.appendChild(dot);
    }
  }
  if (polygons) {
    while (polygons.firstChild) polygons.removeChild(polygons.firstChild);
  } else {
    polygons = document.createElementNS(ns, 'g');
  }
  course.obstacles.forEach(function (o) {
    drawObstaclePolygon(o);
  });
  if (!courseBeingEdited) polygons.style.display = 'none';
  courseElements.appendChild(polygons);
  if (startMarks && startMarks.parentNode)
    startMarks.parentNode.removeChild(startMarks);
  startMarks = document.createElementNS(ns, 'g');
  addStart('x0', '0');
  addStart('x1', '1');
  courseElements.appendChild(startMarks);
}
