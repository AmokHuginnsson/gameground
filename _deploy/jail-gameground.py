doNotCopy=["/home"]
forceCopy=["/dev/urandom",
		"/dev/zero",
		"/dev/null",
		"/usr/bin/env",
		"/lib/ld-linux.so.2",
		"/lib/ld-linux-x86-64.so.2",
		"/lib/ld-2.8.90.so",
		"/usr/lib/aspell/en.multi",
		"/usr/lib/aspell/en-wo_accents.multi",
		"/usr/lib/aspell/en-common.rws",
		"/usr/lib/aspell/en.dat",
		"/usr/lib/aspell/iso-8859-1.cset",
		"/usr/lib/aspell/iso-8859-1.cmap",
		"/usr/lib/aspell/en_phonet.dat",
		"/usr/lib/aspell/en_affix.dat",
		"/usr/lib/aspell/en-wo_accents-only.rws",
		"/usr/lib/aspell/standard.kbd",
		"/usr/lib/aspell/pl.multi",
		"/usr/lib/aspell/pl.rws",
		"/usr/lib/aspell/pl.dat",
		"/usr/lib/aspell/iso-8859-2.cset",
		"/usr/lib/aspell/iso-8859-2.cmap",
		"/usr/lib/aspell/pl_affix.dat",
		"/lib/tfurca-makejail-to-pizda"]
chroot="/home/gameground"
testCommandsInsideJail=["/bin/launchtool -u gameground --chdir='/nest' -t gameground-install \". /nest/set-limits.sh; /usr/bin/env HOME=/nest NEST=/ LD_LIBRARY_PATH=/nest/lib /nest/1exec\""]
processNames=["1exec"]
cleanJailFirst=0
sleepAfterStartCommand=1
users=["gameground"]
groups=["service"]

keepStraceOutputs=1

