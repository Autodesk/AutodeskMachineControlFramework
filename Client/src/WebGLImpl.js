/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
 * Neither the name of the Autodesk Inc. nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

import * as THREE from 'three';
import {
    SVGLoader
}
from "three/examples/jsm/loaders/SVGLoader";

const RAYCAST_LINE_THRESHOLD = 3;

class WebGLElement {

    constructor() {
        this.glelement = null;
        this.position = {
            x: 0.0,
            y: 0.0,
            z: 0.0
        }
        this.scale = {
            x: 1.0,
            y: 1.0,
            z: 1.0
        }
    }

    setPosition(x, y, z) {

        this.position.x = x;
        this.position.y = y;
        this.position.z = z;

        if (this.glelement) {
            this.glelement.position.x = x;
            this.glelement.position.y = y;
            this.glelement.position.z = z;
        }
    }

    setScale(x, y, z) {
        this.scale.x = x;
        this.scale.y = y;
        this.scale.z = z;

        if (this.glelement) {
            this.glelement.scale.x = x;
            this.glelement.scale.y = y;
            this.glelement.scale.z = z;
        }
    }

}

class WebGLLinesElement extends WebGLElement {

    constructor(lineData, zValue, lineThickness, lineColor) {
        super();

        var geometry = new THREE.BufferGeometry();
        const material = new THREE.MeshBasicMaterial({
            vertexColors: false,
            color: lineColor
        });

        const positions = [];

        const lineCount = lineData.length / 4;

        for (let i = 0; i < lineCount; i++) {

            const x1 = lineData[i * 4];
            const y1 = lineData[i * 4 + 1];
            const x2 = lineData[i * 4 + 2];
            const y2 = lineData[i * 4 + 3];

            const dx = x2 - x1;
            const dy = y2 - y1;
            const dlen = Math.sqrt(dx * dx + dy * dy);

            if (dlen > 0) {
                const ux =  - dy / dlen * lineThickness * 0.5;
                const uy = dx / dlen * lineThickness * 0.5;

                positions.push(x1 + ux, y1 + uy, zValue);
                positions.push(x2 + ux, y2 + uy, zValue);
                positions.push(x2 - ux, y2 - uy, zValue);
                positions.push(x2 - ux, y2 - uy, zValue);
                positions.push(x1 - ux, y1 - uy, zValue);
                positions.push(x1 + ux, y1 + uy, zValue);
            }
        }

        geometry.setAttribute('position', new THREE.Float32BufferAttribute(positions, 3));
        geometry.computeBoundingSphere();

        this.glelement = new THREE.Mesh(geometry, material);

    }

}

class WebGLGridElement extends WebGLElement {

    constructor(width, height, zValue, factor) {
        super();

        this.lineElement1 = null;
        this.lineElement2 = null;
        this.lineElement3 = null;

        var group = new THREE.Group()

            var gridArray1 = []
            var gridArray2 = []
            var gridArray3 = []
            var x,
        y;

        var lineCountX = width / factor + 1;
        var lineCountY = height / factor + 1;
		
		var gridRecursion = 5;
		var gridRecursionSquared = gridRecursion * gridRecursion;

        for (x = 0; x < lineCountX; x++) {
            if (x % gridRecursion != 0) {
                gridArray1.push(x * factor, 0, x * factor, height);
            }
        }

        for (y = 0; y < lineCountY; y++) {
            if (y % gridRecursion != 0) {
                gridArray1.push(0, y * factor, width, y * factor);
            }
        }

        for (x = 0; x < (lineCountX / gridRecursion); x++) {
            if (x % gridRecursion != 0) {
                gridArray2.push(x * gridRecursion * factor, 0, x * gridRecursion * factor, height);
            }
        }

        for (y = 0; y < (lineCountY / gridRecursion); y++) {
            if (y % gridRecursion != 0) {
                gridArray2.push(0, y * gridRecursion * factor, width, y * gridRecursion * factor);
            }
        }

        for (x = 0; x < (lineCountX / gridRecursionSquared); x++) {
            gridArray3.push(x * gridRecursionSquared * factor, 0, x * gridRecursionSquared * factor, height);
        }

        for (y = 0; y < (lineCountY / gridRecursionSquared); y++) {
            gridArray3.push(0, y * gridRecursionSquared * factor, width, y * gridRecursionSquared * factor);
        }

        this.lineElement1 = new WebGLLinesElement(gridArray1, zValue, 0.5, 0xe0e0e0);
        this.lineElement2 = new WebGLLinesElement(gridArray2, zValue, 0.6, 0xc8c8c8);
        this.lineElement3 = new WebGLLinesElement(gridArray3, zValue, 0.8, 0xc0c0c0);

        group.add(this.lineElement1.glelement);
        group.add(this.lineElement2.glelement);
        group.add(this.lineElement3.glelement);

        this.glelement = group;

    }

}

