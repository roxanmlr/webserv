from debian:bookworm

ENV DEBIAN_FRONTEND=noninteractive
ENV DISPLAY=:99

RUN apt-get update && apt-get install -y \
    build-essential \
    make \
	clang \
    g++ \
    firefox-esr \
    xvfb \
    x11vnc \
    fluxbox \
    novnc \
    websockify \
    iproute2 \
    iputils-ping \
    curl \
    net-tools \
    tcpdump \
    ca-certificates \
    dbus-x11 \
    x11-utils \
    python3 \
    procps \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY Makefile /app/Makefile
COPY src /app/src
COPY config /app/config/
COPY tests /app/tests/

RUN make fclean || true
RUN make
RUN cp /app/webserv /usr/local/bin/webserv && chmod +x /usr/local/bin/webserv

COPY docker/start.sh /start.sh
RUN chmod +x /start.sh

EXPOSE 6080

CMD [ "/start.sh" ]