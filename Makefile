all:
	gcc `pkg-config --cflags gtk+-3.0` -o TypingTest main.c `pkg-config --libs gtk+-3.0`
install:
	install -m 555 TypingTest /usr/bin/TypingTest
	install -d /usr/share/TypingTest/
	install -m 444 window.ui /usr/share/TypingTest/window.ui
	install -m 444 words.txt /usr/share/TypingTest/words.txt
	install -m 666 words.txt /usr/share/TypingTest/log.txt
	install -m 777 TypingTest.desktop /usr/share/applications/TypingTest.desktop
	
