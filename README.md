# co-change-analysis
Extract and analysis co-changes in Go projects

**Warning**: :construction: Work in Progress :construction:

### Installation and usage
1. Install [python3](#), [virtualenv](#) and [git](#).

2. Init virtualenv and install dependencies

```
make init
```

3. Analysis a project

```
./main.py git@github.com:square/retrofit.git
```

### Usage with Docker
```
docker run -ti rodrigobrito/cochange -p git@github.com:square/retrofit.git
```
