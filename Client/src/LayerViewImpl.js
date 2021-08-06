import Two from "two.js";

class LayerViewImpl {

  constructor (domelement) {
  
    this.domelement = domelement;
	
    this.twoinstance = new Two({
		type: Two.Types.svg,
		width: domelement.clientWidth,
		height: domelement.clientHeight,
		autostart: false
	}).appendTo(domelement);
	
	this.gridData = {		
		gridGroup: null,
		gridLineGroup1: null,
		gridLineGroup2: null,
		gridLineGroup3: null
	}
	
	this.sliceGroup = null;
	
	this.currentSize = {gridWidth: 0, gridHeight: 0}  
	this.transform = { x: 0, y: 0, scaling: 1.0 } 
		
	this.updateSize (domelement.clientWidth, domelement.clientHeight);
	
  }
  
  
  
  updateSize (width, height)
  {
		this.twoinstance.renderer.setSize (width, height);  
		
		var newWidth = this.twoinstance.width + 50;
		var newHeight = this.twoinstance.height + 50;
	
		if ((this.currentSize.gridWidth < newWidth) || (this.currentSize.gridHeight < newHeight)) {
			this.currentSize.gridWidth = newWidth;
			this.currentSize.gridHeight = newHeight;
						
			this.createGrid (this.twoinstance, newWidth, newHeight);	    			
			
		}
		
		this.twoinstance.update ();
  }
     
  
  createGrid (twoinstance, width, height)
  {

	  
	  var createGridAsync = async function (twoinstance, gridData, width, height) {
		  
		  if (gridData.gridGroup) {
			twoinstance.remove (gridData.gridGroup);
			gridData.gridGroup = null;
		  }
		  
		  var gridArray1 = []
		  var gridArray2 = []
		  var gridArray3 = []
		  var x, y;
		  
		  var factor = 3;
		  
		  var lineCountX = width / factor + 1; 
		  var lineCountY = height / factor + 1; 
		  
		  for (x = 0; x < lineCountX; x++) {
			if (x % 4 != 0) {
			  gridArray1.push (twoinstance.makeLine(x * factor, 0, x * factor, height));
			}
		  }

		  for (y = 0; y < lineCountY; y++) {
			if (y % 4 != 0) {
			  gridArray1.push (twoinstance.makeLine(0, y * factor, width, y * factor));
			}
		  }

		  for (x = 0; x < (lineCountX / 4); x++) {
			if (x % 4 != 0) {
			  gridArray2.push (twoinstance.makeLine(x * 4 * factor, 0, x * 4 * factor, height));
			}
		  }

		  for (y = 0; y < (lineCountY / 4); y++) {
			if (y % 4 != 0) {
			  gridArray2.push (twoinstance.makeLine(0, y * 4 * factor, width, y * 4 * factor));
			}
		  }

		  for (x = 0; x < (lineCountX / 16); x++) {
			  gridArray3.push (twoinstance.makeLine(x * 16 * factor, 0, x * 16 * factor, height));
		  }

		  for (y = 0; y < (lineCountY / 16); y++) {
			  gridArray3.push (twoinstance.makeLine(0, y * 16 * factor, width, y * 16 * factor));
		  }
		  
		  var group1 = twoinstance.makeGroup(gridArray1);
		  group1.stroke = '#f0f0f0';
		  group1.linewidth = 0.5;

		  var group2 = twoinstance.makeGroup(gridArray2);
		  group2.stroke = '#e8e8e8';
		  group2.linewidth = 0.6;

		  var group3 = twoinstance.makeGroup(gridArray3);
		  group3.stroke = '#e0e0e0';
		  group3.linewidth = 0.8;
		  
		  gridData.gridLineGroup1 = group1;
		  gridData.gridLineGroup2 = group2;
		  gridData.gridLineGroup3 = group3;
		  
		  gridData.gridGroup = twoinstance.makeGroup(group1, group2, group3);
	  }
	  
	  createGridAsync (this.twoinstance, this.gridData, width, height);
	  	
  }  



