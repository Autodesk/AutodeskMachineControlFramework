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
import * as Assert from "./AMCAsserts.js";

import { SVGLoader } from "three/examples/jsm/loaders/SVGLoader";
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';

//const RAYCAST_LINE_THRESHOLD = 3;

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

    setPositionXY(x, y) {

        this.position.x = x;
        this.position.y = y;

        if (this.glelement) {
            this.glelement.position.x = x;
            this.glelement.position.y = y;
        }
    }

    setScaleXY(x, y) {
        this.scale.x = x;
        this.scale.y = y;

        if (this.glelement) {
            this.glelement.scale.x = x;
            this.glelement.scale.y = y;
        }
    }
	
	updateGLPosition () 
	{
        if (this.glelement) {
            this.glelement.position.x = this.position.x;
            this.glelement.position.y = this.position.y;
            this.glelement.position.z = this.position.z;
            this.glelement.scale.x = this.scale.x;
            this.glelement.scale.y = this.scale.y;
            this.glelement.scale.z = this.scale.z;
        }
	}
}

class WebGLLinesElement extends WebGLElement {

    constructor(lineData, zValue, lineThickness, lineColor, vertexcolors) {
        super();

        let geometry = new THREE.BufferGeometry();
        let material;

		if (vertexcolors) {
			material = new THREE.MeshBasicMaterial({
				vertexColors: true
			});
		} else {
			material = new THREE.MeshBasicMaterial({
				vertexColors: false,
				color: lineColor
			});
		}

        const positions = [];
		const colors = [];

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
				
				if (vertexcolors) {
					
					// Add colors for each vertex
					const color = new THREE.Color(vertexcolors[i]); 
					for (let j = 0; j < 6; j++) { 
						colors.push(color.r, color.g, color.b);
					}
				}
            }
        }

        geometry.setAttribute('position', new THREE.Float32BufferAttribute(positions, 3));
		if (vertexcolors) {
			geometry.setAttribute('color', new THREE.Float32BufferAttribute(colors, 3)); 
		}
        geometry.computeBoundingSphere();

        this.glelement = new THREE.Mesh(geometry, material);
    }
}

class WebGLPointsElement extends WebGLElement {

    constructor(pointsData, zValue, pointsRadius, pointsColor, vertexcolors) {
        super();

        let geometry = new THREE.BufferGeometry();
        let material;

		if (vertexcolors) {
			material = new THREE.MeshBasicMaterial({
				vertexColors: true
			});
		} else {
			material = new THREE.MeshBasicMaterial({
				vertexColors: false,
				color: pointsColor
			});
		}

        let numberOfPoints = pointsData.length / 2;

        const vertices = [];
		const colors = [];

        for (let i = 0; i < numberOfPoints; i++) {
            vertices.push(
                pointsData[i * 2] - pointsRadius / 2, pointsData[i * 2 + 1] - pointsRadius / 2, zValue,
                pointsData[i * 2] - pointsRadius / 2, pointsData[i * 2 + 1] + pointsRadius / 2, zValue,
                pointsData[i * 2] + pointsRadius / 2, pointsData[i * 2 + 1] + pointsRadius / 2, zValue,
                pointsData[i * 2] - pointsRadius / 2, pointsData[i * 2 + 1] - pointsRadius / 2, zValue,
                pointsData[i * 2] + pointsRadius / 2, pointsData[i * 2 + 1] + pointsRadius / 2, zValue,
                pointsData[i * 2] + pointsRadius / 2, pointsData[i * 2 + 1] - pointsRadius / 2, zValue,
            );
			
			if (vertexcolors) {
					
					// Add colors for each vertex
					const color = new THREE.Color(vertexcolors[i]); 
					for (let j = 0; j < 6; j++) { 
						colors.push(color.r, color.g, color.b);
					}
				}
        }

        geometry.setAttribute('position', new THREE.Float32BufferAttribute(vertices, 3));
		if (vertexcolors) {
			geometry.setAttribute('color', new THREE.Float32BufferAttribute(colors, 3)); 
		}
        geometry.computeBoundingSphere();

        this.glelement = new THREE.Mesh(geometry, material);
    }
}

