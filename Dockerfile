FROM python:3.7-alpine

WORKDIR /app

RUN apk update \
    && apk add --no-cache ca-certificates wget git perl ncurses \
    && update-ca-certificates

RUN wget -P /usr/bin \
    https://raw.githubusercontent.com/so-fancy/diff-so-fancy/master/third_party/build_fatpack/diff-so-fancy \
    && chmod +x /usr/bin/diff-so-fancy

ADD . .
RUN pip install -r requirements.txt

ENTRYPOINT ["./main.py"]