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

const LAYERVIEW_MINSCALING = 0.4;
const LAYERVIEW_MAXSCALING = 4000.0;
const LAYERVIEW_MINVELOCITYRANGE = 1.0;

// Segments of parts that have been disabled are still drawn, but in this neutral gray.
const LAYERVIEW_DISABLEDPARTCOLOR = 0xB4B4B4;

// Grid level-of-detail switch band. The grid geometry is self-similar every 5
// subdivisions, so the lower/upper bounds MUST keep a 5x ratio for the wrap to
// stay seamless (LOWER * GRID_LOD_RECURSION === UPPER). Lowering the band
// compared to the previous 0.5..2.5 keeps the on-screen grid finer and avoids
// the coarse, "clunky" cells that appeared at the top of the old band right
// after a zoom step crossed a threshold.
const LAYERVIEW_GRIDLOD_LOWER = 0.25;
const LAYERVIEW_GRIDLOD_UPPER = 1.25;
const LAYERVIEW_GRIDLOD_RECURSION = 5.0;

// The grid mesh is (re)generated only on viewport resize, but at render time it
// is scaled by gridScale, which can drop as low as LAYERVIEW_GRIDLOD_LOWER. The
// mesh must therefore span (viewport / LOWER) model units so it still fills the
// screen at the coarsest LOD. Deriving it from LOWER keeps the two in sync if
// the LOD band is ever retuned (e.g. 1 / 0.25 => a 4x-viewport mesh).
const LAYERVIEW_GRIDMESH_VIEWPORTFACTOR = Math.ceil (1.0 / LAYERVIEW_GRIDLOD_LOWER);

class LayerViewImpl {

    constructor(glInstance) {

        this.glInstance = glInstance;

        this.currentSize = {
            gridWidth: 0,
            gridHeight: 0,
            viewPortWidth: 0,
            viewPortHeight: 0
        }
        this.transform = {
            x: 0,
            y: 0,
            scaling: 1.0
        }
        this.origin = {
            x: 0,
            y: 0
        }
        this.transformAngleDegrees = 0.0;
        this.rotationCenter = {
            x: 0,
            y: 0
        };
        this.translation = {
            x: 0,
            y: 0
        };
		
		this.lineScaleLevel = 0;
		this.renderNeedsUpdate = true;

		this.layerSegmentsArray = null;
		this.layerPartNames = new Map ();
		this.layerDisabledParts = new Set ();
		this.partBoundsCache = null;
		this.partBoundsCacheKey = "";

		// Invoked after every pan/zoom/placement change so screen-space overlays can follow.
		this.onTransformChanged = null;
		
		this.layerPointsMode = "unicolor";
		this.layerPointsArray = null;
		this.layerPointsColorArray = null;
		this.layerPointsVelocities = [];
		this.layerPointsMaxVelocity = LAYERVIEW_MINVELOCITYRANGE;
		this.layerPointsMinVelocity = LAYERVIEW_MINVELOCITYRANGE;
		
        this.linesCoordinates = [];
		
		this.toolpathVisible = true;
		this.pointsVisible = true;
		this.showLaserOffPoints = false;
		this.filteredToOriginalIndexMap = null;

		this.gridColor = undefined;
        
		this.updateTransform ();

    }

    applyColors(colorSet) {
        if (!colorSet) return;
        if (colorSet.background && this.glInstance)
            this.glInstance.setBackgroundColor(colorSet.background);
        if (colorSet.grid !== undefined)
            this.gridColor = colorSet.grid;
        this.rebuildGrid();
        this.RenderScene(true);
    }

    rebuildGrid() {
        if (!this.glInstance) return;
        if (this.currentSize.gridWidth <= 0 || this.currentSize.gridHeight <= 0) return;
        var gridObject = this.glInstance.scene.getObjectByName("grid");
        if (gridObject)
            this.glInstance.scene.remove(gridObject);
        this.glInstance.add2DGridGeometry("grid", this.currentSize.gridWidth, this.currentSize.gridHeight, 50, 5, this.gridColor);
        this.updateTransform();
        this.renderNeedsUpdate = true;
    }

    updateSize(width, height) {
        if ((width > 0) && (height > 0)) {

            this.currentSize.viewPortWidth = width;
            this.currentSize.viewPortHeight = height;

            if (this.glInstance) {
                this.glInstance.resizeTo(width, height);
                this.glInstance.setup2DView(width, height, 0.1, 100);
            }

            var newWidth = width * LAYERVIEW_GRIDMESH_VIEWPORTFACTOR + 50;
            var newHeight = height * LAYERVIEW_GRIDMESH_VIEWPORTFACTOR + 50;

            if ((this.currentSize.gridWidth < newWidth) || (this.currentSize.gridHeight < newHeight)) {
                this.currentSize.gridWidth = newWidth;
                this.currentSize.gridHeight = newHeight;

                this.createGrid(newWidth, newHeight);

            }

        }

		this.renderNeedsUpdate = true;
    }

    createGrid(width, height) {
        if (!this.glInstance)
            return;

        var gridObject = this.glInstance.scene.getObjectByName("grid");
        if (gridObject)
            this.glInstance.scene.remove(gridObject);

        this.glInstance.add2DGridGeometry("grid", width, height, 50, 5, this.gridColor);
		this.updateTransform ();
		
		this.renderNeedsUpdate = true;
		

    }