class WebGLBoxElement extends WebGLElement {

    constructor(paramWidth, paramHeight, paramDepth) {
        super();

		let dWidth = Assert.NumberValue (paramWidth);
		let dHeight = Assert.NumberValue (paramHeight);
		let dDepth = Assert.NumberValue (paramDepth);
		
		this.material = new THREE.MeshPhongMaterial( {
			color: 0x808080,
			shininess: 50,
			specular: 0x111111
		} );


        const meshpositions = [];
        meshpositions.push (dWidth, dHeight, 0.0);		
        meshpositions.push (dWidth, 0.0, 0.0);
        meshpositions.push (0.0, 0.0, 0.0);
        meshpositions.push (0.0, 0.0, 0.0);
        meshpositions.push (0.0, dHeight, 0.0);
        meshpositions.push (dWidth, dHeight, 0.0);
		
        meshpositions.push (0.0, 0.0, dDepth);
        meshpositions.push (dWidth, 0.0, dDepth);
        meshpositions.push (dWidth, dHeight, dDepth);		
        meshpositions.push (dWidth, dHeight, dDepth);
        meshpositions.push (0.0, dHeight, dDepth);
        meshpositions.push (0.0, 0.0, dDepth);
		
        meshpositions.push (0.0, 0.0, 0.0);
        meshpositions.push (dWidth, 0.0, 0.0);
        meshpositions.push (dWidth, 0.0, dDepth);		
        meshpositions.push (dWidth, 0.0, dDepth);
        meshpositions.push (0.0, 0.0, dDepth);
        meshpositions.push (0.0, 0.0, 0.0);

        meshpositions.push (dWidth, 0.0, 0.0);
        meshpositions.push (dWidth, dHeight, 0.0);
        meshpositions.push (dWidth, dHeight, dDepth);
        meshpositions.push (dWidth, dHeight, dDepth);
        meshpositions.push (dWidth, 0.0, dDepth);
        meshpositions.push (dWidth, 0.0, 0.0);

        meshpositions.push (dWidth, dHeight, 0.0);
        meshpositions.push (0.0, dHeight, 0.0);
        meshpositions.push (0.0, dHeight, dDepth);
        meshpositions.push (0.0, dHeight, dDepth);
        meshpositions.push (dWidth, dHeight, dDepth);
        meshpositions.push (dWidth, dHeight, 0.0);
		
        meshpositions.push (0.0, dHeight, 0.0);
        meshpositions.push (0.0, 0.0, 0.0);
        meshpositions.push (0.0, 0.0, dDepth);
        meshpositions.push (0.0, 0.0, dDepth);
        meshpositions.push (0.0, dHeight, dDepth);
        meshpositions.push (0.0, dHeight, 0.0);

		
        this.meshgeometry = new THREE.BufferGeometry();
		this.meshgeometry.setAttribute('position', new THREE.Float32BufferAttribute(meshpositions, 3));
		this.meshgeometry.computeVertexNormals ();
		this.meshelement = new THREE.Mesh( this.meshgeometry, this.material );
		this.meshelement.castShadow = true;
		this.meshelement.receiveShadow = true;
		
        let linegeometry = new THREE.BufferGeometry();
        const linematerial = new THREE.LineBasicMaterial({
            linewidth: 1,
            color: 0x000000,
			transparent: true,
			opacity: 0.5
        })

        const positions = [];

        positions.push (0.0, 0.0, 0.0);
        positions.push (dWidth, 0.0, 0.0);
        positions.push (dWidth, 0.0, 0.0);
        positions.push (dWidth, dHeight, 0.0);
        positions.push (dWidth, dHeight, 0.0);
        positions.push (0.0, dHeight, 0.0);
        positions.push (0.0, dHeight, 0.0);
        positions.push (0.0, 0.0, 0.0);
		
        positions.push (0.0, 0.0, dDepth);
        positions.push (dWidth, 0.0, dDepth);
        positions.push (dWidth, 0.0, dDepth);
        positions.push (dWidth, dHeight, dDepth);
        positions.push (dWidth, dHeight, dDepth);
        positions.push (0.0, dHeight, dDepth);
        positions.push (0.0, dHeight, dDepth);
        positions.push (0.0, 0.0, dDepth);
		
		positions.push (0.0, 0.0, 0.0);
		positions.push (0.0, 0.0, dDepth);
		positions.push (dWidth, 0.0, 0.0);
		positions.push (dWidth, 0.0, dDepth);
		positions.push (dWidth, dHeight, 0.0);
		positions.push (dWidth, dHeight, dDepth);
		positions.push (0.0, dHeight, 0.0);
		positions.push (0.0, dHeight, dDepth);

        linegeometry.setAttribute('position', new THREE.Float32BufferAttribute(positions, 3));
        linegeometry.computeBoundingSphere(); 

		let group = new THREE.Group()
        this.edgeselement = new THREE.LineSegments(linegeometry, linematerial);

        group.add(this.edgeselement);
		group.add(this.meshelement);

        this.glelement = group; 
		
		
		
    }

}

