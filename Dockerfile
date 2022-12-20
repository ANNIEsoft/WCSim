# Download base image from cern repo on docker hub
FROM cern/cc7-base:latest

### Run the following commands as super user (root):
USER root

### install prerequisites
RUN yum install -y cmake \
                   binutils \
                   make \
                   git \
                   tar \
                   wget \
                   devtoolset-3-gcc \
                   devtoolset-3-gcc-c++ \
                   devtoolset-3-gcc-gfortran \
                   which \
                   unzip \
                   cvs \
                   automake \
                   file \
                   patch \
                   bzip2-devel \
                   libxml2 \
                   libxml2-devel \
                   libtool \
                   gsl-devel \
                   expat-devel \
                   xerces-c \
                   xerces-c-devel \
                   libX11-devel \
                   libXmu-devel \
                   libGL-devel \
                   libGLU-devel \
                   libXpm-devel \
                   libXft \
                   libXft-devel \
                   osg-wn-client \
&& yum clean all \
&& rm -rf /var/cache/yum

# make a home area to install stuff in
# also requested for grid job use: create a bindpoint for /cvmfs
# (grid jobs automatically try to bind this)
# create a temp source file
RUN mkdir -p /home/annie && \
    mkdir /cvmfs && \
    touch sourceme && \
    echo '#!/bin/bash' > /sourceme && \
    echo '. /opt/rh/devtoolset-3/enable' >> /sourceme

# fsplit
RUN mkdir /home/annie/fsplit && cd /home/annie/fsplit && \
    wget https://gist.githubusercontent.com/marc1uk/c0e32d955dd1c06ef69d80ce643018ad/raw/10e592d42737ecc7dca677e774ae66dcb5a3859d/fsplit.c && \
    gcc fsplit.c -o fsplit && \
    echo 'export PATH=/home/annie/fsplit/:${PATH}' >> /sourceme

# log4cpp
RUN cd /home/annie && \
    cvs -d :pserver:anonymous@log4cpp.cvs.sourceforge.net:/cvsroot/log4cpp -z3 co log4cpp && \
    cd log4cpp/ && \
    mkdir install && \
    ./autogen.sh && \
    ./configure --prefix=/home/annie/log4cpp/install && \
    make && \
    make install && \
    echo 'export LD_LIBRARY_PATH=/home/annie/log4cpp/install/lib:${LD_LIBRARY_PATH}' >> /sourceme && \
    echo 'export LOG4CPP_FQ_DIR=/home/annie/log4cpp/install' >> /sourceme

# Pythia6
# dummies must be removed for LHAPDF which is needed for Reweight package
RUN source /sourceme && cd /home/annie/ && \
    git clone https://github.com/GENIE-MC-Community/Pythia6Support.git && \
    cd Pythia6Support && \
    ./build_pythia6.sh --dummies=remove && \
    echo 'export PYTHIA6_DIR=/home/annie/Pythia6Support/v6_424/' >> /sourceme && \
    echo 'export PYTHIA6_INCLUDE_DIR=/home/annie/Pythia6Support/v6_424/inc/' >> /sourceme && \
    echo 'export PYTHIA6_LIBRARY=/home/annie/Pythia6Support/v6_424/lib/' >> /sourceme && \
    echo 'export LD_LIBRARY_PATH=/home/annie/Pythia6Support/v6_424/lib:${LD_LIBRARY_PATH}' >> /sourceme

# build Geant
RUN mkdir /home/annie/Geant4 && source /sourceme && \
    cd /home/annie/Geant4 && \
    wget http://cern.ch/geant4-data/releases/geant4.10.01.p03.tar.gz && \
    tar -xzf geant4.10.01.p03.tar.gz && \
    rm geant4.10.01.p03.tar.gz && \
    mkdir /home/annie/Geant4/install /home/annie/Geant4/build && \
    cd /home/annie/Geant4/build && \
    cmake -DCMAKE_INSTALL_PREFIX=/home/annie/Geant4/install -DGEANT4_INSTALL_DATADIR=/home/annie/Geant4/install/share/data -DGEANT4_BUILD_MULTITHREADED=ON -DGEANT4_BUILD_CXXSTD=c++11 -DGEANT4_USE_GDML=ON -DGEANT4_USE_OPENGL_X11=ON -DGEANT4_INSTALL_DATA=ON /home/annie/Geant4/geant4.10.01.p03 -DGEANT4_INSTALL_DATA=ON && \
    make -j8 && \
    make install && \
    echo 'source /home/annie/Geant4/install/bin/geant4.sh' >> /sourceme && \
    rm -r /home/annie/Geant4/geant4.10.01.p03 && \
    rm -r /home/annie/Geant4/build

