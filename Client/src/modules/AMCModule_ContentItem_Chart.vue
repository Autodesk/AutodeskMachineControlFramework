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
									
			this.moduleitem.onChartDataUpdated = this.updateChartData;
			
		},
		
		methods: {
		
			updateChartData ()
			{
				if (this.moduleitem) {
				
					this.$refs.apexChart.updateSeries([{
						data: this.moduleitem.chartData
					}]); 				
					
				}
				
			}
		
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