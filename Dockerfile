FROM ubuntu:focal

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y -q && \
    apt-get install -y -q --no-install-recommends \
      build-essential \
      cmake \
      pkg-config \
      ca-certificates \
      lsb-release \
      wget \
      unzip && \
    wget https://apache.jfrog.io/artifactory/arrow/$(lsb_release --id --short | tr 'A-Z' 'a-z')/apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb && \
    apt-get install -y -V ./apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb && \
    apt-get update -y -q && \
    apt-get install -y -q --no-install-recommends \
      libarrow-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists* && \
    rm apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb

COPY src /src

RUN cmake /src && \
    make

RUN wget https://github.com/USNISTGOV/MIST/wiki/testdata/Small_Fluorescent_Test_Dataset.zip && \
    unzip Small_Fluorescent_Test_Dataset.zip -d /data && \
    rm Small_Fluorescent_Test_Dataset.zip

ENTRYPOINT ["./filepattern", "img_r0{yy}_c00{x+}.tif", "/data/Small_Fluorescent_Test_Dataset/image-tiles/"]