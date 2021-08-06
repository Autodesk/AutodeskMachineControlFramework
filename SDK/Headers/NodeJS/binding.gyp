{
	"targets": [
		{
			"target_name": "libamcf_nodeaddon",
			"sources": [ "libamcf_nodeaddon.cc", "libamcf_nodewrapper.cc", "libamcf_dynamic.cc" ],
			"cflags": [ "-fexceptions " ],
			"cflags_cc": [ "-fexceptions " ],
			"msvs_settings": {
				"VCCLCompilerTool": { "ExceptionHandling": 1 }
			},
			"conditions": [
				["OS=='win'", {	"defines": [ "_HAS_EXCEPTIONS=1" ] }]
			]
		}
	]
}

