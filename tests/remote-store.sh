source common.sh

echo '*** testing slave mode ***'
clearStore
clearManifests
NIX_REMOTE=slave $SHELL ./user-envs.sh

echo '*** testing daemon mode ***'
clearStore
clearManifests
startDaemon
$SHELL ./user-envs.sh
killDaemon
