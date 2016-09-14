#!/bin/sh

# alternative entrypoint for oneshot oneway syncs

set -e

export LOCAL_PATH=/srv/data/

# Perform the initial sync from S3 to the local system.
aws s3 cp "s3://${S3_BUCKET}/${S3_OBJECT}" "${LOCAL_PATH}/" "${S3_ARGS}"