    transformToolpathPoint(x, y) {
        const angleInRadians = this.transformAngleDegrees * Math.PI / 180.0;
        const cosine = Math.cos(angleInRadians);
        const sine = Math.sin(angleInRadians);
        const relativeX = x - this.rotationCenter.x;
        const relativeY = y - this.rotationCenter.y;

        return {
            x: relativeX * cosine - relativeY * sine + this.rotationCenter.x + this.translation.x,
            y: relativeX * sine + relativeY * cosine + this.rotationCenter.y + this.translation.y
        };
    }

    updateTransform() {

        if (!this.glInstance)
            return;

        const placementOffset = this.transformToolpathPoint(0, 0);
        const toolpathPositionX = this.transform.x + (this.origin.x + placementOffset.x) * this.transform.scaling;
        const toolpathPositionY = this.transform.y - (this.origin.y + placementOffset.y) * this.transform.scaling;

        var gridgeometry = this.glInstance.findElement("grid");
        if (gridgeometry) {
            var gridScale = this.transform.scaling;
			var lineScaleLevel = 1;

            if (gridScale < LAYERVIEW_GRIDLOD_LOWER) {
                gridScale = gridScale * LAYERVIEW_GRIDLOD_RECURSION;
				lineScaleLevel = 0.8;
            }

            if (gridScale > LAYERVIEW_GRIDLOD_UPPER) {
                gridScale = gridScale / LAYERVIEW_GRIDLOD_RECURSION;
				lineScaleLevel = 0.4;
            }

            if (gridScale > LAYERVIEW_GRIDLOD_UPPER) {
                gridScale = gridScale / LAYERVIEW_GRIDLOD_RECURSION;
				lineScaleLevel = 0.25;
            }

            if (gridScale > LAYERVIEW_GRIDLOD_UPPER) {
                gridScale = gridScale / LAYERVIEW_GRIDLOD_RECURSION;
				lineScaleLevel = 0.125;
            }

            if (gridScale > LAYERVIEW_GRIDLOD_UPPER) {
                gridScale = gridScale / LAYERVIEW_GRIDLOD_RECURSION;
				lineScaleLevel = 0.06;
            }

            if (gridScale > LAYERVIEW_GRIDLOD_UPPER) {
                gridScale = gridScale / LAYERVIEW_GRIDLOD_RECURSION;
				lineScaleLevel = 0.02;
            }
						
            if (gridScale > LAYERVIEW_GRIDLOD_UPPER) {
                gridScale = gridScale / LAYERVIEW_GRIDLOD_RECURSION;
				lineScaleLevel = 0.01;
            }

			this.updateLineScaleLevel (lineScaleLevel);

            var fullGridSize = gridScale * 25.0 * 5;
            var gridTranslationX = this.transform.x - Math.ceil((this.transform.x / fullGridSize)) * fullGridSize;
            var gridTranslationY = this.transform.y - Math.ceil((this.transform.y / fullGridSize)) * fullGridSize;

            gridgeometry.setPositionXY(gridTranslationX, gridTranslationY);
            gridgeometry.setScaleXY(gridScale, gridScale);
        }

        var layerlinesgeometry = this.glInstance.findElement("layerdata_lines");
        if (layerlinesgeometry) {
            layerlinesgeometry.setPositionXY(toolpathPositionX, toolpathPositionY);
            layerlinesgeometry.setScaleXY(this.transform.scaling,  - this.transform.scaling);
            // Three.js composes rotation before the screen-space Y inversion from
            // setScaleXY. Negating the angle therefore produces a positive
            // counter-clockwise rotation in machine coordinates.
            layerlinesgeometry.setRotationZ(-this.transformAngleDegrees * Math.PI / 180.0);
        }

        var layerhighlightgeometry = this.glInstance.findElement("layerdata_highlight");
        if (layerhighlightgeometry) {
            layerhighlightgeometry.setPositionXY(toolpathPositionX, toolpathPositionY);
            layerhighlightgeometry.setScaleXY(this.transform.scaling,  - this.transform.scaling);
            layerhighlightgeometry.setRotationZ(-this.transformAngleDegrees * Math.PI / 180.0);
        }

        var layerpointsgeometry = this.glInstance.findElement("layerdata_points");
        if (layerpointsgeometry) {
            layerpointsgeometry.setPositionXY(toolpathPositionX, toolpathPositionY);
            layerpointsgeometry.setScaleXY(this.transform.scaling,  - this.transform.scaling);
            layerpointsgeometry.setRotationZ(-this.transformAngleDegrees * Math.PI / 180.0);
        } 

        var buildplategeometry = this.glInstance.findElement("buildplate");
        if (buildplategeometry) {
            buildplategeometry.setPositionXY(this.transform.x, this.transform.y, buildplategeometry);
            buildplategeometry.setScaleXY(this.transform.scaling,  - this.transform.scaling);
        }
		
		this.renderNeedsUpdate = true;

		if (typeof this.onTransformChanged === "function")
			this.onTransformChanged ();
    }
	
	updateLineScaleLevel (newLineScaleLevel)
	{
		if (newLineScaleLevel != this.lineScaleLevel) {
			
			this.lineScaleLevel = newLineScaleLevel;
			
			this.updateLoadedLayer ();
			this.updateLayerPoints ();

			this.updateTransform();
			this.RenderScene(true);		
			
			
		}
	}
	
	
	



	getPathBoundaries() {
		let layerLines = this.glInstance.findElement("layerdata_lines");
		if (layerLines) {
			const pathBoundaries = layerLines.glelement.geometry.boundingSphere;
			if (!pathBoundaries)
				return null;

			const transformedCenter = this.transformToolpathPoint(pathBoundaries.center.x, pathBoundaries.center.y);

			return {
				center: {
					x: transformedCenter.x,
					y: transformedCenter.y,
					z: pathBoundaries.center.z
				},
				radius: pathBoundaries.radius
			};
		} else {
			return null;
		}
	}


