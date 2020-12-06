default: lab3b

.ONESHELL:
lab3b: lab3b.py
	@touch lab3b
	@chmod +x lab3b
	@echo '#!/usr/bin/bash' >> lab3b
	@echo './lab3b.py $$1' >> lab3b

dist: lab3b.py Makefile README
	tar -czvf lab3b-305101238.tar.gz lab3b.py Makefile README

clean:
	rm -f lab3b lab3b-305101238.tar.gz
