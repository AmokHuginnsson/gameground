"use strict"

class GomokuConfigurator {
	static get TAG() { return ( Gomoku.TAG + "-configurator" ) }
	get_configuration_string() { return ( "" ) }
}
Vue.component( GomokuConfigurator.TAG, { template: '<div></div>' } )