    loadLayer(segmentsArray, partsArray) {
		this.layerSegmentsArray = segmentsArray;
		this.layerPartNames = new Map ();
		this.layerDisabledParts = new Set ();
		if (Array.isArray (partsArray)) {
			for (let part of partsArray) {
				if (part && (typeof part.uuid === "string")) {
					let partUUID = part.uuid.toLowerCase ();
					this.layerPartNames.set (partUUID, (typeof part.name === "string") ? part.name : "");
					if (part.disabled === true)
						this.layerDisabledParts.add (partUUID);
				}
			}
		}
		this.partBoundsCache = null;
		this.updateLoadedLayer ();

		this.updateTransform();
		this.RenderScene(true);		
		
	}
	
	computeVelocities ()
	{
		if (this.layerPointsArray) {
			let pointCount = this.layerPointsArray.length / 2;
			
			if (pointCount > 0) {
				let velocities = [];
				let maxVelocity = LAYERVIEW_MINVELOCITYRANGE;
				let minVelocity = LAYERVIEW_MINVELOCITYRANGE;
				
				for (let pointIndex = 0; pointIndex < pointCount; pointIndex++) {
					let velocity = this.getPointVelocity (pointIndex);					
					if (velocity > 0.0) {						
						if (velocity > LAYERVIEW_MINVELOCITYRANGE) {

							if (maxVelocity < velocity)
								maxVelocity = velocity;
							
							if (minVelocity < LAYERVIEW_MINVELOCITYRANGE) {
								minVelocity = velocity;
							}
							if (minVelocity > velocity) {
								minVelocity = velocity;							
							}
							
						}
													
						velocities.push (velocity);									
					} else {
						velocities.push (0.0);
					}
					

				}
				
				if (maxVelocity < LAYERVIEW_MINVELOCITYRANGE)
					maxVelocity = LAYERVIEW_MINVELOCITYRANGE;
				if (minVelocity < LAYERVIEW_MINVELOCITYRANGE)
					minVelocity = LAYERVIEW_MINVELOCITYRANGE;
				
				this.layerPointsVelocities = velocities;
				this.layerPointsMaxVelocity = maxVelocity;
				this.layerPointsMinVelocity = minVelocity;
			
			}
			
			
		} else {
			this.layerPointsVelocities = null;
			this.layerPointsMaxVelocity = LAYERVIEW_MINVELOCITYRANGE;
			this.layerPointsMinVelocity = LAYERVIEW_MINVELOCITYRANGE;
		}
	}
	
	makeVelocityColors ()
	{
		this.layerPointsColorArray = null;

		if (this.layerPointsVelocities && this.layerPointsArray) {

			const velocityRange = (this.layerPointsMaxVelocity - this.layerPointsMinVelocity);
			if (velocityRange > LAYERVIEW_MINVELOCITYRANGE) {

				let pointCount = this.layerPointsArray.length / 2;
				let colors = [];
				
				for (let pointIndex = 0; pointIndex < pointCount; pointIndex++) {
					let velocity = this.layerPointsVelocities[pointIndex];
					let fraction = (velocity - this.layerPointsMinVelocity) / velocityRange;
					if (fraction >= 0.0) {
						if (fraction > 1.0)
							fraction = 1.0;
					} else {
						fraction = 0.0;
					}
					
					const hue = (1 - fraction) * 240 / 360;
					
					colors.push (this.hslToRgb (hue, 1.0, 0.5));				
					
				}
				
				this.layerPointsColorArray = colors;
			}

		}

	}

	computeChannelColumnRange (pointsChannelName, pointsColumnName)
	{
		if (this[pointsChannelName] && this[pointsChannelName][pointsColumnName]) {
			let dataArray = this[pointsChannelName][pointsColumnName];

			let maxValue = Number.NEGATIVE_INFINITY;
			let minValue = Number.POSITIVE_INFINITY;

			for (let i = 0; i < dataArray.length; i++) {
				let value = dataArray[i];
				if (value > maxValue) {
					maxValue = value;
				}
				if (value < minValue) {
					minValue = value;
				}
			}
			this[pointsChannelName][pointsColumnName].max = maxValue;
			this[pointsChannelName][pointsColumnName].min = minValue;
		}
	}

	makeLaserOnColors ()
	{		
		this.laserOnPointsColorArray = null;

		if (this.laser && this.laser.laseron && this.layerPointsArray) {

			let pointCount = this.layerPointsArray.length / 2;
			let colors = [];
			
			for (let pointIndex = 0; pointIndex < pointCount; pointIndex++) {

				let laseron = this.laser.laseron[pointIndex];

				// laseron = 0 => Blue
				// laseron = 1 => Red
				const hue = laseron * 240 / 360;
				
				colors.push (this.hslToRgb (hue, 1.0, 0.5));				
			}
			
			this.layerPointsColorArray = colors;
		}
		
	}
	
