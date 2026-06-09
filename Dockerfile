FROM debian:12

RUN apt-get update && apt-get install -y \
	make gcc gcc-multilib binutils \
	grub-pc-bin grub-common \
    xorriso && rm -rf /var/lib/apt/lists/*

WORKDIR /kfs

CMD ["make"]