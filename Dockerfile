# Create a Docker container with Stateline and Geostate

FROM fedora:20


# Dependencies

RUN yum -y groupinstall "Development Libraries" "Development Tools"

RUN yum -y install	\
	bzip2		\
	bzip2-devel	\
	cmake		\
	gcc-c++		\
	numpy		\
	python3-devel 	\
	python-basemap	\
	python-shapely	\
	pyshp		\
	scipy		\
	wget


# Geostate prereqs

RUN cd /tmp && \
	wget http://ab-initio.mit.edu/nlopt/nlopt-2.4.1.tar.gz && \
	tar zxf nlopt-2.4.1.tar.gz && \
	cd nlopt-2.4.1 && \
	./configure --with-pic && \
	make -j$(nproc) && \
	make install && \
	rm -rf /tmp/nlopt-*

RUN cd /tmp && \
	wget http://download.osgeo.org/geos/geos-3.4.2.tar.bz2 && \
	tar xf geos-3.4.2.tar.bz2 && \
	cd geos-3.4.2 && \
	./configure && \
	make -j$(nproc) && \
	make install && \
	rm -rf /tmp/geos-*

RUN ldconfig


# Build Stateline prereqs

COPY stateline/prereqs /opt/stateline/prereqs

RUN cd /opt/stateline/prereqs && \
	./buildPrereqs.sh

ENV LD_LIBRARY_PATH /opt/stateline/prereqs/lib


# Build Stateline

COPY stateline /opt/stateline
RUN cd /opt/stateline && \
	./build.sh && \
	make -j$(nproc)


# Build Geostate

COPY geostate /opt/geostate

ENV PYTHONPATH /opt/geostate

RUN cd /opt/geostate/tectonic && \
	tar xzf data.tar.gz

RUN cd /opt/geostate && \
	make -j$(nproc)