	makeLaserPowerColors ()
	{
		this.layerPointsColorArray = null;

		if (this.laser && this.laser.power && this.layerPointsArray) {
			const powerRange = (this.laser.power.max - this.laser.power.min);
			if (powerRange > 0) {

				let pointCount = this.laser.power.length;
				let colors = [];
				
				for (let pointIndex = 0; pointIndex < pointCount; pointIndex++) {
					let power = this.laser.power[pointIndex];
					let fraction = (power - this.laser.power.min) / powerRange;
					if (fraction >= 0.0) {
						if (fraction > 1.0)
							fraction = 1.0;
					} else {
						fraction = 0.0;
					}
					
					const hue = (fraction) * 240 / 360;
					colors.push (this.hslToRgb (hue, 1.0, 0.5));				
				}
				
				this.layerPointsColorArray = colors;
			}
		}
	}
	
	
	hslToRgb(h, s, l) {
		// Ensure h, s, l are in the range [0, 1]
		h = h % 1; // Wrap around if h is greater than 1
		s = Math.min(Math.max(s, 0), 1);
		l = Math.min(Math.max(l, 0), 1);

		if (s === 0) {
			// Achromatic (gray)
			const value = Math.round(l * 255);
			return [value, value, value];
		}

		const hueToRgb = (p, q, t) => {
			if (t < 0) t += 1;
			if (t > 1) t -= 1;
			if (t < 1 / 6) return p + (q - p) * 6 * t;
			if (t < 1 / 2) return q;
			if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
			return p;
		};

		const q = l < 0.5 ? l * (1 + s) : l + s - l * s;
		const p = 2 * l - q;

		const r = hueToRgb(p, q, h + 1 / 3);
		const g = hueToRgb(p, q, h);
		const b = hueToRgb(p, q, h - 1 / 3);

		return Math.round(r * 255) + Math.round(g * 255) * 256 + Math.round(b * 255) * 65536;
			
	}


	makeTimeColors ()
	{
		this.layerPointsColorArray = null;

		if (this.layerPointsVelocities && this.layerPointsArray) {

			let pointCount = this.layerPointsArray.length / 2;
			let colors = [];
			
			for (let pointIndex = 0; pointIndex < pointCount; pointIndex++) {

				let t = pointIndex / pointCount;
				
				const hue = (1 - t) * 240 / 360;
				
				colors.push (this.hslToRgb (hue, 1.0, 0.5));				
				
			}
			
			this.layerPointsColorArray = colors;

		}

	}

	
    loadPoints(pointsArray) {
		this.layerPointsArray = pointsArray;
		this.layerPointsColorArray = null;
		this.layerPointsVelocities = null;
		this.layerPointsMaxVelocity = LAYERVIEW_MINVELOCITYRANGE;
		this.layerPointsMinVelocity = LAYERVIEW_MINVELOCITYRANGE;
		
		this.computeVelocities ();		
	}

	loadPointsChannelData(pointsChannelName, pointsColumnName, pointsChannelDataArray) {
		
		// Ensure the channel-level object exists
  		if (!Object.prototype.hasOwnProperty.call(this, pointsChannelName)) {
    		this[pointsChannelName] = {};
  		}

  		// Assign the data array to the corresponding column
  		this[pointsChannelName][pointsColumnName] = pointsChannelDataArray;

		this.computeChannelColumnRange (pointsChannelName, pointsColumnName);
	}
	
	clearPoints ()
	{
		this.layerPointsArray = null;
		this.layerPointsColorArray = null;
		this.layerPointsVelocities = null;
		this.layerPointsMaxVelocity = LAYERVIEW_MINVELOCITYRANGE;
		this.layerPointsMinVelocity = LAYERVIEW_MINVELOCITYRANGE;
		
		this.computeVelocities ();		
		this.updateColors ();			
		this.updateLayerPoints ();		
	}
	
	clearPointsChannelData (pointsChannelName)
	{
		// Ensure the channel-level object exists
  		if (Object.prototype.hasOwnProperty.call(this, pointsChannelName)) {
    		this[pointsChannelName] = {};
  		}
	
		this.layerPointsColorArray = null;
	}

	updateColors ()
	{
		this.layerPointsColorArray = null;

		if (this.layerPointsMode == "time") {
			this.makeTimeColors ();
		} 

		if (this.layerPointsMode == "velocity") {
			this.makeVelocityColors ();
		} 

		if (this.layerPointsMode == "laseron") {
			this.makeLaserOnColors ();
		}

		if (this.layerPointsMode == "powerramp") {
			this.makeLaserPowerColors ();
		}
	}
	
	setColorMode (newColorMode) {
		this.layerPointsMode = newColorMode;
		this.updateColors ();
		this.updateLayerPoints ();
		this.updateTransform();
		this.RenderScene(true);		
	}
	
	pointDataIsAvailable ()
	{
		if (this.layerPointsArray) {
			return true;
		}
		
		return false;
	}
	
	updateLayerPoints () {
		if (!this.glInstance) 
			return;

		this.glInstance.removeElement("layerdata_points");
						
		if (this.layerPointsArray && this.pointsVisible) {
			let pointsToRender = this.layerPointsArray;
			let colorsToRender = this.layerPointsColorArray;
			this.filteredToOriginalIndexMap = null;

			if (!this.showLaserOffPoints && this.laser && this.laser.laseron) {
				let pointCount = this.layerPointsArray.length / 2;
				let filteredCoords = [];
				let filteredColors = colorsToRender ? [] : null;
				let indexMap = [];

				for (let i = 0; i < pointCount; i++) {
					if (this.laser.laseron[i] !== 0) {
						filteredCoords.push(this.layerPointsArray[i * 2], this.layerPointsArray[i * 2 + 1]);
						if (filteredColors && colorsToRender[i] !== undefined) {
							filteredColors.push(colorsToRender[i]);
						}
						indexMap.push(i);
					}
				}
				pointsToRender = new Float32Array(filteredCoords);
				colorsToRender = filteredColors;
				this.filteredToOriginalIndexMap = indexMap;
			}

			this.glInstance.add2DLocalizedPointsGeometry("layerdata_points", pointsToRender, 61, this.lineScaleLevel * 0.06, 0x00d0ff, colorsToRender, -200, -200, 2, 2, 400, 400);
			this.updateTransform ();
			this.RenderScene (true);
		}
	}

