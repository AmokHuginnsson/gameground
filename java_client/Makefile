all: galaxy.jar

galaxy.jar: Galaxy.java
	javac -Xlint Galaxy.java
	jar cf galaxy.jar *.class

run: galaxy.jar
	java -cp galaxy.jar Galaxy

dist:
	$(MAKE)
	@mv galaxy.jar galaxy.jar.keep
	@$(MAKE) clean
	@mv galaxy.jar.keep galaxy.jar

clean:
	cln .
	/bin/rm -f galaxy.jar *.class
