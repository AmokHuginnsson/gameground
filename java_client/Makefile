JAVA=$(shell find . -maxdepth 1 -name '*.java'|sort)
CLASS=$(patsubst ./%.java,./build/%.class,$(JAVA))
RESOURCES=$(shell find res -maxdepth 1 ! -name .svn -a ! -name '*~' -a ! -name '.*.swp')
RES_DEST=$(patsubst %,./build/%,$(RESOURCES))
ENVIRON=./build/META-INF/info.xml
TARGET=./build/gameground.jar

.PHONY: all run debug test test2 dist clean

all: $(TARGET)

build/%.class: %.java
	@echo -n "gcc";
	javac -g -Xlint -Werror -sourcepath . -d build -cp .:lib/commons-cli-1.1.jar:lib/jdom.jar:lib/swixml.jar $(<)

build/res:
	mkdir -p $(@)

build/res/%: res/%
	cp -au $(<) $(@)

$(ENVIRON): lib/jdom.jar lib/swixml.jar lib/commons-cli-1.1.jar lib/forms-1.1.0.jar
	mkdir -p build ; \
	cd build; \
	/bin/rm -rf *; \
	jar xf ../lib/commons-cli-1.1.jar; \
	jar xf ../lib/jdom.jar; \
	jar xf ../lib/forms-1.1.0.jar; \
	jar xf ../lib/swixml.jar; \
	touch META-INF/info.xml

$(TARGET): $(ENVIRON) $(CLASS) $(RES_DEST)
	cd build; \
	ls -1 *.class > class.list; \
	jar cfm ../$(TARGET) ../manifest.mf res org com *.class class.list;

run: $(TARGET)
	java -Dcom.sun.management.jmxremote -Djava.security.manager -Djava.security.policy=gameground.policy -ea -cp build/gameground.jar GameGround --host codestation.org --login Amok

debug: $(TARGET)
	java -Xdebug -Xrunjdwp:transport=dt_socket,address=127.0.0.1:8008,server=y,suspend=n -Dcom.sun.management.jmxremote -Djava.security.manager -Djava.security.policy=gameground.policy -ea -cp build/gameground.jar GameGround

test: $(TARGET)
	java -Dcom.sun.management.jmxremote -Djava.security.manager -Djava.security.policy=gameground.policy -ea -cp build/gameground.jar GameGround

test2: $(TARGET)
	java -Dcom.sun.management.jmxremote -Djava.security.manager -Djava.security.policy=gameground.policy -ea -cp build/gameground.jar GameGround --login Ktos

test3: $(TARGET)
	java -Dcom.sun.management.jmxremote -Djava.security.manager -Djava.security.policy=gameground.policy -ea -cp build/gameground.jar GameGround --login User --password ""

dist:
	$(MAKE)
	@mv build/gameground.jar gameground.jar.keep
	@$(MAKE) clean
	@mv gameground.jar.keep gameground.jar

deploy: clean $(TARGET)
	@echo "Enter password for your keystore:" ; \
	stty -echo; read PASS; stty echo; echo "Signing jar ... " ; \
	jarsigner -keystore codestation.jks -storepass "$${PASS}" ./build/gameground.jar codestation ; \
	echo "Uploading jar ... " ; \
	scp build/gameground.jar codestation.org:src/cellar/web/data/codestation/jar/ ; \
	echo "Done."

clean:
	cln .
	/bin/rm -rf build *.class

ala:
