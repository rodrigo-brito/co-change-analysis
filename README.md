# co-change-analysis

**Warning**: :construction: Work in Progress :construction:

Extract and analysis co-changes in Go, Python and Java projects

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

### Usage with Docker
```
docker run -ti rodrigobrito/cochange -l go -r https://github.com/gorilla/mux.git  -s 0.001 -c 0.1
```