	resolvePointIndex (filteredIndex)
	{
		if (this.filteredToOriginalIndexMap && filteredIndex >= 0
			&& filteredIndex < this.filteredToOriginalIndexMap.length) {
			return this.filteredToOriginalIndexMap[filteredIndex];
		}
		return filteredIndex;
	}

	getPointPosition (pointIndex)
	{
		if (this.layerPointsArray) {
			if (pointIndex >= 0) {
				let arrayIndex = pointIndex * 2;
				if (arrayIndex + 1 < this.layerPointsArray.length) {
					let xPosition = this.layerPointsArray[arrayIndex];
					let yPosition = this.layerPointsArray[arrayIndex + 1];
					return { x: xPosition, y: yPosition }
				}
			}

		}
		
		return null;
					
	}

	getPointVelocity (pointIndex)
	{
		let dT = 1 / 100000;
		
		if (this.layerPointsArray) {
			if (pointIndex > 0) {
				let arrayIndex = pointIndex * 2;
				if (arrayIndex + 3 < this.layerPointsArray.length) {
					let vX = (this.layerPointsArray[arrayIndex + 2] - this.layerPointsArray[arrayIndex - 2]) / (2 * dT);
					let vY = (this.layerPointsArray[arrayIndex + 3] - this.layerPointsArray[arrayIndex - 1]) / (2 * dT);
					let velocity = Math.sqrt (vX * vX + vY * vY);
					return velocity;
				}
			}

		}
		
		return 0.0;
					
	}

	getPointAcceleration (pointIndex)
	{
		let dT = 1 / 100000; 
		
		if (this.layerPointsArray) {
			if (pointIndex > 0) {
				let arrayIndex = pointIndex * 2;
				if (arrayIndex + 3 < this.layerPointsArray.length) {
					let aX = (this.layerPointsArray[arrayIndex + 2] - 2 * this.layerPointsArray[arrayIndex] + this.layerPointsArray[arrayIndex - 2]) / (dT * dT);
					let aY = (this.layerPointsArray[arrayIndex + 3] - 2 * this.layerPointsArray[arrayIndex + 1] + this.layerPointsArray[arrayIndex - 1]) / (dT * dT);
					let accel = Math.sqrt (aX * aX + aY * aY);
					return { ax: aX,  ay: aY, a: accel }
				}
			}

		} 
		
		return null;
					
	}


	getPointJerk (pointIndex)
	{
		let dT = 1 / 100000;
		
		if (this.layerPointsArray) {
			if (pointIndex > 1) {
				let arrayIndex = pointIndex * 2;
				if (arrayIndex + 5 < this.layerPointsArray.length) {
					let aXPrev = (this.layerPointsArray[arrayIndex] - 2 * this.layerPointsArray[arrayIndex - 2] + this.layerPointsArray[arrayIndex - 4]) / (dT * dT);
					let aYPrev = (this.layerPointsArray[arrayIndex + 1] - 2 * this.layerPointsArray[arrayIndex - 1] + this.layerPointsArray[arrayIndex - 3]) / (dT * dT);
					let aXNext = (this.layerPointsArray[arrayIndex + 2] - 2 * this.layerPointsArray[arrayIndex] + this.layerPointsArray[arrayIndex - 2]) / (dT * dT);
					let aYNext = (this.layerPointsArray[arrayIndex + 3] - 2 * this.layerPointsArray[arrayIndex + 1] + this.layerPointsArray[arrayIndex - 1]) / (dT * dT);
					let jX = (aXNext - aXPrev) / (2 * dT);
					let jY = (aYNext - aYPrev) / (2 * dT);
					let jerk = Math.sqrt (jX * jX + jY * jY);
					return { jx: jX,  jy: jY, j: jerk }
				}
			}

		}
		
		return null;
					
	}

	getPointPower (pointIndex)
	{
		if (this.laser && this.laser.power) {
			if (pointIndex >= 0 && pointIndex < this.laser.power.length) {
				return this.laser.power[pointIndex];				
			}
		}
		return null;
	}

