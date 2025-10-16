import * as THREE from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { GLTFExporter } from 'three/examples/jsm/Addons.js';
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls.js';
import { generateTree, tree } from './tree.ts'

// MARK: Scene setup
const scene = new THREE.Scene();

const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.5, 1000);

const renderer = new THREE.WebGLRenderer({antialias: true, preserveDrawingBuffer: true, alpha: true});
renderer.setSize(window.innerWidth, window.innerHeight);
renderer.shadowMap.enabled = true
renderer.shadowMap.type = THREE.PCFSoftShadowMap
document.getElementById("tree-view")?.appendChild(renderer.domElement);

const controls = new OrbitControls(camera, renderer.domElement);
controls.enableDamping = true;
controls.enableZoom = true;
controls.enablePan = false;
controls.minDistance = 5;
controls.maxDistance = 8;
controls.maxPolarAngle = Math.PI / 2 + 0.225;

function updateCameraPos() {
    camera.position.x = 0;
    camera.position.z = 20 - (20 / window.innerWidth);
    camera.position.y = 4;
    controls.target = new THREE.Vector3(0, camera.position.y / 2, 0)
}
updateCameraPos()

const skyColor = 0x37a9fc
const groundColor = 0xaaffaa
scene.background = new THREE.Color(skyColor)

const dLight = new THREE.DirectionalLight(skyColor, 1)
dLight.castShadow = true
dLight.shadow.mapSize = new THREE.Vector2(1024, 1024)

const gltfLoader = new GLTFLoader();
const gltf = await gltfLoader.loadAsync('assets/loofa.glb')
const model = gltf.scene;

const ground = new THREE.BoxGeometry(300, 0, 300)
const groundMtr = new THREE.MeshLambertMaterial({ color: groundColor })
const groundMesh = new THREE.Mesh(ground, groundMtr)
scene.add(groundMesh)

// MARK: Export funs

function saveBlob(blob: Blob, name: string) {
    var downloadElem = document.createElement("a");
    downloadElem.href = window.URL.createObjectURL(blob)
    downloadElem.download = name
    document.body.appendChild(downloadElem);
    downloadElem.click()
    window.URL.revokeObjectURL(downloadElem.href)
    downloadElem.remove()
}

async function saveTreeAsGLTF() {
    var gExp = new GLTFExporter()
    var exportData = await gExp.parseAsync(tree, { binary: true })
    var exportBlob = new Blob([exportData as ArrayBuffer])
    saveBlob(exportBlob, "tree.glb")
}

function saveScreenshot() {
    const canvas = renderer.domElement
    canvas.toBlob((blob) => blob != null ? saveBlob(blob, "tree.png") : null);
}

// MARK: Lifecycle fns

var showPlaneLines = false;

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

function animate(){
    controls.update();
    dLight.shadow.camera.near = camera.near;
    dLight.shadow.camera.far = camera.far;
    dLight.shadow.camera.updateProjectionMatrix()
    
    renderer.render(scene,camera);
}

function init() {
    generateTree(model)
    scene.add(dLight)
    scene.add(coordPlane)
    if(showPlaneLines) showCoordPlane();
    scene.add(tree)

    ground.computeVertexNormals()

    renderer.setAnimationLoop(animate); // -> animate()
}

// MARK: Event Listeners

window.addEventListener("resize", (ev) => {
    renderer.setSize(window.innerWidth, window.innerHeight, true)
    updateCameraPos()
    camera.aspect = window.innerWidth / window.innerHeight
    camera.updateProjectionMatrix() // https://stackoverflow.com/questions/20290402/three-js-resizing-canvas
})


init()

// MARK: During drawing

if(document.getElementById("btn-export") != null) {
    document.getElementById("btn-export")!.addEventListener("click", async (ev) => await saveTreeAsGLTF());
    document.getElementById("btn-export")!.classList.add("hover-enabled")
}

if(document.getElementById("btn-screenshot") != null) {
    document.getElementById("btn-screenshot")!.addEventListener("click", (ev) => saveScreenshot())
    document.getElementById("btn-screenshot")!.classList.add("hover-enabled")
}

if(document.getElementById("btn-new") != null) {
    document.getElementById("btn-new")!.addEventListener("click", (ev) => generateTree(model))
    document.getElementById("btn-new")!.classList.add("hover-enabled")
}

// Publish debug functions
window.debug = new class {
    togglePlaneLines() {
        if(showPlaneLines) {
            hideCoordPlane();
            showPlaneLines = false;
            return
        }
        showCoordPlane();
        showPlaneLines = true;
    }

    clear() {
        scene.children = [];
    }
}