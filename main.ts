import * as THREE from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls.js';

const w = window.innerWidth;
const h = window.innerHeight;
const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75,w/h,0.1,500);
camera.position.z = 3;

const renderer = new THREE.WebGLRenderer({antialias: true});
renderer.setSize(w,h);
document.body.appendChild(renderer.domElement);

const controls = new OrbitControls(camera, renderer.domElement);
controls.enableDamping = true;

const hemiLight = new THREE.HemisphereLight(0xffffff,0x444444,3);
scene.add(hemiLight);

const gltfLoader = new GLTFLoader();
const gltf = await gltfLoader.loadAsync('assets/loofa.glb')
const model = gltf.scene;
// scene.add(model);

console.log(scene);

var frameCounter = 0;
const tickEvery = 1;

const lineMaterial = new THREE.LineBasicMaterial({color: new THREE.Color().setRGB(0.5, 0.5, 0.5)})
var lastLineVector: THREE.Vector3 = new THREE.Vector3(1, 1, 1);

function tick() {
    var newLineVector = new THREE.Vector3(camera.position.x, camera.position.y, camera.position.z);

    var buffGeo = new THREE.BufferGeometry().setFromPoints([lastLineVector, newLineVector]);
    
    var line = new THREE.Line(buffGeo, lineMaterial)
    
    lineMaterial.color.b += 0.005;
    lineMaterial.color.r += 0.001;
    lineMaterial.color.g += 0.002;

    if(lineMaterial.color.b > 1) lineMaterial.color.b = 0;
    if(lineMaterial.color.r > 1) lineMaterial.color.r = 0;
    if(lineMaterial.color.g > 1) lineMaterial.color.g = 0;

    scene.add(line)

    lastLineVector = newLineVector;
}

function animate(){
    controls.update();

    if(frameCounter % tickEvery == 0) {
        tick();
    }

    renderer.render(scene,camera);
    frameCounter++;
}

window.addEventListener("keydown", (ev) => {
    if(ev.code == "KeyC") {
        scene.children = [];
    }
})

renderer.setAnimationLoop(animate);