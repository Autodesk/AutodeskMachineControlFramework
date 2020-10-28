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
	
	this.loadLayer ();

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
  	
  
	if (this.gridGroup) {
	
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
		
	    this.gridGroup.scale=gridScale; 
		this.gridGroup.translation.set (gridTranslationX, gridTranslationY);
		
		this.gridLineGroup1.linewidth = 0.5 / gridScale;
		this.gridLineGroup2.linewidth = 0.6 / gridScale;
		this.gridLineGroup3.linewidth = 0.8 / gridScale;	  
		
		
		
	}

	if (this.sliceGroup) {
	
	  var sliceScale = this.transform.scaling;
	
	  this.sliceGroup.scale=sliceScale; 
	  this.sliceGroup.linewidth=1.0 / sliceScale; 
	  this.sliceGroup.translation.set (this.transform.x, this.transform.y);
	}
		
	this.twoinstance.update ();
  }

  
  loadLayer () {

	  if (this.sliceGroup) {
		this.twoinstance.remove (this.sliceGroup);
		this.sliceGroup = null;
	  }
	  
      var line1 = this.twoinstance.makeLine(10, 20, 50 ,50);
      var line2 = this.twoinstance.makeLine(50, 20, 300 ,400);
	  this.sliceGroup = this.twoinstance.makeGroup (line1, line2);
	  
	  this.twoinstance.update ();

  }
  
  Drag (deltaX, deltaY) {
  
	this.transform.x = this.transform.x + deltaX / this.transform.scaling;	
	this.transform.y = this.transform.y + deltaY / this.transform.scaling;	
	
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
	 	
	this.transform.x = this.transform.x - centerx / this.transform.scaling;	
	this.transform.y = this.transform.y - centery / this.transform.scaling;	
	
	this.transform.scaling = this.transform.scaling * factor;

	this.transform.x = this.transform.x + centerx / this.transform.scaling;	
	this.transform.y = this.transform.y + centery / this.transform.scaling;	
	
	this.updateTransform ();	
	  
  }

  
}


export default LayerViewImpl;