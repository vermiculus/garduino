install:
	aptitude install ruby1.9.1-dev -y && sudo gem install t
	chmod +x garduino.py
clean:
	echo "The Watchdog is very cleanly!"
autostart:
	echo "@python ~/garduino.py" >> /etc/xdg/lxsession/LXDE/autostart
