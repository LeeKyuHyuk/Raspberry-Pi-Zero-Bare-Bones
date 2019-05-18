# Chapter 1: Cross Compiler를 빌드하고, 빌드환경을 구축해보자!

## Build Cross Compiler

> 크로스 컴파일러(Cross Compiler)는 컴파일러가 실행되는 플랫폼이 아닌 다른 플랫폼에서 실행 가능한 코드를 생성할 수 있는 컴파일러이다.  
크로스 컴파일러 툴은 임베디드 시스템 혹은 여러 플랫폼에서 실행파일을 생성하는데 사용된다.
이것은 운영 체제를 지원하지 않는 마이크로컨트롤러와 같이 컴파일이 실현 불가능한 플랫폼에 컴파일하는데 사용된다.  
이것은 시스템이 사용하는데 하나 이상의 플랫폼을 쓰는 반가상화에 이 도구를 사용하는 것이 더 일반적이게 되었다.  
[Wikipedia - 크로스 컴파일러](https://ko.wikipedia.org/wiki/크로스%20컴파일러)

![GNU GCC Cross Compiler](./images/GNU-GCC-Cross-Compiler.png)  
Picture Source : *[Preshing on Programming - How to Build a GCC Cross-Compiler
](https://preshing.com/20141119/how-to-build-a-gcc-cross-compiler)*

### Step 1. Download Source code

```sh
wget https://ftp.gnu.org/gnu/binutils/binutils-2.32.tar.xz
wget https://ftp.gnu.org/gnu/gcc/gcc-9.1.0/gcc-9.1.0.tar.xz
wget https://ftp.gnu.org/gnu/mpfr/mpfr-4.0.2.tar.xz
wget https://ftp.gnu.org/gnu/gmp/gmp-6.1.2.tar.xz
wget https://ftp.gnu.org/gnu/mpc/mpc-1.1.0.tar.gz
```

### Step 2. Build GNU Binutils

> GNU 바이너리 유틸리티(GNU Binary Utilities) 또는 GNU Binutils는 여러 종류의 오브젝트 파일 형식들을 조작하기 위한 프로그래밍 도구 모음이다.  
> [Wikipedia - GNU 바이너리 유틸리티](https://ko.wikipedia.org/wiki/GNU_바이너리_유틸리티)

```sh
tar xvJf binutils-2.32.tar.xz
cd binutils-2.32
./configure \
--target=arm-none-eabi \
--prefix=/usr/local/cross-compiler \
--disable-nls \
--disable-werror \
--disable-shared
make configure-host
make
make install
```

- `--target`: 컴파일해서 만들어진 바이너리가 만들어내는 바이너리가 실행되는 시스템
- `--prefix` : PREFIX에 binutils를 설치합니다.
  - 원하는 경로를 입력하면 됩니다. 저는 `/home/leekyuhyuk/workspace/tools`로 입력하였습니다.
- `--disable-shared` : Static Library로 빌드합니다.
- `--disable-nls` : 모든 메세지를 영어로 출력합니다.

### Step 3. Build GNU GCC

```sh
tar xvJf gcc-9.1.0.tar.xz
tar xvJf mpfr-4.0.2.tar.xz
tar xvJf gmp-6.1.2.tar.xz
tar xvzf mpc-1.1.0.tar.gz
mv -v mpfr-4.0.2 gcc-9.1.0/mpfr
mv -v gmp-6.1.2 gcc-9.1.0/gmp
mv -v mpc-1.1.0 gcc-9.1.0/mpc
cd gcc-9.1.0
./configure \
--target=arm-none-eabi \
--prefix=/usr/local/cross-compiler \
--disable-nls \
--enable-languages=c \
--without-headers \
--disable-shared
make configure-host
make all-gcc
make install-gcc
```

- `--enable-languages` : Compiler나 Runtime Library들이 어떤걸로 빌드되어야 할지 설정합니다
