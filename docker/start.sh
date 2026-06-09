#!/bin/bash
set -e

export DISPLAY=:99

rm -f /tmp/.X99-lock
rm -f /tmp/.X11-unix/X99

Xvfb :99 -screen 0 1280x720x24 -ac &
XVFB_PID=$!

for i in $(seq 1 20); do
    if xdpyinfo -display :99 >/dev/null 2>&1; then
        break
    fi
    sleep 0.2
done

if ! xdpyinfo -display :99 >/dev/null 2>&1; then
    echo "Xvfb failed to start"
    exit 1
fi

fluxbox &

# Faire de 211.0.0.0/8 un réseau local dans le conteneur
ip route add local 211.0.0.0/8 dev lo || true

/usr/local/bin/webserv /app/config/docker.conf &
WEBSERV_PID=$!

x11vnc -display :99 -nopw -forever -shared -rfbport 5900 &

websockify --web=/usr/share/novnc/ 6080 localhost:5900 &

firefox-esr --no-remote --new-instance http://211.15.23.17 &

wait $WEBSERV_PID