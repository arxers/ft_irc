#!/bin/bash

# Variables
SESSION="compare_servers"
LIBERA_IP="irc.libera.chat"
LIBERA_PORT="6667"
LOCAL_SERVER_IP="127.0.0.1"
LOCAL_SERVER_PORT="6668"
PASSWORD="secret"
#USERNICK="USERNICK12389"
#USERNICK="usernick12389"
USERNICK="usernick"

# Start a new tmux session
tmux new-session -d -s $SESSION

# Pane 1: Connect to Libera and authenticate
tmux send-keys -t $SESSION "nc -C $LIBERA_IP $LIBERA_PORT" C-m
sleep 1
tmux send-keys -t $SESSION "NICK liberaTest" C-m
tmux send-keys -t $SESSION "USER liberaTest 0 * :Testing on Libera" C-m
sleep 1
tmux send-keys $'\033c'

# Split into 4 panes
tmux split-window -v -t $SESSION
tmux split-window -h -t $SESSION
tmux select-pane -t 0
tmux split-window -h -t $SESSION

# Pane 2: Run your local IRC server
tmux select-pane -t 1
tmux send-keys -t $SESSION "./ircserv $LOCAL_SERVER_PORT $PASSWORD" C-m

# Pane 3: Connect to Libera Chat using netcat with CRLF
tmux select-pane -t 2
tmux send-keys -t $SESSION "nc -C $LIBERA_IP $LIBERA_PORT" C-m
sleep 1
tmux send-keys -t $SESSION "NICK $USERNICK" C-m
tmux send-keys -t $SESSION "USER $USERNICK 0 * :Testing Libera Client" C-m

# Pane 4: Connect to your server using netcat with CRLF
tmux select-pane -t 3
tmux send-keys -t $SESSION "nc -C $LOCAL_SERVER_IP $LOCAL_SERVER_PORT" C-m
sleep 1
tmux send-keys -t $SESSION "PASS $PASSWORD" C-m
tmux send-keys -t $SESSION "NICK $USERNICK" C-m
tmux send-keys -t $SESSION "USER $USERNICK 0 * :Testing Local Server" C-m


# Link panes 3 and 4 for synchronized input
# Synchronize input only between panes 3 and 4
tmux select-pane -t 2
tmux send-keys C-l

tmux set-window-option -t $SESSION synchronize-panes off
tmux select-pane -t 2
tmux send-keys C-l
tmux set-window-option -t $SESSION synchronize-panes on

sleep 1
tmux select-pane -t 1
tmux send-keys -t $SESSION "JOIN #testChanA" C-m
tmux send-keys -t $SESSION "JOIN #testChanA" C-m

# Attach to the session
tmux select-pane -t 0
tmux attach -t $SESSION