	updateLoadedLayer () {
		if (!this.glInstance) 
			return;
		
        this.glInstance.removeElement("layerdata_lines");
        // The highlighted line references indices into linesCoordinates, which are
        // about to be regenerated, so drop any stale highlight overlay.
        this.glInstance.removeElement("layerdata_highlight");
        this.highlightLineIndex = -1;
		
		if (this.layerSegmentsArray && this.toolpathVisible) {
				
			let segmentsArray = this.layerSegmentsArray;
			var segmentCount = segmentsArray.length;
			var segmentIndex;
			
			this.linesCoordinates = [];
			this.segmentProperties = [];
			
			let vertexcolors = [];

			for (segmentIndex = 0; segmentIndex < segmentCount; segmentIndex++) {
				var segment = segmentsArray[segmentIndex];
				let partUUID = (typeof segment.partuuid === "string") ? segment.partuuid.toLowerCase () : "";
				let partDisabled = this.layerDisabledParts.has (partUUID);
				let segmentColor = partDisabled ? LAYERVIEW_DISABLEDPARTCOLOR : segment.color;
				let segmentData = {
					type: segment.type,
					laserpower: segment.laserpower,
					laserspeed: segment.laserspeed,
					profilename: segment.profilename,
					partid: segment.partid,
					partuuid: partUUID,
					partname: this.layerPartNames.get (partUUID) || "",
					partdisabled: partDisabled,
					laserindex: segment.laserindex
				}

				if ((segment.type === "loop") || (segment.type === "polyline")) {
					var pointCount = segment.points.length;
					var pointIndex;
					
					var oldx = segment.points[0].x;
					var oldy = segment.points[0].y;

					for (pointIndex = 1; pointIndex < pointCount; pointIndex++) {
						var x = segment.points[pointIndex].x;
						var y = segment.points[pointIndex].y;

						if ((oldx === x) && (oldy === y)) {
							oldx = x;
							oldy = y;
							continue;
						}

						this.linesCoordinates.push(oldx, oldy, x, y);
						this.segmentProperties.push (segmentData);
						vertexcolors.push (segmentColor);

						oldx = x;
						oldy = y;
					}
				}

				if (segment.type === "hatch") {
					var lineCount = segment.points.length / 2;
					var lineIndex;

					for (lineIndex = 0; lineIndex < lineCount; lineIndex++) {
						var x1 = segment.points[lineIndex * 2].x;
						var y1 = segment.points[lineIndex * 2].y;
						var x2 = segment.points[lineIndex * 2 + 1].x;
						var y2 = segment.points[lineIndex * 2 + 1].y;

						if ((x1 === x2) && (y1 === y2)) {
							continue;
						}

						this.linesCoordinates.push(x1, y1, x2, y2);
						this.segmentProperties.push (segmentData);
						vertexcolors.push (segmentColor);
					}

				}
			}
			
			var layerLinesObject = this.glInstance.scene.getObjectByName("layerdata_lines");
			if (layerLinesObject)
				this.glInstance.scene.remove(layerLinesObject);
			
			
			let thickness = this.lineScaleLevel * 0.3;
			
			console.log ("rebulding layer preview with thickness: " + thickness);
			
			this.glInstance.add2DLineGeometry("layerdata_lines", this.linesCoordinates, 60, thickness, 0x000000, vertexcolors);
		}

		this.updateTransform();
		this.RenderScene(true);		

    }

    Drag(deltaX, deltaY) {

        // console.log("dX: " + deltaX + " dY: " + deltaY);

        this.transform.x = this.transform.x + deltaX;
        this.transform.y = this.transform.y + deltaY;

        this.updateTransform();

    }

    SetAbsoluteScaling(newScaling, centerx, centery) {

        if (!centerx) {
            centerx = this.twoinstance.width * 0.5;
        }

        if (!centery) {
            centery = this.twoinstance.height * 0.5;
        }

        this.ScaleRelative(newScaling / this.transform.scaling, centerx, centery);

    }

    ScaleRelative(factor, centerx, centery) {

        if (!centerx) {
            centerx = this.currentSize.viewPortWidth * 0.5;
        }

        if (!centery) {
            centery = this.currentSize.viewPortHeight * 0.5;
        }

        this.transform.x = this.transform.x - centerx;
        this.transform.y = this.transform.y - centery;

		// console.log("oldscaling: " + this.transform.scaling);

        var oldScaling = this.transform.scaling;
        this.transform.scaling = oldScaling * factor;
        if (this.transform.scaling < LAYERVIEW_MINSCALING)
            this.transform.scaling = LAYERVIEW_MINSCALING;

        if (this.transform.scaling > LAYERVIEW_MAXSCALING)
            this.transform.scaling = LAYERVIEW_MAXSCALING;

		// console.log("newscaling: " + this.transform.scaling);

        factor = this.transform.scaling / oldScaling;

        this.transform.x = Math.ceil(this.transform.x * factor + centerx);
        this.transform.y = Math.ceil(this.transform.y * factor + centery);

        this.updateTransform();
		
    }
	
	CenterOnRectangle (minx, miny, maxx, maxy)
	{
		var sizex = maxx - minx;
		var sizey = maxy - miny;
		
		if ((this.currentSize.viewPortWidth > 0) && (this.currentSize.viewPortHeight > 0)) {
			if ((sizex > 0) && (sizey > 0)) {
				
				var scalingx = (this.currentSize.viewPortWidth / sizex);
				var scalingy = (this.currentSize.viewPortHeight / sizey);
				
				var newcenterx = (maxx + minx) * 0.5;
				var newcentery = (maxy + miny) * 0.5;
				
				var viewcenterx = this.currentSize.viewPortWidth * 0.5;
				var viewcentery = this.currentSize.viewPortHeight * 0.5;
								
				this.transform.x = viewcenterx - this.transform.scaling * newcenterx;
				this.transform.y = viewcentery + this.transform.scaling * newcentery;
								
				var newScaling;
				if (scalingx > scalingy) {
					newScaling = scalingy;
				} else {
					newScaling = scalingx;
				}
				
				this.SetAbsoluteScaling (newScaling, viewcenterx, viewcentery);
			
			}
		}
		
		
	}
	
    RenderScene (forceRender = false) 
	{
		if (forceRender) {
			this.renderNeedsUpdate = true;
		}
		
		if (this.renderNeedsUpdate && this.glInstance) {
			this.glInstance.renderScene ();
			this.renderNeedsUpdate = false;
		}			
		
	} 
	
