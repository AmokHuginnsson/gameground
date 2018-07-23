"use strict"

class GoConfigurator {
	static get TAG() { return ( Go.TAG + "-configurator" ) }
	get_configuration_string() { return ( "" ) }
}
Vue.component( GoConfigurator.TAG, { template: '<div></div>' } )

