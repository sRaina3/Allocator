.DEFAULT_GOAL := all

ASTYLE        := astyle
CHECKTESTDATA := checktestdata
CPPCHECK      := cppcheck
DOXYGEN       := doxygen
SHELL         := bash

ifeq ($(shell uname -s), Darwin)
    BOOST    := /usr/local/include/boost
    CXX      := clang++
    CXXFLAGS := --coverage -g -std=c++20 -I$(INCLUDE_PATH) -Wall -Wextra -Wpedantic
    GCOV     := llvm-cov gcov
    GTEST    := /usr/local/include/gtest
    LDFLAGS  := -lgtest -lgtest_main
    LIB      := $(LIBRARY_PATH)
    VALGRIND := valgrind-3.17
else ifeq ($(shell uname -p), unknown)
    BOOST    := /usr/include/boost
    CXX      := g++
    CXXFLAGS := --coverage -g -std=c++20 -Wall -Wextra -Wpedantic
    GCOV     := gcov
    GTEST    := /usr/include/gtest
    LDFLAGS  := -lgtest -lgtest_main -pthread
    LIB      := /usr/lib
    VALGRIND := valgrind
else
    BOOST    := /usr/local/opt/boost-1.67/include/boost
    CXX      := g++-11
    CXXFLAGS := --coverage -g -std=c++20 -Wall -Wextra -Wpedantic
    GCOV     := gcov-11
    GTEST    := /usr/local/include/gtest
    LDFLAGS  := -L/usr/local/opt/boost-1.77/lib/ -lgtest -lgtest_main -pthread
    LIB      := /usr/local/lib
    VALGRIND := valgrind-3.17
endif

# run docker
docker:
	docker run --rm -it -v $(PWD):/usr/gcc -w /usr/gcc gpdowning/gcc

# get git config
config:
	git config -l

# get git log
Allocator.log.txt:
	git log > Allocator.log.txt

# get git status
status:
	make clean
	@echo
	git branch
	git remote -v
	git status

# download files from the Allocator code repo
pull:
	make clean
	@echo
	git pull
	git status

# upload files to the Allocator code repo
push:
	make clean
	@echo
	git add .gitignore
	git add .gitlab-ci.yml
	git add Allocator.hpp
	-git add Allocator.log.txt
	-git add html
	git add Makefile
	git add README.md
	git add RunAllocator.cpp
	-git add RunAllocator.ctd.txt
	git add TestAllocator.cpp
	git commit -m "another commit"
	git push
	git status

# compile run harness
RunAllocator: Allocator.hpp RunAllocator.cpp
	-$(CPPCHECK) RunAllocator.cpp
	$(CXX) $(CXXFLAGS) RunAllocator.cpp -o RunAllocator

# compile test harness
TestAllocator: Allocator.hpp TestAllocator.cpp
	-$(CPPCHECK) TestAllocator.cpp
	$(CXX) $(CXXFLAGS) TestAllocator.cpp -o TestAllocator $(LDFLAGS)

# run/test files, compile with make all
FILES :=          \
    RunAllocator  \
    TestAllocator

# compile all
all: $(FILES)

# execute test harness with coverage
test: TestAllocator
	$(VALGRIND) ./TestAllocator
ifeq ($(shell uname -s), Darwin)
	$(GCOV) TestAllocator.cpp | grep -B 2 "hpp.gcov"
else
	$(GCOV) TestAllocator.cpp | grep -B 2 "hpp.gcov"
endif

# clone the Allocator test repo
../cs371p-allocator-tests:
	git clone https://gitlab.com/gpdowning/cs371p-allocator-tests.git ../cs371p-allocator-tests

# test files in the Allocator test repo
T_FILES := `ls ../cs371p-allocator-tests/*.in.txt`

# generate a random input file
ctd-generate:
	for v in {1..100}; do $(CHECKTESTDATA) -g RunAllocator.ctd.txt >> RunAllocator.gen.txt; done

# execute the run harness against a test file in the Collatz test repo and diff with the expected output
../cs371p-allocator-tests/%: RunAllocator
	$(CHECKTESTDATA) RunAllocator.ctd.txt $@.in.txt
	./RunAllocator < $@.in.txt > RunAllocator.tmp.txt
	diff RunAllocator.tmp.txt $@.out.txt

# execute the run harness against your test files in the Allocator test repo and diff with the expected output
run: ../cs371p-allocator-tests
	-make ../cs371p-allocator-tests/gpdowning-RunAllocator # change gpdowning to your GitLab-ID

# execute the run harness against all of the test files in the Allocator test repo and diff with the expected output
run-all: ../cs371p-allocator-tests
	-for v in $(T_FILES); do make $${v/.in.txt/}; done

# auto format the code
format:
	$(ASTYLE) Allocator.hpp
	$(ASTYLE) RunAllocator.cpp
	$(ASTYLE) TestAllocator.cpp

# you must edit Doxyfile and
# set EXTRACT_ALL     to YES
# set EXTRACT_PRIVATE to YES
# set EXTRACT_STATIC  to YES
# create Doxfile
Doxyfile:
	$(DOXYGEN) -g

# create html directory
html: Doxyfile
	$(DOXYGEN) Doxyfile

# check files, check their existence with make check
C_FILES :=            \
    .gitignore        \
    .gitlab-ci.yml    \
    Allocator.log.txt \
    html

# check the existence of check files
check: $(C_FILES)

# remove executables and temporary files
clean:
	rm -f  *.gcda
	rm -f  *.gcno
	rm -f  *.gcov
	rm -f  *.gen.txt
	rm -f  *.tmp.txt
	rm -f  RunAllocator
	rm -f  TestAllocator
	rm -rf *.dSYM

# remove executables, temporary files, and generated files
scrub:
	make clean
	rm -f  Allocator.log.txt
	rm -f  Doxyfile
	rm -rf html
	rm -rf latex

# output versions of all tools
versions:
	uname -p

	@echo
	uname -s

	@echo
	which $(ASTYLE)
	@echo
	$(ASTYLE) --version

	@echo
	which $(CHECKTESTDATA)
	@echo
	$(CHECKTESTDATA) --version | head -n 1

	@echo
	which $(CPPCHECK)
	@echo
	$(CPPCHECK) --version

	@echo
	which $(DOXYGEN)
	@echo
	$(DOXYGEN) --version

	@echo
	which $(CXX)
	@echo
	$(CXX) --version | head -n 1

	@echo
	which $(GCOV)
	@echo
	$(GCOV) --version | head -n 1

	@echo
	which git
	@echo
	git --version

	@echo
	which make
	@echo
	make --version | head -n 1

ifneq ($(VALGRIND),)
	@echo
	which $(VALGRIND)
	@echo
	$(VALGRIND) --version
endif

	@echo
	which vim
	@echo
	vim --version | head -n 1

	@echo
	grep "#define BOOST_LIB_VERSION " $(BOOST)/version.hpp

	@echo
	ls -al $(GTEST)/gtest.h
	@echo
	pkg-config --modversion gtest
	@echo
	ls -al $(LIB)/libgtest*.a