class WebGLMeshElement extends WebGLElement {

    constructor(applicationInstance, meshUUID, meshcolor) {
		
        super();

		let normalizedUUID = Assert.UUIDValue (meshUUID);
		this.meshUUID = normalizedUUID;
		
		if (!meshcolor)
			meshcolor = 0x808080;
		
		this.material = new THREE.MeshPhongMaterial( {
			color: 0x808080,
			shininess: 50,
			specular: 0x111111
		} );
		
		let materialToUse = this.material;
		
		let group = new THREE.Group()

        this.glelement = group; 
		
		applicationInstance.axiosGetArrayBufferRequest("/ui/meshgeometry/" + normalizedUUID)
		.then(responseData => {
			let meshpositions = responseData.data;
			
			let meshgeometry = new THREE.BufferGeometry();
			meshgeometry.setAttribute('position', new THREE.Float32BufferAttribute(meshpositions, 3));
			meshgeometry.computeVertexNormals ();
			let meshelement = new THREE.Mesh( meshgeometry, materialToUse );
			meshelement.castShadow = true;
			meshelement.receiveShadow = true;
						
			group.add(meshelement);			
			
		})
		.catch(err => {
			if (err.response) {
				console.log (err.response);
			} else {
				console.log ("fatal error while retrieving mesh geometry " + normalizedUUID);
			}
		});
				
		applicationInstance.axiosGetArrayBufferRequest("/ui/meshedges/" + normalizedUUID)
		.then(responseData => {
			let linepositions = responseData.data;
			
			let linegeometry = new THREE.BufferGeometry();
			const linematerial = new THREE.LineBasicMaterial({
				linewidth: 1,
				color: 0x000000,
				transparent: true,
				opacity: 0.5
			});
			
			linegeometry.setAttribute('position', new THREE.Float32BufferAttribute(linepositions, 3));
			linegeometry.computeBoundingSphere(); 
			
			let edgeselement = new THREE.LineSegments(linegeometry, linematerial);
						
			group.add(edgeselement);			
			
		})
		.catch(err => {
			if (err.response) {
				console.log (err.response);
			} else {
				console.log ("fatal error while retrieving edges " + normalizedUUID);
			}
		});
				
    }

}

class WebGLGridElement extends WebGLElement {

