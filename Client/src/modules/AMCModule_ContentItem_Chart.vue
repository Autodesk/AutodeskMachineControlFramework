<!--

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

!-->

<template>

<div v-if="(moduleitem.type=='chart')">  	
	 <VueApexCharts width="500" type="line" ref="apexChart" :options="options" :series="series"></VueApexCharts>
</div>

</template>

<script>
	import VueApexCharts from 'vue-apexcharts';
	
	export default {
	  props: ["Application", "moduleitem"],
	  components: {
		VueApexCharts
	  },
	  
		mounted() {
			
			
			let normalizedUUID = "d20b8598-1d4d-4cb8-a594-2f16abbae13d";
			
			this.Application.axiosGetArrayBufferRequest("/ui/chart/" + normalizedUUID)
				.then(responseData => {
					var floatView = new Float32Array(responseData.data);
					let dataLength = floatView.length;
					let pointCount = dataLength / 2;
										
					this.newData = [];
					for (let index = 0; index < pointCount; index++) {
						let xvalue = floatView[index * 2];
						let yvalue = floatView[index * 2 + 1];
						this.newData.push ([xvalue, yvalue]);
					}
					
					this.$refs.apexChart.updateSeries([{
						data: this.newData
					}]); 
					
				})
				.catch(err => {
					if (err.response) {
						console.log (err.response);
					} else {
						console.log ("fatal error while retrieving chart data " + normalizedUUID);
					}
				});			
			
		},
	  
	   data: () => ({
		options: {
        chart: {
          id: 'vuechart-example'
        },
  yaxis: [
    {
      axisTicks: {
        show: true
      },
      axisBorder: {
        show: true,		
      },
      labels: {
        style: {
          
        },
		formatter: function (val) {
			return (val).toFixed(2);
		},
      },
      title: {
        style: {
          
        }
      }
    }
  ],
		
		xaxis: {			
			type: "numeric",
			  labels: {
				formatter: function (value) {
					return (value / 1000.0).toFixed(3) + "s";
				}
			  }
		}
		
      },
	  
  newData: [],
	  
  series: [
    {
      name: "Series 1",
      data: []
    }
	
  ],
	  


	  })
	};
	
</script>