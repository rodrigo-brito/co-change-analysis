# co-change-analysis

Extract and analysis co-changes in Go and C projects

### Installation and usage
1. Install [python3](https://www.python.org/downloads/), [virtualenv](https://virtualenv.pypa.io/en/latest/installation/) and [git](https://git-scm.com/downloads).

2. Init virtualenv and install dependencies

```
make init
```

3. Analysis a project

```
./main.py --lang go -r https://github.com/gorilla/mux.git  --support 0.001 --confidence 0.1
```
