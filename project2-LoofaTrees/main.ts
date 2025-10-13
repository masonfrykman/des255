import * as THREE from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { GLTFExporter } from 'three/examples/jsm/Addons.js';
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls.js';

const w = window.innerWidth;
const h = 600;

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(95,w/h,0.1,1000);

camera.position.x = 4;
camera.position.z = 4;
camera.position.y = 1;

const renderer = new THREE.WebGLRenderer({antialias: true});
renderer.setSize(w,h);
document.getElementById("tree-view")?.appendChild(renderer.domElement);

const controls = new OrbitControls(camera, renderer.domElement);
controls.enableDamping = true;
controls.enableZoom = true;
controls.enablePan = false;

controls.minDistance = 5;
controls.maxDistance = 8;

const hemiLight = new THREE.HemisphereLight(0xffffff,0x444444,3);

const lightbulb = new THREE.PointLight(0xff00000, 1, 0);
lightbulb.position.set(23, 23, 23);

const gltfLoader = new GLTFLoader();
const gltf = await gltfLoader.loadAsync('assets/loofa.glb')
const model = gltf.scene;

console.log(scene);

var frameCounter = 0;
const tickEvery = 2;

export var origins: THREE.Vector3[] = [new THREE.Vector3(1, 1, 0)]

const tree: THREE.Group = new THREE.Group();

function makeTree(origin: THREE.Vector3, length: number, iter: number, angleInc: number, zAngleDeg: number) {
    if(angleInc < 0 || zAngleDeg < 0) return

    //console.log("makeTree: " + origin + ", " + lastUsedAngle + ", " + length + ", " + iter);
    if(iter >= 8) {
        // draw the loofa at the end of the branch
        var cap = new THREE.Object3D();
        cap.copy(model);
        model.scale.set(5, 5, 5)

        cap.position.set(origin.x, origin.y, origin.z);
        tree.add(cap);
        return
    }

    // make two new origins, one at a 75 degree angle and one at a 105 angle from the X axis.

    // Solving a right triangle here ugh

    // calculation of (x, y) plane
    var a, b, c;
    var angleA = angleInc; // will be radians DO NOT PASS THIS AS A PARAMETER THAT SHOULD BE DEGREES
    while(angleA >= 360) { // handle coterminals
        angleA -= 360;
    }

    angleA *= Math.PI;
    angleA /= 180;
    c = length;
    a = c * Math.sin(angleA);
    b = Math.sqrt(Math.pow(c, 2) - Math.pow(a, 2));

    var x = origin.x;
    var y = origin.y;
    var z = origin.z;
    

    // the b side becomes the extension of the origin x coordinate
    // the a side becomes the extension of the origin y coordinate.
    // a, b, and c will ALWAYS be positive, the angle is going to determine the negativity of the extensions.
    
    

    var xExtensionFactor = b;
    var yExtensionFactor = a;

    var angleDeg = angleA * 180;
    angleDeg /= Math.PI;
    if(angleDeg == 0) {
        x += length;
    } else if(angleDeg > 0 && angleDeg < 90) { // I
        x += xExtensionFactor;
        y += yExtensionFactor;
    } else if(angleDeg == 90) {
        y += length;
    } else if(angleDeg > 90 && angleDeg < 180) { // II
        x -= xExtensionFactor;
        y += yExtensionFactor;
    } else if(angleDeg == 180) {
        x -= length;
    } else if(angleDeg > 180 && angleDeg < 270) { // III
        x -= xExtensionFactor;
        y += yExtensionFactor; // note: i had to flip the -= to +=, idk why but it works
    } else if(angleDeg == 270) {
        y -= length;
    } else { // IV
        y += yExtensionFactor;
        x += xExtensionFactor; // note: i had to flip the -= to +=, idk why but it works
    }

    // Draw a line from the old origin to the new origin

    var zA, zB, zC;
    var zAngle1T = zAngleDeg;
    while(zAngle1T >= 360) {
        zAngle1T -= 360;
    }
    
    var zAngleRad = (zAngle1T * Math.PI) / 180;
    zC = length;
    zA = zC * Math.sin(zAngleRad);
    zB = Math.sqrt(Math.pow(zC, 2) - Math.pow(zA, 2));
    
    if(zAngleRad == 0) {
        //x += length;
    } else if(zAngleRad > 0 && zAngleRad < 90) {
        //x += zB;
        z += zA;
    } else if(zAngleRad == 90) {
        z += length;
    } else if(zAngleRad > 90 && zAngleRad < 180) {
        //x -= zB;
        z += zA;
    } else if(zAngleRad == 180) {
        //x -= length;
    } else if(zAngleRad > 180 && zAngleRad < 270) {
        //x -= zA;
        z -= zB;
    } else if(zAngleRad == 270) {
        z -= length;
    } else {
        //x += zB;
        z -= zA;
    }

    var vec = new THREE.Vector3(x, y, z);

    var geo = new THREE.BufferGeometry().setFromPoints([origin, vec]);
    var line = new THREE.Line(geo, new THREE.LineBasicMaterial({color: new THREE.Color().setRGB(Math.random(), Math.random(), Math.random())}));
    tree.add(line);

    // Recursively make new origins.
    //makeTree(vec, length, iter + 1, angleInc * 2, zAngleDeg + 5);
    makeTree(vec, length / 1.09, iter + 1, Math.random() * 180, Math.random() * 360);
    makeTree(vec, length / 1.09, iter + 1, Math.random() * 180, Math.random() * 360);
    //makeTree(vec, length, iter + 1, angleInc * 2, Math.random() * 360);
    //makeTree(vec, length, iter + 1, angleInc + 4, zAngleDeg * 2);
}

