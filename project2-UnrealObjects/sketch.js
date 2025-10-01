let m, tex;

function preload() {
  m = loadModel('asset/luffa.obj');
  tex = loadImage('asset/luffa.jpg');
}

function setup() {
  createCanvas(windowWidth, windowHeight, WEBGL);
}

function draw() {
  background('blue');
  orbitControl();

  noStroke()

  texture(tex);

  translate(-150, 0, 0);
  model(m);

  translate(360, 0, 0);
  model(m);

  translate(0, -360, 0);
  model(m);

  for(var i = 2; i < 100; i++) {
    translate(i * 50, 0, 0);
    model(m);
  }
}