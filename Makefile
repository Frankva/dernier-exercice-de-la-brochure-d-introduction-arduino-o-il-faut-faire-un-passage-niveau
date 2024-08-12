tags: 13.cpp
	ctags -R .
	awk '{gsub(/cpp/, "ino"); print > FILENAME}' $@
	rm 13.cpp
13.cpp: 13.ino
	cp 13.ino 13.cpp
