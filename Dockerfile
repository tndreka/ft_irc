# Use the matching Ubuntu release (Jammy)
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TERM=xterm-256color
ENV IRSSI_HOST=172.17.0.1
ENV IRSSI_PORT=4343

# Install the exact irssi package version, avoid recommends, and clean caches
RUN apt-get update \
	&& apt-get install -y --no-install-recommends ca-certificates gnupg2 wget \
	&& apt-get update \
	# install the exact version (will fail if unavailable for the architecture)
	&& apt-get install -y --no-install-recommends irssi=1.2.3-1ubuntu4 \
	&& apt-mark hold irssi \
	# cleanup apt caches and unnecessary files to make the image smaller
	&& apt-get remove -y --purge gnupg2 wget \
	&& apt-get autoremove -y \
	&& rm -rf /var/lib/apt/lists/* /usr/share/doc/* /usr/share/man/* /usr/share/locale/*

# run as an unprivileged user and use an inline shell ENTRYPOINT so no extra script is needed
RUN useradd --create-home --shell /bin/bash irssiuser
USER irssiuser
WORKDIR /home/irssiuser

# Default CMD is empty; ENTRYPOINT uses IRSSI_HOST/IRSSI_PORT env vars (can be overridden with -e)
ENTRYPOINT ["sh","-lc","exec irssi -c \"${IRSSI_HOST}\" \"${IRSSI_PORT}\" \"$@\""]
CMD [""]