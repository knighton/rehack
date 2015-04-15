ubuntu_setup:
	# TODO: get working with virtualenv.
	sudo apt-get install python-numpy python-scipy python-sklearn
	pip install numpy scipy scikit-learn

all:
	CFLAGS="-fdiagnostics-color" python setup.py build_ext -i
