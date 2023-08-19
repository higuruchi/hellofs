SUBDIRS = hellofs

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

clean: RULE = clean
install: RULE = install
uninstall: RULE = uninstall
load: RULE = load

all clean install uninstall load: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(RULE)