# build ROOT
RUN mkdir /home/annie/ROOT && source /sourceme && \
    cd /home/annie/ROOT && \
    wget https://root.cern/download/root_v6.06.08.source.tar.gz && \
    tar -xzf root_v6.06.08.source.tar.gz && \
    rm root_v6.06.08.source.tar.gz && \
    mkdir /home/annie/ROOT/install /home/annie/ROOT/build && \
    cd /home/annie/ROOT/build && \
    cmake -DCMAKE_INSTALL_PREFIX=/home/annie/ROOT/install -Dcxx11=ON -Dgdml=ON -Dxml=ON \
    -Dmathmore=ON -Dmt=ON -Dx11=ON -Dpythia6=ON -Dimt=ON -Dtmva=ON -Dfftw3=ON \
    -DCMAKE_BUILD_TYPE=Rel /home/annie/ROOT/root-6.06.08 && \
    make -j8 && \
    make install && \
    echo 'source /home/annie/ROOT/install/bin/thisroot.sh' >> /sourceme && \
    rm -r /home/annie/ROOT/root-6.06.08 && \
    rm -r /home/annie/ROOT/build

# LHAPDF
RUN source /sourceme && cd /home/annie && \
    wget http://lhapdf.hepforge.org/downloads/old/lhapdf-5.9.1.tar.gz && \
    tar zxf lhapdf-5.9.1.tar.gz && \
    rm -rf lhapdf-5.9.1.tar.gz && \
    cd lhapdf-5.9.1 && \
    mkdir install && \
    ./configure --prefix=/home/annie/lhapdf-5.9.1/install && \
    make && \
    make install && \
    /bin/bash -O extglob -c 'rm -rf !(install)' && \
    echo 'export PATH=/home/annie/lhapdf-5.9.1/install/bin:${PATH}' >> /sourceme && \
    echo 'export export LD_LIBRARY_PATH=/home/annie/lhapdf-5.9.1/install/lib:${LD_LIBRARY_PATH}' >> /sourceme

# GENIE
# env variables need to be set before configuration/installation
RUN source /sourceme && mkdir /home/annie/Genie && \
    cd /home/annie/Genie && \
    wget https://github.com/ANNIEsoft/GENIE-v3/archive/refs/heads/master.zip && \
    unzip master.zip && \
    rm -rf master.zip && \
    cd GENIE-v3-master/ && \
    mkdir install && \
    export GENIE="/home/annie/Genie/GENIE-v3-master" && \
    ./configure --prefix=/home/annie/Genie/GENIE-v3-master/install/ \
    --with-pythia6-lib=/home/annie/Pythia6Support/v6_424/lib/ \
    --with-log4cpp-inc=/home/annie/log4cpp/install/include/ \
    --with-log4cpp-lib=/home/annie/log4cpp/install/lib/ && \
    make -j4 && \
    make install && \
    /bin/bash -O extglob -c 'rm -rf !(install)' && \
    echo 'export GENIE=/home/annie/Genie/GENIE-v3-master/install' >> /sourceme && \
    echo 'export PATH=${GENIE}/bin:${PATH}' >> /sourceme && \
    echo 'export LD_LIBRARY_PATH=${GENIE}/lib:${LD_LIBRARY_PATH}' >> /sourceme