    constructor(width, height, zValue, factor) {
        super();

        this.lineElement1 = null;
        this.lineElement2 = null;
        this.lineElement3 = null;

        let group = new THREE.Group()

        let gridArray1 = []
        let gridArray2 = []
        let gridArray3 = []
        let x, y;

        let lineCountX = width / factor + 1;
        let lineCountY = height / factor + 1;
		
		let gridRecursion = 5;
		let gridRecursionSquared = gridRecursion * gridRecursion;

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

    constructor ()
	{
        super();
		this.glelement = null
	}
	
	
	loadData (data, fillShapes, showStrokes) {

        const paths = data.paths;

        const group = new THREE.Group();
        group.position.x = 0;
        group.position.y = 0;
        group.position.z = 0;
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

class WebGLAmbientLight extends WebGLElement {

    constructor (paramColor, paramIntensity) {
        super();

		let dColor = Assert.NumberValue (paramColor);
		let dIntensity = Assert.NumberValue (paramIntensity);

        this.glelement = new THREE.AmbientLight( dColor, dIntensity );

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

        this.renderElements = new Map();
		
		this.camera = null;
		this.hasPerspectiveView = false;
		
		this.controls = null;
    }

    setup2DView(paramSizeX, paramSizeY, paramNear, paramFar) 
	{
		let dSizeX = Assert.NumberValue (paramSizeX);
		let dSizeY = Assert.NumberValue (paramSizeY);
		let dNear = Assert.NumberValue (paramNear);
		let dFar = Assert.NumberValue (paramFar);
		
        this.camera = new THREE.OrthographicCamera(0, dSizeX, 0, dSizeY, dNear, dFar);
        this.camera.position.z = dFar * 0.99;
    }

    setupOrthoView(paramSizeX, paramSizeY, paramNear, paramFar) 
	{
		let dSizeX = Assert.NumberValue (paramSizeX);
		let dSizeY = Assert.NumberValue (paramSizeY);
		let dNear = Assert.NumberValue (paramNear);
		let dFar = Assert.NumberValue (paramFar);

        this.camera = new THREE.OrthographicCamera(-dSizeX * 0.5, dSizeX * 0.5, -dSizeY * 0.5, dSizeY * 0.5, dNear, dFar);
        this.camera.position.z = dFar * 0.99;
    }
	
    setupPerspectiveView(paramFieldOfView, paramAspectRatio, paramNear, paramFar) 
	{
				
		let dFieldOfView = Assert.NumberValue (paramFieldOfView);
		let dAspectRatio = Assert.NumberValue (paramAspectRatio);
		let dNear = Assert.NumberValue (paramNear);
		let dFar = Assert.NumberValue (paramFar);
		
        this.camera = new THREE.PerspectiveCamera( dFieldOfView, dAspectRatio, dNear, dFar );
		this.camera.up.set (0, 0, 1); // Set Z up
		this.hasPerspectiveView = true;
	}

	setCameraPosition (paramX, paramY, paramZ) 
	{	
		if (!this.camera)
			throw "Camera has not been set up";

		let dX = Assert.NumberValue (paramX);
		let dY = Assert.NumberValue (paramY);
		let dZ = Assert.NumberValue (paramZ);

		this.camera.position.set( dX, dY, dZ );
	}

    setupDOMElement(domelement) {
		if (!domelement)
			throw "No DOM Element to attach to";

        domelement.append(this.renderer.domElement);

        // const rendererWindowPosition = this.renderer.domElement.getBoundingClientRect();
        // console.log(rendererWindowPosition);

        const infoboxDiv = document.createElement('div');
        infoboxDiv.id = 'infobox_points';
        infoboxDiv.style.display = 'none'
        infoboxDiv.style.position = 'absolute';
        infoboxDiv.style.top = 0;
        infoboxDiv.style.left = 0;
        infoboxDiv.style.zIndex = 100;
        infoboxDiv.style.padding = '5px';
        infoboxDiv.style.borderRadius = '5px';
        infoboxDiv.style.fontFamily = 'Arial';
        infoboxDiv.style.fontSize = '8pt';
        infoboxDiv.style.fontWeight = 'bold';
        infoboxDiv.style.background = 'rgba(0, 0, 0, 0.7)';
        infoboxDiv.style.color = 'white';
        infoboxDiv.style.visibility = 'none';
        infoboxDiv.innerText = '---';

        this.renderer.domElement.parentElement.style.position = 'relative';
        this.renderer.domElement.parentElement.appendChild(infoboxDiv);
    }

    resizeTo(sizex, sizey) {
        if (this.renderer) {
            this.renderer.setSize(sizex, sizey);
        }
		
		if (this.camera && this.hasPerspectiveView) {
			if ((sizex > 0) && (sizey > 0)) {
				this.camera.aspect = sizex / sizey;
				this.camera.updateProjectionMatrix ();
			}
			
		}
		
    }

    renderScene() {

        if (this.scene && this.camera) {
            this.renderer.render(this.scene, this.camera);
        }

    }

	getRaycasterCollisions(elementidentifier, mouseX, mouseY) {
		
		let elementMesh = this.findElement (elementidentifier);
		
		if (!elementMesh) 
			return -1;
			
		if (!elementMesh.glelement) 
			return -1;
			
		const rayOrigin = new THREE.Vector3(mouseX, mouseY, -100);
		const rayDirection = new THREE.Vector3(0, 0, 1);

		this.raycaster.ray.origin.copy(rayOrigin);
		this.raycaster.ray.direction.copy(rayDirection);                

		//console.log ("sending raycast at " + mouseX + "/" + mouseY);
		const collisionsPoints = this.raycaster.intersectObject(elementMesh.glelement);
		
		if (collisionsPoints.length > 0) {
			return collisionsPoints[0].faceIndex;
		} else {
			return -1;
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

		let selectedObject = this.scene.getObjectByName(identifier);
		if (selectedObject) {
			this.scene.remove(selectedObject);
		}
		
        if (this.renderElements.has(identifier)) {
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

    getElementPosition(identifier) {
        const element = this.findElement(identifier);

        if (!element || !element.position) {
            return null;
        }

        return element.position;
    }

    getElementEdgePositions(identifier) {
        const element = this.findElement(identifier);

        if (!element || !element.glelement.geometry.attributes.position.array) {
            return null;
        }

        return element.glelement.geometry.attributes.position.array;
    }

    getElementScale(identifier) {
        const element = this.findElement(identifier);

        if (!element || !element.glelement.scale) {
            return null;
        }

        return element.glelement.scale;
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

        let gridElement = new WebGLGridElement(width, height, zValue, factor);

        this.addElement(identifier, gridElement);

        return gridElement;
    }

    add2DLineGeometry(identifier, lineData, zValue, lineThickness, lineColor, vertexcolors) {
        if (!identifier)
            return;
		
		this.removeElement (identifier);

        let linesElement = new WebGLLinesElement(lineData, zValue, lineThickness, lineColor, vertexcolors);

        this.addElement(identifier, linesElement);

        return linesElement;
    }

    add2DPointsGeometry(identifier, pointsData, zValue, pointsSize, pointsColor, vertexcolors) {
        if (!identifier)
            return;
		
		this.removeElement (identifier);

        let pointsElement = new WebGLPointsElement(pointsData, zValue, pointsSize, pointsColor, vertexcolors);

        this.addElement(identifier, pointsElement);

        return pointsElement;
    }

    addSVGImage(identifier, url, zValue, fillShapes, showStrokes) {
        if (!identifier)
            return;

        let scene = this.scene;

        let svgElement = new WebGLSVGElement();
        this.addElement(identifier, svgElement);
		
		svgElement.setPosition (0, 0, zValue);

        const loader = new SVGLoader();
        loader.load(url, (data) => {
						
			svgElement.loadData (data, fillShapes, showStrokes);
			
            if (svgElement.glelement) {
                svgElement.glelement.name = identifier;
                scene.add(svgElement.glelement);
				
				svgElement.updateGLPosition ();						
            }
			
			this.renderScene ();
									
        });

    }
	
	addAmbientLight (identifier, color, intensity)
	{
        if (!identifier)
            return;
		
		let lightElement = new WebGLAmbientLight (color, intensity);
				
		this.addElement(identifier, lightElement);
		
		return lightElement;
	}
	
	addBoxElement (identifier, width, height, depth)
	{
        if (!identifier)
            return;
		
		let boxElement = new WebGLBoxElement (width, height, depth);
				
		this.addElement(identifier, boxElement);
		
		return boxElement;
	}

	addMeshElement (identifier, applicationInstance, meshUUID, meshColor)
	{
        if (!identifier)
            return;
		
		let meshElement = new WebGLMeshElement (applicationInstance, meshUUID, meshColor);
				
		this.addElement(identifier, meshElement);
		
		return meshElement;
	}
	
	pick2DElement (x, y)
	{		
		x; y;
		/*if (this.camera) {
			
			console.log("pick X: " + x + " y: " + y);
			
			const pointer = new THREE.Vector2();
			pointer.x = x;
			pointer.y = y;
			
			this.raycaster.setFromCamera( pointer, this.camera );
			
			const intersects = this.raycaster.intersectObjects( this.scene.children[1], true );
			if (intersects.length > 0) {
				alert ("intersect");
			}
		} */
		
	}

    setupDemoScene() {

		// Lights

		//this.scene.add(  );

		this.spotLight = new THREE.SpotLight( 0xffffff, 500 );
		this.spotLight.name = 'Spot Light';
		this.spotLight.angle = Math.PI / 5;
		this.spotLight.penumbra = 0.3;
		this.spotLight.position.set( 10, 5, 10 );
		this.spotLight.castShadow = true;
		this.spotLight.shadow.camera.near = 8;
		this.spotLight.shadow.camera.far = 30;
		this.spotLight.shadow.mapSize.width = 1024;
		this.spotLight.shadow.mapSize.height = 1024;
		this.scene.add( this.spotLight );


		this.dirLight = new THREE.DirectionalLight( 0xffffff, 3 );
		this.dirLight.name = 'Dir. Light';
		this.dirLight.position.set( 10, 0, 10 );
		this.dirLight.castShadow = true;
		this.dirLight.shadow.camera.near = 1;
		this.dirLight.shadow.camera.far = 10;
		this.dirLight.shadow.camera.right = 15;
		this.dirLight.shadow.camera.left = - 15;
		this.dirLight.shadow.camera.top	= 15;
		this.dirLight.shadow.camera.bottom = - 15;
		this.dirLight.shadow.mapSize.width = 1024;
		this.dirLight.shadow.mapSize.height = 1024;
		this.scene.add( this.dirLight );


		// Geometry
		/*let geometry = new THREE.TorusKnotGeometry( 25, 8, 75, 20 );
		let material = new THREE.MeshPhongMaterial( {
			color: 0xff0000,
			shininess: 150,
			specular: 0x222222
		} ); */

		/*this.torusKnot = new THREE.Mesh( geometry, material );
		this.torusKnot.scale.multiplyScalar( 1 / 18 );
		this.torusKnot.position.z = 3;
		this.torusKnot.castShadow = true;
		this.torusKnot.receiveShadow = true;
		this.scene.add( this.torusKnot ); */


/*		let geometry = new THREE.BoxGeometry( 30, 30, 1 );
		let material = new THREE.MeshPhongMaterial( {
			color: 0xa0adaf,
			shininess: 150,
			specular: 0x111111
		} );

		const ground = new THREE.Mesh( geometry, material );
		ground.castShadow = false;
		ground.receiveShadow = true;
		ground.position.z = -6.5;
		this.scene.add( ground );  */
		
		this.scene.background = new THREE.Color( 0xffffff );
		
		const mesh = new THREE.Mesh( new THREE.PlaneGeometry( 300, 300 ), new THREE.MeshPhongMaterial( { color: 0xc0c0c0, depthWrite: false } ) );
				//mesh.rotation.x = - Math.PI / 2;
				mesh.position.z = -6;
				//mesh.receiveShadow = true;	
				this.scene.add( mesh );
				
		
		this.scene.fog = new THREE.Fog( 0xffffff, 1505, 1650 );
		
		const hemiLight = new THREE.HemisphereLight( 0xffffff, 0x8d8d8d, 3 );
		hemiLight.position.set( 0, 500, 0 );
		this.scene.add( hemiLight );
		
		this.renderer.shadowMap.enabled = true;
		this.renderer.shadowMap.type = THREE.BasicShadowMap;

		// Mouse control
		const controls = new OrbitControls( this.camera, this.renderer.domElement );
		controls.mouseButtons = {
			LEFT: THREE.MOUSE.NONE,
			MIDDLE: THREE.MOUSE.PAN,
			RIGHT: THREE.MOUSE.ROTATE
		}		
		
		controls.target.set( 0, 0, 2 );
		controls.update();

		this.clock = new THREE.Clock();
				
		return;
    }
}

export default WebGLImpl;