class WebGLSVGElement extends WebGLElement {

    constructor(data, zValue, fillShapes, showStrokes) {
        super();

        const paths = data.paths;

        const group = new THREE.Group();
        group.position.x = 0;
        group.position.y = 0;
        group.position.z = zValue;
        group.scale.y *=  - 1;

        for (let i = 0; i < paths.length; i++) {

            const path = paths[i];

            const fillColor = path.userData.style.fill;
            if (fillShapes && fillColor !== undefined && fillColor !== 'none') {

                const material = new THREE.MeshBasicMaterial({
                    color: new THREE.Color().setStyle(fillColor),
                    opacity: path.userData.style.fillOpacity,
                    transparent: true,
                    side: THREE.DoubleSide,
                    depthWrite: false,
                    wireframe: false
                });

                const shapes = SVGLoader.createShapes(path);

                for (let j = 0; j < shapes.length; j++) {

                    const shape = shapes[j];

                    const geometry = new THREE.ShapeGeometry(shape);
                    const mesh = new THREE.Mesh(geometry, material);

                    group.add(mesh);

                }

            }

            const strokeColor = path.userData.style.stroke;

            if (showStrokes && strokeColor !== undefined && strokeColor !== 'none') {

                const material = new THREE.MeshBasicMaterial({
                    color: new THREE.Color().setStyle(strokeColor),
                    opacity: path.userData.style.strokeOpacity,
                    transparent: true,
                    side: THREE.DoubleSide,
                    depthWrite: false,
                    wireframe: false
                });

                for (let j = 0, jl = path.subPaths.length; j < jl; j++) {

                    const subPath = path.subPaths[j];

                    const geometry = SVGLoader.pointsToStroke(subPath.getPoints(), path.userData.style);

                    if (geometry) {

                        const mesh = new THREE.Mesh(geometry, material);

                        group.add(mesh);

                    }

                }

            }

        }

        this.glelement = group;
    }

}

class WebGLImpl {

    constructor() {
        this.renderer = new THREE.WebGLRenderer({
            antialias: true
        });
        this.scene = new THREE.Scene();
        this.scene.background = new THREE.Color(0xffffff);
		
		this.raycaster = new THREE.Raycaster();
		this.raycaster.params.Line.threshold = RAYCAST_LINE_THRESHOLD;

        this.renderElements = new Map();
    }

    setup2DView(sizex, sizey, near, far) {
        this.camera = new THREE.OrthographicCamera(0, sizex, 0, sizey, near, far);
        this.camera.position.z = far * 0.99;
    }

    setupOrthoView(sizex, sizey, near, far) {
        this.camera = new THREE.OrthographicCamera(-sizex * 0.5, sizex * 0.5, -sizey * 0.5, sizey * 0.5, near, far);
        this.camera.position.z = far * 0.99;
    }

    setupDOMElement(domelement) {
        domelement.append(this.renderer.domElement);
    }

    resizeTo(sizex, sizey) {
        if (this.renderer) {
            this.renderer.setSize(sizex, sizey);
        }
    }

    renderScene() {

        if (this.scene && this.camera) {
            this.renderer.render(this.scene, this.camera);
        }

    }

    hasElement(identifier) {
        if (!identifier)
            return false;

        return (this.renderElements.has(identifier));
    }

    removeElement(identifier) {
        if (!identifier)
            return false;

        if (this.renderElements.has(identifier)) {
            this.scene.remove(identifier);
            this.renderElements.delete(identifier);
        }

    }

    findElement(identifier) {
        if (!identifier)
            return null;

        if (!this.renderElements.has(identifier))
            return null;

        return this.renderElements.get(identifier);
    }

    addElement(identifier, renderElement) {
        if (!identifier)
            return;
        if (!renderElement)
            return;

        if (this.hasElement(identifier))
            this.removeElement(identifier);

        this.renderElements.set(identifier, renderElement);

        if (this.scene) {
            if (renderElement.glelement) {
                renderElement.glelement.name = identifier;
                this.scene.add(renderElement.glelement);
            }
        }
    }

    add2DGridGeometry(identifier, width, height, zValue, factor) {

        if (!identifier)
            return;

        var gridElement = new WebGLGridElement(width, height, zValue, factor);

        this.addElement(identifier, gridElement);

        return gridElement;
    }

    add2DLineGeometry(identifier, lineData, zValue, lineThickness, lineColor) {
        if (!identifier)
            return;

        var linesElement = new WebGLLinesElement(lineData, zValue, lineThickness, lineColor);

        this.addElement(identifier, linesElement);

        return linesElement;
    }