	SetBuildPlateSVG (url) {
		if (this.glInstance) {
			if (url) {
				this.svgImage = this.glInstance.addSVGImage("buildplate", url, 35, true, true);
				this.updateTransform ();
			} else {
				this.glInstance.removeElement ("buildplate");
			}
		}
	} 
		
	setOrigin (x, y)
	{
		if (isNaN (x) || isNaN (y))
			return;
		
		this.origin.x = x;
		this.origin.y = y;
	}

	// Converts a viewport pixel coordinate (relative to the canvas, in CSS pixels)
	// into machine/build-plate coordinates in millimeters. The build plate places
	// machine (0,0) at (transform.x, transform.y) with the Y axis inverted, so this
	// is the exact inverse of that mapping. Returns null if no valid scaling exists.
	/**
	 * @param {number} screenX
	 * @param {number} screenY
	 */
	screenToMachine (screenX, screenY)
	{
		if (!this.transform)
			return null;

		const scaling = this.transform.scaling;
		if (!(scaling > 0) || isNaN (screenX) || isNaN (screenY))
			return null;

		return {
			x: (screenX - this.transform.x) / scaling,
			y: (this.transform.y - screenY) / scaling
		};
	}

	// Maps a raw toolpath point (as stored in linesCoordinates, i.e. before the
	// placement/rotation/origin transform) into machine/build-plate coordinates
	// in millimeters. This is the exact forward of screenToMachine so that a
	// cursor position and a segment endpoint can be compared in the same frame.
	/**
	 * @param {number} toolpathX
	 * @param {number} toolpathY
	 */
	toolpathPointToMachine (toolpathX, toolpathY)
	{
		const angleInRadians = this.transformAngleDegrees * Math.PI / 180.0;
		const cosine = Math.cos (angleInRadians);
		const sine = Math.sin (angleInRadians);
		const placement = this.transformToolpathPoint (0, 0);

		return {
			x: this.origin.x + placement.x + (toolpathX * cosine + toolpathY * sine),
			y: this.origin.y + placement.y + (-toolpathX * sine + toolpathY * cosine)
		};
	}

	// Inverse of screenToMachine: machine coordinates (mm) to viewport pixels.
	/**
	 * @param {number} machineX
	 * @param {number} machineY
	 */
	machineToScreen (machineX, machineY)
	{
		if (!this.transform)
			return null;

		return {
			x: this.transform.x + machineX * this.transform.scaling,
			y: this.transform.y - machineY * this.transform.scaling
		};
	}

	// Returns one axis-aligned bounding box per part of the loaded layer, in
	// machine coordinates (mm). Parts are identified by their build item UUID
	// (lower case), which is stable across layers, unlike the layer-local partid.
	// Segments without a UUID (older servers) are grouped by partid instead;
	// those boxes have an empty uuid.
	getPartBoundingBoxes ()
	{
		if (!this.layerSegmentsArray || !this.toolpathVisible)
			return [];

		const cacheKey = [this.transformAngleDegrees, this.rotationCenter.x, this.rotationCenter.y,
			this.translation.x, this.translation.y, this.origin.x, this.origin.y].join ("|");
		if (this.partBoundsCache && (this.partBoundsCacheKey === cacheKey))
			return this.partBoundsCache;

		const angleInRadians = this.transformAngleDegrees * Math.PI / 180.0;
		const cosine = Math.cos (angleInRadians);
		const sine = Math.sin (angleInRadians);
		const placement = this.transformToolpathPoint (0, 0);
		const offsetX = this.origin.x + placement.x;
		const offsetY = this.origin.y + placement.y;

		const boxesByKey = new Map ();
		for (const segment of this.layerSegmentsArray) {
			if (!segment || !Array.isArray (segment.points))
				continue;

			let partUUID = (typeof segment.partuuid === "string") ? segment.partuuid.toLowerCase () : "";
			if (partUUID === "00000000-0000-0000-0000-000000000000")
				partUUID = "";

			let key;
			let name;
			if (partUUID !== "") {
				key = partUUID;
				name = this.layerPartNames.get (partUUID) || "";
			} else if (Number.isInteger (segment.partid)) {
				key = "partid:" + segment.partid;
				name = "Part " + segment.partid;
			} else {
				continue;
			}

			let box = boxesByKey.get (key);
			if (!box) {
				box = {
					key: key,
					uuid: partUUID,
					name: name,
					disabled: (partUUID !== "") && this.layerDisabledParts.has (partUUID),
					minx: Infinity, miny: Infinity, maxx: -Infinity, maxy: -Infinity
				};
				boxesByKey.set (key, box);
			}

			for (const point of segment.points) {
				const machineX = offsetX + (point.x * cosine + point.y * sine);
				const machineY = offsetY + (-point.x * sine + point.y * cosine);
				if (machineX < box.minx) box.minx = machineX;
				if (machineX > box.maxx) box.maxx = machineX;
				if (machineY < box.miny) box.miny = machineY;
				if (machineY > box.maxy) box.maxy = machineY;
			}
		}

		this.partBoundsCache = Array.from (boxesByKey.values ()).filter ((box) => (box.minx <= box.maxx) && (box.miny <= box.maxy));
		this.partBoundsCacheKey = cacheKey;
		return this.partBoundsCache;
	}

	// Squared distance from point (px,py) to the line segment (ax,ay)-(bx,by).
	static pointSegmentDistanceSq (px, py, ax, ay, bx, by)
	{
		const dx = bx - ax;
		const dy = by - ay;
		const lengthSq = dx * dx + dy * dy;

		let t = 0.0;
		if (lengthSq > 0.0) {
			t = ((px - ax) * dx + (py - ay) * dy) / lengthSq;
			if (t < 0.0) t = 0.0;
			if (t > 1.0) t = 1.0;
		}

		const cx = ax + t * dx;
		const cy = ay + t * dy;
		const ex = px - cx;
		const ey = py - cy;
		return ex * ex + ey * ey;
	}

