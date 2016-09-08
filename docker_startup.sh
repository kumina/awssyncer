#!/bin/sh

set -e

export LOCAL_PATH=/srv/data/

# Perform the initial sync from S3 to the local system.
aws s3 sync --exclude .volume_ready "${S3_PATH}${LOCAL_PATH}" "${LOCAL_PATH}"

# Put a marker in place indicating that we've started up successfully.
touch /srv/data/.volume_ready

# Start up the AWS syncer to sync back from the local system to S3.
exec awssyncer