var angleInc = 90;
var zAngleInc = 0;

function ntick() {
    tree.clear();
    var beginningOrigin = new THREE.Vector3(0, -2, 0);
    makeTree(beginningOrigin, 1, 2, angleInc, zAngleInc);
}

const coordPlane: THREE.Group = new THREE.Group()

function showCoordPlane() {
    var x = new THREE.BufferGeometry().setFromPoints([new THREE.Vector3(0, 0, 0), new THREE.Vector3(200, 0, 0)]);
    var y = new THREE.BufferGeometry().setFromPoints([new THREE.Vector3(0, 0, 0), new THREE.Vector3(0, 200, 0)]);
    var z = new THREE.BufferGeometry().setFromPoints([new THREE.Vector3(0, 0, 0), new THREE.Vector3(0, 0, 200)]);

    coordPlane.add(new THREE.Line(x, new THREE.LineBasicMaterial({color: new THREE.Color().setRGB(1, 0, 0)})))
    coordPlane.add(new THREE.Line(y, new THREE.LineBasicMaterial({color: new THREE.Color().setRGB(0, 1, 0)})))
    coordPlane.add(new THREE.Line(z, new THREE.LineBasicMaterial({color: new THREE.Color().setRGB(0, 0, 1)})))
}

function hideCoordPlane() {
    coordPlane.clear()
}

async function saveTreeAsGLTF() {
    var gExp = new GLTFExporter()
    var exportData = await gExp.parseAsync(tree, { binary: true })
    var exportBlob = new Blob([exportData as ArrayBuffer]);
    var downloadElem = document.createElement("a");
    downloadElem.href = window.URL.createObjectURL(exportBlob)
    downloadElem.download = "tree.glb"
    document.body.appendChild(downloadElem);
    downloadElem.click()
    window.URL.revokeObjectURL(downloadElem.href)
    downloadElem.remove()
}

var paused = true;
var showPlaneLines = true;

function animate(){
    controls.update();
    renderer.render(scene,camera);
    frameCounter++;
}

window.addEventListener("keydown", async (ev) => {
    if(ev.code == "KeyC") {
        scene.children = [];
    } else if(ev.code == "KeyP") {
        paused = !paused;
        console.log(paused ? "PAUSED" : "UNPAUSED")
    } else if(ev.code == "KeyL") {
        if(showPlaneLines) {
            hideCoordPlane();
            showPlaneLines = false;
            return
        }
        showCoordPlane();
        showPlaneLines = true;
    } else if(ev.code == "KeyN") {
        ntick();
    } else if(ev.code == "KeyS") {
        await saveTreeAsGLTF()
    }
})

window.addEventListener("resize", (ev) => {
    renderer.setSize(window.innerWidth, 600, true)
    camera.aspect = window.innerWidth / 600
})

// Constructs the main elements of the scene
ntick();
scene.add(hemiLight)
scene.add(coordPlane)
showCoordPlane();
scene.add(tree)

renderer.setAnimationLoop(animate);