{
	"targets": [
		{
			"target_name": "dns_sd",
			"sources": [
				"dns_sd/dns_sd.cc",
				"dns_sd/dns_service_ref_wrap.cc"
			], 
			"include_dirs" : [
				"<!(node -e \"require('nan')\")"
			]
		}
	]
}