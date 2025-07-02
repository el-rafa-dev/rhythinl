# the SO
FROM archlinux:latest

# set the manteiner label
LABEL maintainer="El-Rafa-Dev <rafaelsousa12365@gmail.com"

# synchronize the pacman databases and install the wget and cmake without asking for confirmation with y (--noconfirm) && clean the pacman cache packages
RUN pacman -Sy && \
    pacman -S --noconfirm base-devel cmake wget && \ 
    rm -rf /var/cache/pacman/pkg/*

# sets the workdir as /app
WORKDIR /app

# copy all files and dirs to workdir ./app
COPY . ./app

# only for tests
# RUN ls -la app

# starts the build
RUN cd app && cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build

# the localhost port of docker
EXPOSE 3000

# go into the build directory
#RUN cd app/build && ./rhythin --version

# execute the rhythin within a rhythin file
CMD ["./app/build/rhythin", "-f", "./app/test/main.ry"]