# build WCSim
RUN source /sourceme && mkdir /home/annie/WCSim && \
    cd /home/annie/WCSim && \
    git clone https://github.com/ANNIEsoft/WCSim.git WCSim && \
    cd /home/annie/WCSim/WCSim && \
    git checkout annie && \
    source /home/annie/Geant4/install/bin/geant4.sh && \
    source /home/annie/ROOT/install/bin/thisroot.sh && \
    make rootcint && \
    make
RUN source /sourceme && mkdir /home/annie/WCSim/build && \
    cd /home/annie/WCSim/build && \
    source /home/annie/Geant4/install/bin/geant4.sh && \
    source /home/annie/ROOT/install/bin/thisroot.sh && \
    cmake ../WCSim && \
    make -j8

RUN echo "#!/bin/bash"                                      > /home/annie/WCSim/WCSim/sourceme  && \
    echo 'export LOG4CPP_FQ_DIR=/home/annie/log4cpp/install' >> /home/annie/WCSim/WCSim/sourceme && \
    echo 'export PYTHIA6_DIR=/home/annie/Pythia6Support/v6_424/' >> /home/annie/WCSim/WCSim/sourceme && \
    echo 'export PYTHIA6_INCLUDE_DIR=/home/annie/Pythia6Support/v6_424/inc/' >> /home/annie/WCSim/WCSim/sourceme && \
    echo 'export PYTHIA6_LIBRARY=/home/annie/Pythia6Support/v6_424/lib/' >> /home/annie/WCSim/WCSim/sourceme && \
    echo 'export GENIE=/home/annie/Genie/GENIE-v3-master/install' >> /home/annie/WCSim/WCSim/sourceme && \
    echo 'source /opt/rh/devtoolset-3/enable'              >> /home/annie/WCSim/WCSim/sourceme && \
    echo "source /home/annie/Geant4/install/bin/geant4.sh" >> /home/annie/WCSim/WCSim/sourceme && \
    echo "source /home/annie/ROOT/install/bin/thisroot.sh" >> /home/annie/WCSim/WCSim/sourceme && \
    echo "source /home/annie/WCSim/WCSim/envHadronic.sh"   >> /home/annie/WCSim/WCSim/sourceme && \
    echo '#export NO_GENIE=1 #ALLOWS WCSIM TO BE BUILT WITHOUT GENIE' >> /home/annie/WCSim/WCSim/sourceme && \
    echo 'export PATH=${GENIE}/bin:/home/annie/lhapdf-5.9.1/install/bin:/home/annie/fsplit/:${PATH}' >> /home/annie/WCSim/WCSim/sourceme && \
    echo 'export LD_LIBRARY_PATH=/home/annie/WCSim/WCSim:${GENIE}/lib:/home/annie/lhapdf-5.9.1/install/lib:/home/annie/Pythia6Support/v6_424/lib:/home/annie/log4cpp/install/lib:$LD_LIBRARY_PATH' >> /home/annie/WCSim/WCSim/sourceme && \
    echo 'export ROOT_INCLUDE_PATH=/home/annie/WCSim/WCSim/include:$ROOT_INCLUDE_PATH' >> /home/annie/WCSim/WCSim/sourceme && \
    rm /sourceme

RUN echo "#!/bin/bash"                                                                >  /home/annie/setup  && \
    echo "PS1='\[\033[00;32m\]\u@WCSim\[\033[00m\]:\[\033[00;36m\]\w\[\033[00m\]\$ '" >> /home/annie/setup  && \
    echo "source /home/annie/WCSim/WCSim/sourceme"                                    >> /home/annie/setup  && \
    echo "export LIBGL_ALWAYS_INDIRECT=1"                                             >> /home/annie/setup  && \
    echo "alias root='root -l'"                                                       >> /home/annie/setup  && \
    echo "alias cp='cp -i'"                                                           >> /home/annie/setup  && \
    echo "alias mv='mv -i'"                                                           >> /home/annie/setup  && \
    echo "alias diff='diff -s'"                                                       >> /home/annie/setup

### Open terminal
WORKDIR /home/annie/WCSim
ENTRYPOINT ["/bin/bash", "--rcfile", "/home/annie/setup"]
#CMD ["/home/annie/WCSim/build/WCSim"]