    addSVGImage(identifier, url, zValue, fillShapes, showStrokes) {
        if (!identifier)
            return;

        var renderElements = this.renderElements;
        var scene = this.scene;

        const loader = new SVGLoader();
        loader.load(url, function (data) {
            var svgElement = new WebGLSVGElement(data, zValue, fillShapes, showStrokes);

            if (renderElements.has(identifier)) {
                scene.remove(identifier);
                renderElements.delete(identifier);
            }

            renderElements.set(identifier, svgElement);

            if (svgElement.glelement) {
                svgElement.glelement.name = identifier;
                scene.add(svgElement.glelement);
            }

        });

    }
	
	
	pick2DElement (x, y)
	{		
		if (this.camera) {
			
			console.log("pick X: " + x + " y: " + y);
			
			const pointer = new THREE.Vector2();
			pointer.x = x;
			pointer.y = y;
			
			this.raycaster.setFromCamera( pointer, this.camera );
			
			const intersects = this.raycaster.intersectObjects( this.scene.children[1], true );
			if (intersects.length > 0) {
				alert ("intersect");
			}
		}
	}

    setupDemoScene() {
        this.camera = new THREE.PerspectiveCamera(27, window.innerWidth / window.innerHeight, 1, 3500);
        this.camera.position.z = 2750;

        //this.scene.background = new THREE.Color( 0x050505 );
        this.scene.fog = new THREE.Fog(0x050505, 2000, 3500);

        this.scene.add(new THREE.AmbientLight(0x444444));

        const light1 = new THREE.DirectionalLight(0xffffff, 0.5);
        light1.position.set(1, 1, 1);
        this.scene.add(light1);

        const light2 = new THREE.DirectionalLight(0xffffff, 1.5);
        light2.position.set(0,  - 1, 0);
        this.scene.add(light2);

        //

        const triangles = 5000;

        let geometry = new THREE.BufferGeometry();

        const positions = new Float32Array(triangles * 3 * 3);
        const normals = new Float32Array(triangles * 3 * 3);
        const colors = new Float32Array(triangles * 3 * 3);

        const color = new THREE.Color();

        const n = 800,
        n2 = n / 2; // triangles spread in the cube
        const d = 120,
        d2 = d / 2; // individual triangle size

        const pA = new THREE.Vector3();
        const pB = new THREE.Vector3();
        const pC = new THREE.Vector3();

        const cb = new THREE.Vector3();
        const ab = new THREE.Vector3();

        for (let i = 0; i < positions.length; i += 9) {

            // positions

            const x = Math.random() * n - n2;
            const y = Math.random() * n - n2;
            const z = (Math.random() * n - n2) * 0.1;

            const ax = x + Math.random() * d - d2;
            const ay = y + Math.random() * d - d2;
            const az = z + Math.random() * d - d2;

            const bx = x + Math.random() * d - d2;
            const by = y + Math.random() * d - d2;
            const bz = z + Math.random() * d - d2;

            const cx = x + Math.random() * d - d2;
            const cy = y + Math.random() * d - d2;
            const cz = z + Math.random() * d - d2;

            positions[i] = ax;
            positions[i + 1] = ay;
            positions[i + 2] = az;

            positions[i + 3] = bx;
            positions[i + 4] = by;
            positions[i + 5] = bz;

            positions[i + 6] = cx;
            positions[i + 7] = cy;
            positions[i + 8] = cz;

            // flat face normals

            pA.set(ax, ay, az);
            pB.set(bx, by, bz);
            pC.set(cx, cy, cz);

            cb.subVectors(pC, pB);
            ab.subVectors(pA, pB);
            cb.cross(ab);

            cb.normalize();

            const nx = cb.x;
            const ny = cb.y;
            const nz = cb.z;

            normals[i] = nx;
            normals[i + 1] = ny;
            normals[i + 2] = nz;

            normals[i + 3] = nx;
            normals[i + 4] = ny;
            normals[i + 5] = nz;

            normals[i + 6] = nx;
            normals[i + 7] = ny;
            normals[i + 8] = nz;

            // colors

            const vx = (x / n) + 0.5;
            const vy = (y / n) + 0.5;
            const vz = (z / n) + 0.5;

            color.setRGB(vx, vy, vz);

            colors[i] = color.r;
            colors[i + 1] = color.g;
            colors[i + 2] = color.b;

            colors[i + 3] = color.r;
            colors[i + 4] = color.g;
            colors[i + 5] = color.b;

            colors[i + 6] = color.r;
            colors[i + 7] = color.g;
            colors[i + 8] = color.b;

        }

        geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
        geometry.setAttribute('normal', new THREE.BufferAttribute(normals, 3));
        geometry.setAttribute('color', new THREE.BufferAttribute(colors, 3));

        geometry.computeBoundingSphere();

        let material = new THREE.MeshPhongMaterial({
            color: 0xaaaaaa,
            specular: 0xffffff,
            shininess: 250,
            side: THREE.DoubleSide,
            vertexColors: true
        });

        var mesh = new THREE.Mesh(geometry, material);
        this.scene.add(mesh);

        this.renderScene();

        return mesh;
    }

}

export default WebGLImpl;
