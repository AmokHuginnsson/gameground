all: galaxy.jar

*.class: *.java
	javac -Xlint -cp jdom.jar:swixml.jar *.java

galaxy.jar: *.class res/galaxy.xml res/*.xpm
	jar cf galaxy.jar res *.class

run: galaxy.jar
	java -Djava.security.manager -Djava.security.policy=galaxy.policy -cp jdom.jar:swixml.jar:galaxy.jar Galaxy

dist:
	$(MAKE)
	@mv galaxy.jar galaxy.jar.keep
	@$(MAKE) clean
	@mv galaxy.jar.keep galaxy.jar

clean:
	cln .
	/bin/rm -f galaxy.jar *.class
