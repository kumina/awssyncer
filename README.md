# awssyncer: an inotify-based automatic syncer for Amazon AWS S3

This repository provides a utility called awssyncer. awssyncer uses
Linux inotify to track changes to a directory structure, syncing any
modifcations into an Amazon AWS S3 bucket. It makes use of the AWS
command line interface utility, `aws`, invoking it either to add or
remove files and directories. awssyncer is written in C++, making use of
some C++14 and C++17 language features.

# Running awssyncer

This utility obtains its configuration through environment variables:

- `LOCAL_PATH`: The local directory that should be synchronised. This
  path needs to be absolute and must end with a trailing slash. When
  running this utility using Docker, it is always set to `/srv/data/`.
- `S3_BUCKET`: The name of the AWS S3 bucket to which data should be
  synchronised.
- `FILTER_REGEX`: Regular expression of files/directories whose activity
  should be monitored. This does not mean that these files will not end
  up in S3. It only applies to monitoring; not synchronisation.

Credentials for accessing AWS can be configured by exporting the
canonical AWS environment variables (e.g., `AWS_ACCESS_KEY_ID` and
`AWS_SECRET_ACCESS_KEY`).

# Docker

This repository also provices a simple Docker file that creates a simple
installation based on Alpine Linux. After building the awssyncer, it
also invokes the awssyncer unit tests.
