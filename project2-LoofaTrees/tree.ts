import * as THREE from 'three';

export const tree: THREE.Group = new THREE.Group();

function makeTree(origin: THREE.Vector3, length: number, iter: number, angleInc: number, zAngleDeg: number, leafModel: THREE.Group) {
    if(angleInc < 0
        || zAngleDeg < 0
        || angleInc > 180
        || zAngleDeg < 0
    ) return

    // MARK: Place leaf
    if(iter >= 8) {
        // draw the loofa at the end of the branch
        var cap = new THREE.Object3D();
        cap.copy(leafModel);
        leafModel.scale.set(5, 5, 5)
        

        cap.position.set(origin.x, origin.y, origin.z);
        tree.add(cap);
        cap.castShadow = true
        cap.receiveShadow = true
        return
    }

    // Solving a right triangle here ugh

    // MARK: Calculation of (x, y) plane
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
    
    // MARK: X-Y movement

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

    // MARK: Z movement

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

    // MARK: Push to scene

    var vec = new THREE.Vector3(x, y, z);
    var geo = new THREE.BufferGeometry().setFromPoints([origin, vec]);
    var line = new THREE.Line(geo, new THREE.LineBasicMaterial({color: new THREE.Color().setRGB(Math.random(), Math.random(), Math.random())}));
    tree.add(line);

    // Recursively make new origins.
    makeTree(vec, length / 1.09, iter + 1, Math.random() * 180, Math.random() * 360, leafModel);
    makeTree(vec, length / 1.09, iter + 1, Math.random() * 180, Math.random() * 360, leafModel);
}

export function generateTree(leaf: THREE.Group) {
    tree.clear();
    var beginningOrigin = new THREE.Vector3(0, 0, 0);
    makeTree(beginningOrigin, 1, 2, 90, 0, leaf);
}