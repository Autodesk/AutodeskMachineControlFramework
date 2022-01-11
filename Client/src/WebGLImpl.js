
import * as THREE from 'three';

class WebGLImpl {
	
	
	constructor () {					
		this.renderer = new THREE.WebGLRenderer({ antialias: true });
		this.scene = new THREE.Scene();		
		this.scene.background = new THREE.Color( 0xffffff );							
	}
	
	
	setup2DView (sizex, sizey, near, far) {
		this.camera = new THREE.OrthographicCamera( 0, sizex, 0, sizey, near, far );
		this.camera.position.z = far * 0.99;
	}
	
	setupDOMElement (domelement) {
		domelement.append (this.renderer.domElement);
	}
	
	resizeTo (sizex, sizey)
	{
		if (this.renderer) {
			this.renderer.setSize( sizex, sizey );
		}
	}
	
	renderScene () {
		
		if (this.scene && this.camera) {
			this.renderer.render(this.scene, this.camera);
		}
		
	}
	
	
	create2DLineGeometry (lineData, zValue, lineThickness, lineColor) {
	
		var geometry = new THREE.BufferGeometry();
		const material = new THREE.MeshBasicMaterial( { vertexColors: false, color: lineColor } );

		const positions = [];
		
		const lineCount = lineData.length / 4;
		
		for ( let i = 0; i < lineCount; i ++ ) {

			const x1 = lineData[i * 4];
			const y1 = lineData[i * 4 + 1];
			const x2 = lineData[i * 4 + 2];
			const y2 = lineData[i * 4 + 3];
			
			const dx = x2 - x1;
			const dy = y2 - y1;
			const dlen = Math.sqrt (dx * dx + dy * dy);
			
			if (dlen > 0) {
				const ux = - dy / dlen * lineThickness * 0.5;
				const uy = dx / dlen * lineThickness * 0.5;
									
				positions.push( x1 + ux, y1 + uy, zValue );
				positions.push( x2 + ux, y2 + uy, zValue );
				positions.push( x2 - ux, y2 - uy, zValue );
				positions.push( x2 - ux, y2 - uy, zValue );
				positions.push( x1 - ux, y1 - uy, zValue );
				positions.push( x1 + ux, y1 + uy, zValue );
			}
		}
		

		geometry.setAttribute( 'position', new THREE.Float32BufferAttribute( positions, 3 ) );
		geometry.computeBoundingSphere();

		var line = new THREE.Mesh( geometry, material );
		
		return line;
	
	}
	
	add2DGridGeometry (width, height, zValue) {
		var group = this.create2DGridGeometry (width, height, zValue);
		this.scene.add (group);
		
		return group;
	}
	
	create2DGridGeometry (width, height, zValue) {
		  var group = new THREE.Group()
		
		  var gridArray1 = []
		  var gridArray2 = []
		  var gridArray3 = []
		  var x, y;
		  
		  var factor = 5;
		  
		  var lineCountX = width / factor + 1; 
		  var lineCountY = height / factor + 1; 
		  
		  for (x = 0; x < lineCountX; x++) {
			if (x % 4 != 0) {
			  gridArray1.push (x * factor, 0, x * factor, height);
			}
		  }

		  for (y = 0; y < lineCountY; y++) {
			if (y % 4 != 0) {
			  gridArray1.push (0, y * factor, width, y * factor);
			}
		  }

		  for (x = 0; x < (lineCountX / 4); x++) {
			if (x % 4 != 0) {
			  gridArray2.push (x * 4 * factor, 0, x * 4 * factor, height);
			}
		  }

		  for (y = 0; y < (lineCountY / 4); y++) {
			if (y % 4 != 0) {
			  gridArray2.push (0, y * 4 * factor, width, y * 4 * factor);
			}
		  }

		  for (x = 0; x < (lineCountX / 16); x++) {
			  gridArray3.push (x * 16 * factor, 0, x * 16 * factor, height);
		  }

		  for (y = 0; y < (lineCountY / 16); y++) {
			  gridArray3.push (0, y * 16 * factor, width, y * 16 * factor);
		  }

		  var line1 = this.create2DLineGeometry (gridArray1, zValue, 0.5, 0xe0e0e0);
		  var line2 = this.create2DLineGeometry (gridArray2, zValue, 0.6, 0xc8c8c8);
		  var line3 = this.create2DLineGeometry (gridArray3, zValue, 0.8, 0xc0c0c0);
		  group.add (line1);
		  group.add (line2);
		  group.add (line3);
		  
		  return group;
		  
	}

	add2DImage (width, height, url, zValue)
	{
		var newImage = this.create2DImage (width, height, url, zValue);
		this.scene.add(newImage);
		
		return newImage;
	}


	create2DImage (width, height, url, zValue)
	{
		
		const texture = new THREE.TextureLoader().load(url );
		
		const positions = [];
		positions.push (0, 0, zValue);
		positions.push (0, height, zValue);
		positions.push (width, height, zValue);
		positions.push (width, height, zValue);
		positions.push (width, 0, zValue);
		positions.push (0, 0, zValue);

		const uvcoords = [];
		uvcoords.push (0, 1);
		uvcoords.push (0, 0);
		uvcoords.push (1, 0);
		uvcoords.push (1, 0);
		uvcoords.push (1, 1);
		uvcoords.push (0, 1);
		
		var geometry = new THREE.BufferGeometry();		
		
		geometry.setAttribute( 'position', new THREE.Float32BufferAttribute( positions, 3 ) );
		geometry.setAttribute( 'uv', new THREE.Float32BufferAttribute( uvcoords, 2 ) );
		geometry.computeBoundingSphere();		
				
		var materialDecalRoof=new THREE.MeshBasicMaterial( {
			'map': texture,
			'wireframe': false,
			transparent: true, 
			opacity: 1.0,
			color: 0xffffff,
		} ); 
		
		var mesh= new THREE.Mesh( geometry, materialDecalRoof);
		return mesh;

	}


}


export default WebGLImpl;
