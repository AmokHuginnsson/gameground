JAVA=$(shell find . -maxdepth 1 -name '*.java'|sort)
CLASS=$(patsubst ./%.java,./build/%.class,$(JAVA))
RESOURCES=$(shell find res -maxdepth 1 ! -name .svn)
RES_DEST=$(patsubst %,./build/%,$(RESOURCES))
ENVIRON=./build/META-INF/info.xml
TARGET=./build/gameground.jar

.PHONY: all run dist clean

all: $(TARGET)

build/%.class: %.java
	@echo -n "gcc";
	javac -Xlint -sourcepath . -d build -cp .:jdom.jar:swixml.jar $(<)

build/res:
	mkdir -p $(@)

build/res/%: res/%
	cp -au $(<) $(@)

$(ENVIRON): jdom.jar swixml.jar
	mkdir -p build ; \
	cd build; \
	/bin/rm -rf *; \
	jar xf ../jdom.jar; \
	jar xf ../swixml.jar; \
	touch META-INF/info.xml

$(TARGET): $(ENVIRON) $(CLASS) $(RES_DEST)
	cd build; \
	ls -1 *.class > class.list; \
	jar cf ../$(TARGET) res org *.class class.list;

run: $(TARGET)
	java -Djava.security.manager -Djava.security.policy=gameground.policy -cp build/gameground.jar GameGround

dist:
	$(MAKE)
	@mv build/gameground.jar gameground.jar.keep
	@$(MAKE) clean
	@mv gameground.jar.keep gameground.jar

clean:
	cln .
	/bin/rm -rf build *.class
