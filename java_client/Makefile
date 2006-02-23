all: galaxy.jar

*.class: *.java
	javac -Xlint -cp jdom.jar:swixml.jar *.java

build_dir:
	mkdir -p build

galaxy.jar: build_dir *.class res/galaxy.xml res/*.xpm
	cd build; \
	/bin/rm -rf *; \
	jar xf ../jdom.jar; \
	jar xf ../swixml.jar; \
	cp -a ../*.class ../res .; \
	jar cf galaxy.jar res org *.class;

run: galaxy.jar
	java -Djava.security.manager -Djava.security.policy=galaxy.policy -cp build/galaxy.jar Galaxy

dist:
	$(MAKE)
	@mv build/galaxy.jar galaxy.jar.keep
	@$(MAKE) clean
	@mv galaxy.jar.keep galaxy.jar

clean:
	cln .
	/bin/rm -rf build galaxy.jar *.class
