# Extension build target directory.
EXT_DIR=panoptes/build/

# clang++ flags.
BASE_FLAGS=-std=c++11 -fcolor-diagnostics -O3
WARN_FLAGS=-Wall -Weverything -Wextra -Werror
DISABLE_FLAGS=-Wno-padded -Wno-c++98-compat-pedantic
FLAGS=$(BASE_FLAGS) $(WARN_FLAGS) $(DISABLE_FLAGS)

ubuntu_setup:
	# TODO: get working with virtualenv.
	sudo apt-get install python-numpy python-scipy python-sklearn
	pip install numpy scipy scikit-learn

clean:
	@rm -rf $(EXT_DIR)

all:
	@rm -rf $(EXT_DIR)
	@mkdir -p $(EXT_DIR)
	@touch $(EXT_DIR)/__init__.py
	@CFLAGS="$(FLAGS)" python setup.py build_ext --build-lib $(EXT_DIR) --compiler unix > /dev/null
