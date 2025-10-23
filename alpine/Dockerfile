FROM docker.io/i386/alpine:3.21.0

ENV KERNEL=virt
RUN apk add --no-cache openrc alpine-base agetty alpine-conf libc-dev linux-$KERNEL linux-firmware-none gcc make fuse3 git pkgconf fuse3-dev

RUN sed -i 's/getty 38400 tty1/agetty --autologin root tty1 linux/' /etc/inittab
RUN echo 'ttyS0::respawn:/sbin/agetty --autologin root -s ttyS0 115200 vt100' >> /etc/inittab
RUN echo "root:" | chpasswd

RUN setup-hostname localhost

RUN git clone https://github.com/cosmo-ray/cringy-file.git /root/cringe-file
RUN cd /root/cringe-file && make
RUN cp /root/cringe-file/cringe-fs /bin/

RUN echo -e "#!/bin/sh\ninsmod ./fuse.ko\nmkdir cringe-space\ncringe-fs cringe-space\necho look at what s in cringe-space/ hint: ls/cat/echo" > root/start-game.sh

RUN chmod +x root/start-game.sh

RUN echo -e "Hello, welcome\nto play this game you need to do './start-game.sh'\nthis will create the directory cringe-space, this directory is where the game happen, look at files insides, and make cringy happy :)\nList files: ls (so ls cringe-space for cringy directory)\nlook at a file: cat A_FILE\nremove a file: rm FILE\noutput stuff in a file: echo something > DST_FILE\nBTW you can use vi" > /etc/motd

RUN apk del -f git gcc pkgconf fuse3-dev libc-dev make

RUN gunzip /lib/modules/6.12.40-0-virt/kernel/fs/fuse/*ko.gz

RUN cp /lib/modules/6.12.40-0-virt/kernel/fs/fuse/* /root

# https://wiki.alpinelinux.org/wiki/Alpine_Linux_in_a_chroot#Preparing_init_services
RUN for i in devfs dmesg mdev hwdrivers; do rc-update add $i sysinit; done
RUN for i in hwclock modules sysctl hostname syslog bootmisc; do rc-update add $i boot; done
RUN rc-update add killprocs shutdown

# Generate initramfs with 9p modules
RUN mkinitfs -F "base virtio 9p" $(cat /usr/share/kernel/$KERNEL/kernel.release)

RUN rm -rvf /root/cringe-file /lib/modules/6.12.40-0-virt/kernel/net* /etc/ssl /usr/share/apk /lib/modules/6.12.40-0-virt/kernel/drivers/net* /usr/sbin/setup-*
