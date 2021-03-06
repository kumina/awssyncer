FROM alpine:latest
MAINTAINER support@kumina.nl

# Supported environment variables:
# ENV AWS_ACCESS_KEY_ID ''
# ENV AWS_DEFAULT_REGION ''
# ENV AWS_SECRET_ACCESS_KEY ''
# ENV FILTER_REGEX ''
# ENV S3_BUCKET ''
# ENV SYNC_EXCLUDES ''

COPY docker_startup.sh /sbin/docker_startup.sh
COPY oneoffcopy.sh /sbin/oneoffcopy.sh
COPY . /tmp/awssyncer-src/

RUN apk --update upgrade \
 && apk add cmake g++ libstdc++ ninja python py-pip \
 && mkdir -p /srv/data \
 && pip install awscli \
 && cd /tmp/awssyncer-src/ \
 && cmake . -GNinja \
 && ninja \
 && ./awssyncer_test \
 && cp awssyncer /bin/awssyncer \
 && apk --purge -v del cmake g++ ninja py-pip \
 && rm -rf /tmp/awssyncer-src/ /var/cache/apk/*

VOLUME ["/srv/data"]
WORKDIR /
ENTRYPOINT ["/sbin/docker_startup.sh"]
