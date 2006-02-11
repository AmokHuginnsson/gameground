all: galaxy.jar

galaxy.jar: *.java
	javac -Xlint -cp jdom.jar:swixml.jar *.java
	jar cf galaxy.jar galaxy.xml *.class

run: galaxy.jar
	java -Djava.security.manager -cp jdom.jar:swixml.jar:galaxy.jar Galaxy

dist:
	$(MAKE)
	@mv galaxy.jar galaxy.jar.keep
	@$(MAKE) clean
	@mv galaxy.jar.keep galaxy.jar

clean:
	cln .
	/bin/rm -f galaxy.jar *.class
