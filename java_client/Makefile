JAVA=$(shell find . -maxdepth 1 -name '*.java')
CLASS=$(patsubst %.java,%.class,$(JAVA))

all: gameground.jar

%.class: %.java
	@echo -n "gcc";
	javac -Xlint -sourcepath . -cp .:jdom.jar:swixml.jar $(<)

build_dir:
	mkdir -p build

gameground.jar: build_dir $(CLASS) res/gameground.xml res/*.xpm
	cd build; \
	/bin/rm -rf *; \
	jar xf ../jdom.jar; \
	jar xf ../swixml.jar; \
	cp -a ../*.class ../res .; \
	jar cf gameground.jar res org *.class;

run: gameground.jar
	java -Djava.security.manager -Djava.security.policy=gameground.policy -cp build/gameground.jar GameGround

dist:
	$(MAKE)
	@mv build/gameground.jar gameground.jar.keep
	@$(MAKE) clean
	@mv gameground.jar.keep gameground.jar

clean:
	cln .
	/bin/rm -rf build *.class
