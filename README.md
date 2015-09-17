#dwm - dynamic window manager
I run dwm with Xquartz on Mac OS X. It runs really well but needs a few little tricks to
get it to work smoothly.

##Running dwm
- Start a script from ~/.xinitrc

```
[[ -f ~/.Xresources ]] && xrdb -load ~/.Xresources
cd $HOME
PATH=$PATH:/usr/local/bin
~/Dropbox/dotfiles/Scripts/dwm-launch
```

- Start a script that fills the header and starts dwm

```
while true; do
	xsetroot -name "$($HOME/Dropbox/dotfiles/Scripts/weather.sh "PLACE") | $(ioreg -l | grep -i capacity | tr '\n' ' | ' | awk '{printf("%d", $10/$5 * 100)}')% | $(date +'%Y-%m-%d %H:%M')"
	sleep 1
done &

exec dwm
```

