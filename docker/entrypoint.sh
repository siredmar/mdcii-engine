#!/bin/bash

# Add non root user
# Either use the LOCAL_USER_ID if passed in at runtime or
# fallback
USER_ID=${LOCAL_USER_ID:-8000}

echo "Starting with UID : ${USER_ID}"
useradd --shell /bin/bash -u "${USER_ID}" -o -m user
export HOME=/home/user

mkdir -p /build
chown user:user /build

exec gosu user "$@"
