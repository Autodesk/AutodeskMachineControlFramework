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
 
 
const GRAPHIC_ZLEVEL_DEFAULT = 50;
const GRAPHIC_ZLEVEL_MIN = 1;
const GRAPHIC_ZLEVEL_MAX = 100;

const GRAPHIC_MINSCALING = 0.5;
const GRAPHIC_MAXSCALING = 125.0;

const GRAPHIC_GRIDZVALUE = 1;

class GraphicImpl {

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
		
		this.renderNeedsUpdate = true;
		this.graphicElements = new Set();

		this.updateTransform ();

    }

    updateSize(width, height) {
        if ((width > 0) && (height > 0)) {

            this.currentSize.viewPortWidth = width;
            this.currentSize.viewPortHeight = height;

            if (this.glInstance) {
                this.glInstance.resizeTo(width, height);
                this.glInstance.setup2DView(width, height, 0.1, 100);
            }

            let newWidth = width * 2 + 50;
            let newHeight = height * 2 + 50;

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

        let gridObject = this.glInstance.scene.getObjectByName("grid");
        if (gridObject)
            this.glInstance.scene.remove(gridObject);

        this.glInstance.add2DGridGeometry("grid", width, height, GRAPHIC_GRIDZVALUE, 5);
		this.updateTransform ();
		
		this.renderNeedsUpdate = true;
		

    }

    updateTransform() {

        if (!this.glInstance)
            return;

        let gridgeometry = this.glInstance.findElement("grid");
        if (gridgeometry) {
            let gridScale = this.transform.scaling;

            if (gridScale < 0.5) {
                gridScale = gridScale * 5.0;
            }

            if (gridScale > 2.5) {
                gridScale = gridScale / 5.0;
            }

            if (gridScale > 2.5) {
                gridScale = gridScale / 5.0;
            }

            if (gridScale > 2.5) {
                gridScale = gridScale / 5.0;
            }

            let fullGridSize = gridScale * 25.0 * 5;
            let gridTranslationX = this.transform.x - Math.ceil((this.transform.x / fullGridSize)) * fullGridSize;
            let gridTranslationY = this.transform.y - Math.ceil((this.transform.y / fullGridSize)) * fullGridSize;

            gridgeometry.setPositionXY(gridTranslationX, gridTranslationY);
            gridgeometry.setScaleXY(gridScale, gridScale);
        }
		
		for (const name of this.graphicElements) {
			let geometryElement = this.glInstance.findElement(name);
			if (geometryElement) {
				geometryElement.setPositionXY(this.transform.x, this.transform.y);
				geometryElement.setScaleXY(this.transform.scaling,  this.transform.scaling);
			}
		}
		
		this.renderNeedsUpdate = true;

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

		console.log("oldscaling: " + this.transform.scaling);

        let oldScaling = this.transform.scaling;
        this.transform.scaling = oldScaling * factor;
        if (this.transform.scaling < GRAPHIC_MINSCALING)
            this.transform.scaling = GRAPHIC_MINSCALING;

        if (this.transform.scaling > GRAPHIC_MAXSCALING)
            this.transform.scaling = GRAPHIC_MAXSCALING;

		console.log("newscaling: " + this.transform.scaling);

        factor = this.transform.scaling / oldScaling;

        this.transform.x = Math.ceil(this.transform.x * factor + centerx);
        this.transform.y = Math.ceil(this.transform.y * factor + centery);

        this.updateTransform();
		
    }
		
	
	CenterOnRectangle (minx, miny, maxx, maxy)
	{
		let sizex = maxx - minx;
		let sizey = maxy - miny;
		
		if ((this.currentSize.viewPortWidth > 0) && (this.currentSize.viewPortHeight > 0)) {
			if ((sizex > 0) && (sizey > 0)) {
				
				let scalingx = (this.currentSize.viewPortWidth / sizex);
				let scalingy = (this.currentSize.viewPortHeight / sizey);
				
				let newcenterx = (maxx + minx) * 0.5;
				let newcentery = (maxy + miny) * 0.5;
				
				let viewcenterx = this.currentSize.viewPortWidth * 0.5;
				let viewcentery = this.currentSize.viewPortHeight * 0.5;
								
				this.transform.x = viewcenterx - this.transform.scaling * newcenterx;
				this.transform.y = viewcentery - this.transform.scaling * newcentery;
								
				let newScaling;
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
	

	AddSVGElement (url, name, zLevel) {
		
		if (!name)
			return;
								
		if (!zLevel)
			zLevel = GRAPHIC_ZLEVEL_DEFAULT;
		if (zLevel < GRAPHIC_ZLEVEL_MIN)
			zLevel = GRAPHIC_ZLEVEL_MIN;
		if (zLevel > GRAPHIC_ZLEVEL_MAX)
			zLevel = GRAPHIC_ZLEVEL_MAX;
		
		if (this.glInstance) {
			if (url) {
				this.glInstance.addSVGImage(name, url.toString (), zLevel, true, true);
				this.graphicElements.add (name);
				
			} else {
				this.glInstance.removeElement (name);
				this.graphicElements.delete (name);
			}
			
			
		}
	}
	
	BeginUpdate ()
	{
		this.graphicElements.clear ();
	}
	
	
	EndUpdate ()
	{
	}

}



export default GraphicImpl;
