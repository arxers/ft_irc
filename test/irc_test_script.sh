#!/bin/bash

# Variables
SESSION="irctest"
SERVER_IP="127.0.0.1"  # Update with your actual server IP
PORT="6667"             # Update with your desired port
PASSWORD="secret"        # Update with your server password
CHANNEL="#testingchannel123"

# Start a new tmux session
tmux new-session -d -s $SESSION

# Pane 1: Run the IRC server
tmux send-keys -t $SESSION "clear" C-m
tmux send-keys -t $SESSION "./ircserv $PORT $PASSWORD" C-m

# Split into 4 panes
tmux split-window -v -t $SESSION
tmux split-window -h -t $SESSION
tmux select-pane -t 0
tmux split-window -h -t $SESSION

# Pane 2: Connect as chanop using netcat
tmux select-pane -t 1
tmux send-keys -t $SESSION "clear" C-m
tmux send-keys -t $SESSION "nc -C $SERVER_IP $PORT" C-m
sleep 1
tmux send-keys -t $SESSION "PASS $PASSWORD" C-m
tmux send-keys -t $SESSION "NICK chanop" C-m
tmux send-keys -t $SESSION "USER chanop 0 * :Channel Operator" C-m
sleep 1
tmux send-keys -t $SESSION "JOIN $CHANNEL" C-m

# Pane 3: Connect as user1 using netcat
tmux select-pane -t 2
tmux send-keys -t $SESSION "clear" C-m
tmux send-keys -t $SESSION "nc -C $SERVER_IP $PORT" C-m
sleep 1
tmux send-keys -t $SESSION "PASS $PASSWORD" C-m
tmux send-keys -t $SESSION "NICK user1" C-m
tmux send-keys -t $SESSION "USER user1 0 * :User One" C-m
sleep 1
tmux send-keys -t $SESSION "JOIN $CHANNEL" C-m

# Pane 4: Connect as user2 using netcat
tmux select-pane -t 3
tmux send-keys -t $SESSION "clear" C-m
tmux send-keys -t $SESSION "nc -C $SERVER_IP $PORT" C-m
sleep 1
tmux send-keys -t $SESSION "PASS $PASSWORD" C-m
tmux send-keys -t $SESSION "NICK user2" C-m
tmux send-keys -t $SESSION "USER user2 0 * :User Two" C-m
sleep 1
tmux send-keys -t $SESSION "JOIN $CHANNEL" C-m

# Kick users from the channel using chanop
tmux select-pane -t 1
sleep 1
tmux send-keys -t $SESSION "KICK $CHANNEL user1 :Goodbye user1" C-m
tmux send-keys -t $SESSION "KICK $CHANNEL user2 :Goodbye user2" C-m

# Attach to the tmux session
tmux select-pane -t 0
tmux attach -t $SESSION
