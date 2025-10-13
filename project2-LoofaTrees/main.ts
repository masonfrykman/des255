import * as THREE from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { GLTFExporter } from 'three/examples/jsm/Addons.js';
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls.js';
import { generateTree, tree} from './tree.ts'

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

const hemiLight = new THREE.HemisphereLight(0x37a9fc,0x444444,3);

const lightbulb = new THREE.PointLight(0xff00000, 1, 0);
lightbulb.position.set(23, 23, 23);

const gltfLoader = new GLTFLoader();
const gltf = await gltfLoader.loadAsync('assets/loofa.glb')
const model = gltf.scene;

console.log(scene);

const ground = new THREE.BoxGeometry(300, 0, 300)
const groundMtr = new THREE.MeshBasicMaterial( { color: 0x50bb50})

const groundMesh = new THREE.Mesh(ground, groundMtr)
groundMesh.translateY(-2)
scene.add(groundMesh)

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

var showPlaneLines = true;

function animate(){
    controls.update();
    renderer.render(scene,camera);
}

function init() {
    generateTree(model)
    scene.add(hemiLight)
    scene.add(coordPlane)
    showCoordPlane();
    scene.add(tree)
    renderer.setClearColor(0x37a9fc, 1)
    renderer.setAnimationLoop(animate); // -> animate()
}

window.addEventListener("keydown", async (ev) => {
    if(ev.code == "KeyC") {
        scene.children = [];
    } else if(ev.code == "KeyL") {
        if(showPlaneLines) {
            hideCoordPlane();
            showPlaneLines = false;
            return
        }
        showCoordPlane();
        showPlaneLines = true;
    } else if(ev.code == "KeyN") {
        generateTree(model)
    } else if(ev.code == "KeyS") {
        await saveTreeAsGLTF()
    }
})

window.addEventListener("resize", (ev) => {
    renderer.setSize(window.innerWidth, 600, true)
    camera.aspect = window.innerWidth / 600
})

init()