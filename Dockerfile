FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TERM=xterm-256color
ENV IRSSI_HOST=172.17.0.1
ENV IRSSI_PORT=4343

RUN apt-get update \
	&& apt-get install -y --no-install-recommends ca-certificates gnupg2 wget \
	&& apt-get update \
	&& apt-get install -y --no-install-recommends irssi=1.2.3-1ubuntu4 \
	&& apt-mark hold irssi \
	&& apt-get remove -y --purge gnupg2 wget \
	&& apt-get autoremove -y \
	&& rm -rf /var/lib/apt/lists/* /usr/share/doc/* /usr/share/man/* /usr/share/locale/*

RUN useradd --create-home --shell /bin/bash irssiuser
USER irssiuser
WORKDIR /home/irssiuser

ENTRYPOINT ["sh","-lc","exec irssi -c \"${IRSSI_HOST}\" \"${IRSSI_PORT}\" \"$@\""]
CMD [""]