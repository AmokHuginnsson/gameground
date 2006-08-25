directory .:~/src/yaal:~/src/yaal/hcore:~/src/yaal/hconsole:~/src/yaal/hdata:~/src/yaal/tools
set solib-search-path ~/lib
set auto-solib-add on
set history filename .gdbhistory
set history save on
tty /dev/tty11
define go
	run
	source .breaks
end
file ../1exec -readnow -mapped
symbol-file ../1exec -readnow -mapped
share
break main
set language c++
core-file ./core
#run
