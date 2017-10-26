if HAVE_DOXYGEN
doc.pdf:
	$(DOXYGEN) doxyconfig
	+$(MAKE) -C doc/latex
	mv doc/latex/refman.pdf ./doc.pdf
	rm -rf ./doc

all-local: doc.pdf
clean-local:
	rm -f doc.pdf
endif
