all: gameground.jar

*.class: *.java
	javac -Xlint -cp jdom.jar:swixml.jar *.java

build_dir:
	mkdir -p build

gameground.jar: build_dir *.class res/gameground.xml res/*.xpm
	cd build; \
	/bin/rm -rf *; \
	jar xf ../jdom.jar; \
	jar xf ../swixml.jar; \
	cp -a ../*.class ../res .; \
	jar cf gameground.jar res org *.class;

run: gameground.jar
	java -Djava.security.manager -Djava.security.policy=gameground.policy -cp build/gameground.jar Galaxy

dist:
	$(MAKE)
	@mv build/gameground.jar gameground.jar.keep
	@$(MAKE) clean
	@mv gameground.jar.keep gameground.jar

clean:
	cln .
	/bin/rm -rf build *.class