	// Finds the toolpath line segment closest to the given machine coordinate
	// (mm). Returns the associated segment properties (laser power/speed, profile
	// name, part/laser index, type) when within toleranceMM, otherwise null.
	/**
	 * @param {number} machineX
	 * @param {number} machineY
	 * @param {number} toleranceMM
	 */
	pickSegmentAtMachinePoint (machineX, machineY, toleranceMM)
	{
		if (!this.linesCoordinates || !this.segmentProperties)
			return null;

		const coords = this.linesCoordinates;
		const lineCount = Math.floor (coords.length / 4);
		if (lineCount <= 0)
			return null;

		const angleInRadians = this.transformAngleDegrees * Math.PI / 180.0;
		const cosine = Math.cos (angleInRadians);
		const sine = Math.sin (angleInRadians);
		const placement = this.transformToolpathPoint (0, 0);
		const baseX = this.origin.x + placement.x;
		const baseY = this.origin.y + placement.y;

		let bestDistanceSq = toleranceMM * toleranceMM;
		let bestIndex = -1;

		for (let lineIndex = 0; lineIndex < lineCount; lineIndex++) {
			const arrayIndex = lineIndex * 4;
			const px1 = coords[arrayIndex];
			const py1 = coords[arrayIndex + 1];
			const px2 = coords[arrayIndex + 2];
			const py2 = coords[arrayIndex + 3];

			const ax = baseX + (px1 * cosine + py1 * sine);
			const ay = baseY + (-px1 * sine + py1 * cosine);
			const bx = baseX + (px2 * cosine + py2 * sine);
			const by = baseY + (-px2 * sine + py2 * cosine);

			const distanceSq = LayerViewImpl.pointSegmentDistanceSq (machineX, machineY, ax, ay, bx, by);
			if (distanceSq < bestDistanceSq) {
				bestDistanceSq = distanceSq;
				bestIndex = lineIndex;
			}
		}

		if (bestIndex >= 0 && bestIndex < this.segmentProperties.length)
			return { ...this.segmentProperties[bestIndex], lineIndex: bestIndex };

		return null;
	}

	// Draws (or removes, when lineIndex < 0) a highlight overlay on top of a
	// single toolpath line so the segment currently under the cursor stands out.
	// The overlay shares the toolpath transform applied in updateTransform().
	/**
	 * @param {number} lineIndex
	 */
	setHighlightLine (lineIndex)
	{
		if (!this.glInstance)
			return;

		this.glInstance.removeElement ("layerdata_highlight");
		this.highlightLineIndex = -1;

		if (lineIndex === undefined || lineIndex === null || lineIndex < 0 || !this.linesCoordinates) {
			this.RenderScene (true);
			return;
		}

		const arrayIndex = lineIndex * 4;
		if (arrayIndex + 3 >= this.linesCoordinates.length) {
			this.RenderScene (true);
			return;
		}

		const highlightCoordinates = [
			this.linesCoordinates[arrayIndex],
			this.linesCoordinates[arrayIndex + 1],
			this.linesCoordinates[arrayIndex + 2],
			this.linesCoordinates[arrayIndex + 3]
		];

		// Noticeably thicker than the regular preview lines (lineScaleLevel * 0.3)
		// and drawn at a higher z so it sits above lines (60) and points (61).
		const highlightThickness = this.lineScaleLevel * 1.1;
		const highlightColor = 0x00e5ff;

		this.glInstance.add2DLineGeometry ("layerdata_highlight", highlightCoordinates, 63, highlightThickness, highlightColor, [highlightColor]);
		this.highlightLineIndex = lineIndex;

		this.updateTransform ();
		this.RenderScene (true);
	}

	clearHighlight ()
	{
		this.setHighlightLine (-1);
	}

	// Convenience picker that works directly in viewport pixel space. The pixel
	// tolerance is converted to millimeters using the current zoom so the hover
	// hit area stays constant on screen regardless of zoom level.
	/**
	 * @param {number} screenX
	 * @param {number} screenY
	 * @param {number} [pixelTolerance]
	 */
	pickSegmentAtScreenPoint (screenX, screenY, pixelTolerance = 6)
	{
		const machine = this.screenToMachine (screenX, screenY);
		if (!machine)
			return null;

		const scaling = this.transform.scaling;
		if (!(scaling > 0))
			return null;

		return this.pickSegmentAtMachinePoint (machine.x, machine.y, pixelTolerance / scaling);
	}

	setTransformAngle (angleInDegrees)
	{
		this.setCoordinateTransform(angleInDegrees, this.rotationCenter.x, this.rotationCenter.y, this.translation.x, this.translation.y);
	}

	setCoordinateTransform (angleInDegrees, rotationCenterX, rotationCenterY, translationX, translationY)
	{
		if (isNaN(angleInDegrees) || isNaN(rotationCenterX) || isNaN(rotationCenterY) || isNaN(translationX) || isNaN(translationY))
			return;

		this.transformAngleDegrees = Number(angleInDegrees);
		this.rotationCenter.x = Number(rotationCenterX);
		this.rotationCenter.y = Number(rotationCenterY);
		this.translation.x = Number(translationX);
		this.translation.y = Number(translationY);
		this.updateTransform();
	}
}



export default LayerViewImpl;