  updateTransform ()
  {
  	
  
	if (this.gridData.gridGroup) {
	
		var gridScale = this.transform.scaling;
		if (gridScale < 1.0) {
			gridScale = 1.0;
		}
	
		if (gridScale > 4.0) {
			gridScale = gridScale / 4.0;
		}
		
		
		var fullGridSize = gridScale * 16.0 * 3;
		var gridTranslationX = this.transform.x - Math.ceil ((this.transform.x / fullGridSize)) * fullGridSize;
		var gridTranslationY = this.transform.y - Math.ceil ((this.transform.y / fullGridSize)) * fullGridSize;
		
	    this.gridData.gridGroup.scale=gridScale; 
		this.gridData.gridGroup.translation.set (gridTranslationX, gridTranslationY);
		
		this.gridData.gridLineGroup1.linewidth = 0.5 / gridScale;
		this.gridData.gridLineGroup2.linewidth = 0.6 / gridScale;
		this.gridData.gridLineGroup3.linewidth = 0.8 / gridScale;	  
		
		
		
	}

	if (this.sliceGroup) {
	
	  var sliceScale = this.transform.scaling;
	
	  this.sliceGroup.scale=sliceScale; 
	  this.sliceGroup.linewidth=1.0 / sliceScale; 
	  this.sliceGroup.translation.set (this.transform.x, this.transform.y);
	}
		
	this.twoinstance.update ();
  }

  
  loadLayer (segmentsArray) {

	  if (this.sliceGroup) {
		this.twoinstance.remove (this.sliceGroup);
		this.sliceGroup = null;
	  }
	  
	  var height = this.twoinstance.height;
	  
	  var paths=[];
	  var segmentCount = segmentsArray.length;
	  var segmentIndex;


	  for (segmentIndex = 0; segmentIndex < segmentCount; segmentIndex++) {
		var segment = segmentsArray[segmentIndex];
		  
		if ((segment.type === "loop") ||(segment.type === "polyline")) {
			
			var pointCount = segment.points.length;
			var pointIndex;
			
			var oldx = segment.points[0].x;
			var oldy = segment.points[0].y;
			
			for (pointIndex = 1; pointIndex < pointCount; pointIndex ++) {
				var x = segment.points[pointIndex].x;
				var y = segment.points[pointIndex].y;
				
				paths.push (this.twoinstance.makeLine (oldx, height - oldy, x, height - y));
				
				oldx = x;
				oldy = y;
			}
						
						
		} 

		if (segment.type === "hatch") {
		 	
			var lineCount = segment.points.length / 2;
			var lineIndex;
			
			for (lineIndex = 0; lineIndex < lineCount; lineIndex ++) {
				var x1 = segment.points[lineIndex * 2].x;
				var y1 = segment.points[lineIndex * 2].y;
				var x2 = segment.points[lineIndex * 2 + 1].x;
				var y2 = segment.points[lineIndex * 2 + 1].y;
				
				paths.push (this.twoinstance.makeLine (x1, height - y1, x2, height - y2));
			}
						
							
			//lines.push (this.twoinstance.makeLine(10, 20, 50, 50));	
			
		}
		  
	  }
	  
	  	  
	  this.sliceGroup = this.twoinstance.makeGroup (paths);
	  
	  this.updateTransform ();

  }
  
  Drag (deltaX, deltaY) {
  
	this.transform.x = this.transform.x + deltaX;	
	this.transform.y = this.transform.y + deltaY;	
	
	this.updateTransform ();
	
  }

  SetAbsoluteScaling (newScaling, centerx, centery) {
	 	
	if (!centerx) {
		centerx = this.twoinstance.width * 0.5;
	}

	if (!centery) {
		centery = this.twoinstance.height * 0.5;
	}
		
	this.ScaleRelative (newScaling / this.transform.scaling, centerx, centery);
	  
  }
  
  ScaleRelative (factor, centerx, centery) {

	if (!centerx) {
		centerx = this.twoinstance.width * 0.5;
	}

	if (!centery) {
		centery = this.twoinstance.height * 0.5;
	}
	 	
	this.transform.x = this.transform.x - centerx;	
	this.transform.y = this.transform.y - centery;	
	
	this.transform.scaling = this.transform.scaling * factor;

	this.transform.x = this.transform.x * factor + centerx;	
	this.transform.y = this.transform.y * factor + centery;	

	
	this.updateTransform ();	
	  
  }

  
}


export default LayerViewImpl;